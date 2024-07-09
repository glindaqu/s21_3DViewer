#include "viewer-app-window.h"

#include <epoxy/gl.h>

#include "../../include/matrix_calc.h"
#include "../../include/parser.h"
#include "viewer-app-settings.h"
#include "viewer-glfuncs.h"

struct _ViewerAppWindow {
  GtkApplicationWindow parent_instance;

  GtkHeaderBar *header_bar;
  GSettings *settings;

  mvp_matrix_movement_t matrix_movement;

  GtkWidget *primary_menu;
  GtkButton *open_button;

  ObjFile_t *obj_file;

  gl_matrix *mvp_matrix;

  GtkWidget *gl_drawing_area;

  GtkLabel *model_name_label;

  GtkScale *x_scale;
  GtkScale *y_scale;
  GtkScale *z_scale;

  /* GL objects */
  guint vao;
  guint vbo;
  guint ebo;
  guint program;
  guint mvp_location;
  GLint loc_res;
  GLint loc_factor;
  GLint loc_pattern;
  GLint loc_thickness;
  float lineColor[3];

  guint projection_index;
  guint color_index;

  GLushort pattern;
  GLfloat factor;

  /* Mouse state */
  gboolean alt_key;
  gboolean mouse_dragging;
  double last_mouse_x;
  double last_mouse_y;
};

struct _ViewerAppWindowClass {
  GtkApplicationWindowClass parent_class;
};

G_DEFINE_TYPE(ViewerAppWindow, viewer_app_window, GTK_TYPE_APPLICATION_WINDOW)

static void gl_init(ViewerAppWindow *self) {
  gtk_gl_area_make_current (GTK_GL_AREA(self->gl_drawing_area));

  GError *error = gtk_gl_area_get_error(GTK_GL_AREA(self->gl_drawing_area));
  if (error != NULL) {
    g_warning(
      "GL area error: %s", error->message);
    g_error_free(error);
    return;
  }
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  init_buffers(self->obj_file, 0, 0, &self->vao, &self->vbo, &self->ebo);

  error = NULL;
  if (!init_shaders(&self->program, &self->mvp_location,
                    &self->projection_index, &self->color_index, &self->loc_res, &self->loc_pattern, &self->loc_factor , &self->loc_thickness, &error)) {
    gtk_gl_area_set_error(GTK_GL_AREA(self->gl_drawing_area), error);
    g_error_free(error);
    return;
  }
}

static void gl_fini(ViewerAppWindow *self) {
  gtk_gl_area_make_current(GTK_GL_AREA(self->gl_drawing_area));
  
  if (gtk_gl_area_get_error(GTK_GL_AREA(self->gl_drawing_area)) != NULL) return;

  if (self->vao != 0) glDeleteVertexArrays(1, &self->vao);
  if (self->vbo != 0) glDeleteBuffers(1, &self->vbo);
  if (self->ebo != 0) glDeleteBuffers(1, &self->ebo);
  if (self->program != 0) glDeleteProgram(self->program);
}

static void gl_model_draw(ViewerAppWindow *self) {
  glUseProgram(self->program);

  glUniform3fv(self->loc_thickness, 1, self->lineColor);
  glUniform1ui(self->loc_pattern, self->pattern);
  glUniform1f(self->loc_factor, self->factor);
  glUniform2f(self->loc_res, 800.f, 800.f);
  
  glUniformMatrix4fv(self->mvp_location, 1, GL_FALSE,
                     &self->mvp_matrix->mvp[0]);
                     
  glBindVertexArray(self->vao);

  glDrawElements(GL_LINES, self->obj_file->surfacesCount * 6, GL_UNSIGNED_INT,
                 0);

  glBindVertexArray(0);
  glUseProgram(0);
}

static gboolean gl_draw(ViewerAppWindow *self) {
  glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gl_model_draw(self);

  glFlush();

  return TRUE;
}

typedef struct {
  GtkLabel *model_name_label;
  ObjFile_t *obj_file;
} AppData;

const char *format_number(int number) {
  static char buffer[20];
  if (number >= 1000000) {
    snprintf(buffer, sizeof(buffer), "%.1fM", number / 1000000.0);
  } else if (number >= 1000) {
    snprintf(buffer, sizeof(buffer), "%.1fK", number / 1000.0);
  } else {
    snprintf(buffer, sizeof(buffer), "%d", number);
  }
  return buffer;
}

void update_label_text(AppData *self) {
  const char *basename = g_path_get_basename(self->obj_file->fileName);
  const char *formatted_vertices = format_number(self->obj_file->verticesCount);
  const char *formatted_surfaces = format_number(self->obj_file->surfacesCount);
  char *model_name;
  if (strlen(basename) > 9) {
    model_name = g_strdup_printf("*...[.obj]");
  } else {
    model_name = g_strdup_printf("%s", basename);
  }

  char *label_text =
      g_strdup_printf("Model: %s, Vertices: %3s, Surfaces: %3s", model_name,
                      formatted_vertices, formatted_surfaces);
  gtk_label_set_text(GTK_LABEL(self->model_name_label), label_text);
  g_free(label_text);
  g_free(model_name);
}

static void open_file(ViewerAppWindow *self, GFile *file) {
  if (file == NULL) {
    g_printerr("Unable to open file\n");
    return;
  }

  if (self->obj_file->fileName != NULL) {
    removeObjFile(self->obj_file);
    initParser(self->obj_file);
  }

  self->obj_file->fileName = g_file_peek_path(file);

  if (parse(self->obj_file) != OK) {
    g_printerr("Unable to parse “%s”\n", g_file_peek_path(file));
    return;
  }
  
  gtk_gl_area_make_current(GTK_GL_AREA(self->gl_drawing_area));

  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    g_warning("OpenGL error before glDrawElements: %d\n", err);
  }

  GError *error = gtk_gl_area_get_error(GTK_GL_AREA(self->gl_drawing_area));
  if (error != NULL) {
    g_warning("GL area error: %s", error->message);
    g_error_free(error);
    return;
  }

  if (self->vao != 0) glDeleteVertexArrays(1, &self->vao);
  if (self->vbo != 0) glDeleteBuffers(1, &self->vbo);
  if (self->ebo != 0) glDeleteBuffers(1, &self->ebo);

  init_buffers(self->obj_file, self->projection_index, self->color_index,
               &self->vao, &self->vbo, &self->ebo);

  init_mvp_matrix(self->mvp_matrix);
  gtk_range_set_value(GTK_RANGE(self->x_scale), 0);
  gtk_range_set_value(GTK_RANGE(self->y_scale), 0);
  gtk_range_set_value(GTK_RANGE(self->z_scale), 0);

  AppData app_data;
  app_data.obj_file = self->obj_file;
  app_data.model_name_label = GTK_LABEL(self->model_name_label);

  update_label_text(&app_data);

  gtk_widget_queue_draw(self->gl_drawing_area);
}

static void on_open_response(GObject *source, GAsyncResult *result,
                             gpointer user_data) {
  GtkFileDialog *dialog = GTK_FILE_DIALOG(source);
  ViewerAppWindow *self = user_data;

  g_autoptr(GFile) file = gtk_file_dialog_open_finish(dialog, result, NULL);

  if (file != NULL) open_file(self, file);
}

static void viewer_app_window__open_file_dialog(GAction *action,
                                                GVariant *parameter,
                                                ViewerAppWindow *self) {
  g_autoptr(GtkFileDialog) dialog = gtk_file_dialog_new();

  gtk_file_dialog_open(dialog, GTK_WINDOW(self), NULL, on_open_response, self);
}

static void gl_button_press_event(GtkGestureClick *gesture, int n_press,
                                  double x, double y, ViewerAppWindow *self) {
  if (gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture)) ==
      GDK_BUTTON_PRIMARY) {
    self->mouse_dragging = TRUE;
    self->last_mouse_x = x;
    self->last_mouse_y = y;
  }
}

static void gl_button_release_event(GtkGestureClick *gesture, int n_press,
                                    double x, double y, ViewerAppWindow *self) {
  if (gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture)) ==
      GDK_BUTTON_PRIMARY) {
    self->mouse_dragging = FALSE;
  }
}

static void gl_motion_notify_event(GtkEventControllerMotion *controller,
                                   double x, double y, ViewerAppWindow *self) {
  if (self->mouse_dragging) {
    double dx = x - self->last_mouse_x;
    double dy = y - self->last_mouse_y;

    self->matrix_movement.rotation_angles[X_AXIS] += dy * 0.5;
    self->matrix_movement.rotation_angles[Y_AXIS] += dx * 0.5;

    move_mvp_matrix(self->mvp_matrix, &self->matrix_movement);

    self->last_mouse_x = x;
    self->last_mouse_y = y;

    gtk_widget_queue_draw(self->gl_drawing_area);
  }
}

static void gl_scroll_event(GtkEventControllerScroll *controller, double dx,
                            double dy, ViewerAppWindow *self) {
  GdkModifierType state = gtk_event_controller_get_current_event_state(
      GTK_EVENT_CONTROLLER(controller));
  if (state & GDK_SHIFT_MASK) {
    self->matrix_movement.scale_vector[X_AXIS] += dy;
    self->matrix_movement.scale_vector[Y_AXIS] += dy;
    self->matrix_movement.scale_vector[Z_AXIS] += dy;

    if (self->matrix_movement.scale_vector[X_AXIS] < 0.1) {
      self->matrix_movement.scale_vector[X_AXIS] = 0.1;
    }
    if (self->matrix_movement.scale_vector[Y_AXIS] < 0.1) {
      self->matrix_movement.scale_vector[Y_AXIS] = 0.1;
    }
    if (self->matrix_movement.scale_vector[Z_AXIS] < 0.1) {
      self->matrix_movement.scale_vector[Z_AXIS] = 0.1;
    }

    move_mvp_matrix(self->mvp_matrix, &self->matrix_movement);

    gtk_widget_queue_draw(self->gl_drawing_area);

  } else if (state & GDK_ALT_MASK) {
    self->matrix_movement.rotation_angles[Z_AXIS] += (dx + dy) * 5.0;

    move_mvp_matrix(self->mvp_matrix, &self->matrix_movement);

    gtk_widget_queue_draw(self->gl_drawing_area);
  }
}

static void on_scale_value_changed(GtkRange *range, ViewerAppWindow *self) {
  self->matrix_movement.translation_vector[X_AXIS] =
      gtk_range_get_value(GTK_RANGE(self->x_scale));
  self->matrix_movement.translation_vector[Y_AXIS] =
      gtk_range_get_value(GTK_RANGE(self->y_scale));
  self->matrix_movement.translation_vector[Z_AXIS] =
      gtk_range_get_value(GTK_RANGE(self->z_scale));

  move_mvp_matrix(self->mvp_matrix, &self->matrix_movement);

  gtk_widget_queue_draw(self->gl_drawing_area);
}

static void open_settings_dialog(GAction *action, GVariant *parameter,
                                 ViewerAppWindow *self) {
  g_print("Opening settings dialog\n");
  ViewerAppSettings *settings_dialog = viewer_app_settings_new(self);
  gtk_window_present(GTK_WINDOW(settings_dialog));
}

void set_orthographic_projection(float left, float right, float bottom,
                                 float top, float near, float far) {
  // glMatrixMode(GL_PROJECTION);
  // glLoadIdentity();
  // glOrtho(left, right, bottom, top, near, far);
  // glMatrixMode(GL_MODELVIEW);
}

void set_perspective_projection(float left, float right, float bottom,
                                float top, float near, float far) {
  // glMatrixMode(GL_PROJECTION);
  // glLoadIdentity();
  // glFrustum(left, right, bottom, top, near, far);
  // glMatrixMode(GL_MODELVIEW);
}

void set_edge_thickness(float thickness) {
  GLfloat lineWidthRange[2] = {0.0f, 0.0f};
  glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);
  g_print("Setting edge thickness to %f\n", thickness);
  glLineWidth(thickness / 10.f);
}

void set_edge_strippled(ViewerAppWindow *self) {
  self->pattern = 0x18ff;
  self->factor = 2.f;
} 

void set_edge_not_strippled(ViewerAppWindow *self) {
  self->pattern = 0xffff;
  self->factor = 1.f;
}

static void apply_projection_type_setting(ViewerAppWindow *self) {
  const gchar *projection_type = g_settings_get_string(self->settings, "projection-type");
  if (g_strcmp0(projection_type, "Parallel") == 0) {
    set_orthographic_projection(-1.0, 1.0, -1.0, 1.0, 5, 100);
  } else if (g_strcmp0(projection_type, "Central") == 0) {
    set_perspective_projection(-1.0, 1.0, -1.0, 1.0, 5, 100);
  }
}

static void apply_edge_type_setting(ViewerAppWindow *self) {
  const gchar *edge_type = g_settings_get_string(self->settings, "edge-type");
  if (g_strcmp0(edge_type, "Solid") == 0) {
    set_edge_not_strippled(self);
  } else if (g_strcmp0(edge_type, "Dashed") == 0) {
    set_edge_strippled(self);
  }
}


static void apply_edge_thickness_setting(ViewerAppWindow *self) {
  set_edge_thickness(g_settings_get_int(self->settings, "edge-thickness"));
}

static void set_line_color(const GVariant *color_variant, float* lineColor) {
  gchar **color_str_array = g_variant_get_strv(color_variant, NULL);
  for (int i = 0; i < 3; i++) {
    lineColor[i] = g_ascii_strtod(color_str_array[i], NULL);
  }
  g_strfreev(color_str_array);
  g_variant_unref(color_variant);
}

static void apply_line_color_setting(ViewerAppWindow *self) {
  GVariant *color_variant = g_settings_get_value(self->settings, "line-color");
  set_line_color(color_variant, self->lineColor);
}

static void on_settings_changed(GSettings *settings, gchar *key,
                                ViewerAppWindow *self) {
  if (g_strcmp0(key, "projection-type") == 0) {
    apply_projection_type_setting(self);
    gtk_widget_queue_draw(self->gl_drawing_area);
  }

  if (g_strcmp0(key, "edge-type") == 0) {
    apply_edge_type_setting(self);
    gtk_widget_queue_draw(self->gl_drawing_area);
  }

  if (g_strcmp0(key, "edge-thickness") == 0) {
    apply_edge_thickness_setting(self);
    gtk_widget_queue_draw(self->gl_drawing_area);
  }

  if (g_strcmp0(key, "line-color") == 0) {
    apply_line_color_setting(self);
    gtk_widget_queue_draw(self->gl_drawing_area);
  }
}
static void viewer_app_window_dispose(GObject *object) {
  ViewerAppWindow *win = VIEWER_APP_WINDOW(object);

  g_clear_object(&win->settings);

  win = VIEWER_APP_WINDOW(object);
  if (win->mvp_matrix) {
    free(win->mvp_matrix);
    win->mvp_matrix = NULL;
  }

  if (win->obj_file) {
    removeObjFile(win->obj_file);
    free(win->obj_file);
    win->obj_file = NULL;
  }

  G_OBJECT_CLASS(win)->dispose(object);
}

static void viewer_app_window_class_init(ViewerAppWindowClass *klass) {
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
  G_OBJECT_CLASS(klass)->dispose = viewer_app_window_dispose;

  gtk_widget_class_set_template_from_resource(
      widget_class,
      "/src/modules/ui/school21/gdy/_3dviewer/viewer-app-window.ui");

  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow,
                                       header_bar);
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow,
                                       open_button);
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow,
                                       primary_menu);
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow,
                                       gl_drawing_area);

  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow, x_scale);
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow, y_scale);
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow, z_scale);
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow,
                                       model_name_label);

  gtk_widget_class_bind_template_callback(widget_class, gl_init);
  gtk_widget_class_bind_template_callback(widget_class, gl_draw);
  gtk_widget_class_bind_template_callback(widget_class, gl_fini);
}

static void viewer_app_window_init(ViewerAppWindow *self) {
  self->obj_file = calloc(1, sizeof(ObjFile_t));
  self->mvp_matrix = calloc(1, sizeof(gl_matrix));
  initParser(self->obj_file);
  init_mvp_matrix(self->mvp_matrix);

  memset(&self->matrix_movement, 0, sizeof(mvp_matrix_movement_t));

  self->matrix_movement.scale_vector[X_AXIS] = 1.0;
  self->matrix_movement.scale_vector[Y_AXIS] = 1.0;
  self->matrix_movement.scale_vector[Z_AXIS] = 1.0;

  gtk_widget_init_template(GTK_WIDGET(self));

  gtk_gl_area_set_required_version(GTK_GL_AREA(self->gl_drawing_area), 4, 6);

  GtkSettings *settings = gtk_settings_get_default();
  g_object_set(settings, "gtk-theme-name", "Adwaita-dark", NULL);

  const char cssPath[] =
      "/src/modules/ui/school21/gdy/_3dviewer/window-style.css";
  GtkCssProvider *cssProvider = gtk_css_provider_new();
  gtk_css_provider_load_from_resource(cssProvider, cssPath);
  gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                             GTK_STYLE_PROVIDER(cssProvider),
                                             GTK_STYLE_PROVIDER_PRIORITY_USER);

  g_object_unref(cssProvider);

  GtkBuilder *builder = gtk_builder_new_from_resource(
      "/src/modules/ui/school21/gdy/_3dviewer/viewer-app-menu.ui");
  GMenuModel *menu = G_MENU_MODEL(gtk_builder_get_object(builder, "appmenu"));
  gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(self->primary_menu), menu);
  g_object_unref(builder);

  g_autoptr(GSimpleAction) open_action = g_simple_action_new("open", NULL);
  g_signal_connect(open_action, "activate",
                   G_CALLBACK(viewer_app_window__open_file_dialog), self);
  g_action_map_add_action(G_ACTION_MAP(self), G_ACTION(open_action));

  g_autoptr(GSimpleAction) settings_action =
      g_simple_action_new("settings", NULL);
  g_signal_connect(settings_action, "activate",
                   G_CALLBACK(open_settings_dialog), self);
  g_action_map_add_action(G_ACTION_MAP(self), G_ACTION(settings_action));

  self->settings = g_settings_new("school21.gdy._3dviewer");
  g_signal_connect(self->settings, "changed", G_CALLBACK(on_settings_changed),
                   self);

  apply_projection_type_setting(self);
  apply_edge_type_setting(self);
  apply_edge_thickness_setting(self);

  GtkGesture *click_gesture = gtk_gesture_click_new();
  g_signal_connect(click_gesture, "pressed", G_CALLBACK(gl_button_press_event),
                   self);
  g_signal_connect(click_gesture, "released",
                   G_CALLBACK(gl_button_release_event), self);
  gtk_widget_add_controller(GTK_WIDGET(self->gl_drawing_area),
                            GTK_EVENT_CONTROLLER(click_gesture));

  GtkEventController *motion_controller = gtk_event_controller_motion_new();
  g_signal_connect(motion_controller, "motion",
                   G_CALLBACK(gl_motion_notify_event), self);
  gtk_widget_add_controller(GTK_WIDGET(self->gl_drawing_area),
                            motion_controller);

  GtkEventController *scroll_controller =
      gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES);
  g_signal_connect(scroll_controller, "scroll", G_CALLBACK(gl_scroll_event),
                   self);
  gtk_widget_add_controller(GTK_WIDGET(self->gl_drawing_area),
                            scroll_controller);

  g_signal_connect(self->x_scale, "value-changed",
                   G_CALLBACK(on_scale_value_changed), self);
  g_signal_connect(self->y_scale, "value-changed",
                   G_CALLBACK(on_scale_value_changed), self);
  g_signal_connect(self->z_scale, "value-changed",
                   G_CALLBACK(on_scale_value_changed), self);
  
  gtk_window_set_icon_name(GTK_WINDOW(self), "viewer");
}

GtkWidget *viewer_app_window_new(ViewerApp *app) {
  return g_object_new(viewer_app_window_get_type(), "application", app, NULL);
}
