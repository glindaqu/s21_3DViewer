#include "viewer-app-window.h"

#include <epoxy/gl.h>

#include "../../include/matrix_calc.h"
#include "../../include/parser.h"
#include "viewer-glfuncs.h"

struct _ViewerAppWindow {
  GtkApplicationWindow parent_instance;

  GtkHeaderBar *header_bar;

  // double rotation_angles[N_AXES];
  mvp_matrix_movement_t matrix_movement;

  GtkWidget *primary_menu;
  GtkButton *open_button;

  // ObjFile_t obj_file;
  ObjFile_t *obj_file;

  gl_matrix *mvp_matrix;

  GtkWidget *gl_drawing_area;

  /* GL objects */
  guint vao;
  guint vbo;
  guint ebo;
  guint program;
  guint mvp_location;
  guint position_index;
  guint color_index;

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
  gtk_gl_area_make_current(GTK_GL_AREA(self->gl_drawing_area));

  if (gtk_gl_area_get_error(GTK_GL_AREA(self->gl_drawing_area)) != NULL) return;

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  GError *error = NULL;
  if (!init_shaders(&self->program, &self->mvp_location, &self->position_index,
                    &self->color_index, &error)) {
    gtk_gl_area_set_error(GTK_GL_AREA(self->gl_drawing_area), error);
    g_error_free(error);
    return;
  }
  init_buffers(self->obj_file, self->position_index, self->color_index,
               &self->vao);
}

static void gl_fini(ViewerAppWindow *self) {
  /* we need to ensure that the GdkGLContext is set before calling GL API */
  gtk_gl_area_make_current(GTK_GL_AREA(self->gl_drawing_area));

  /* skip everything if we're in error state */
  if (gtk_gl_area_get_error(GTK_GL_AREA(self->gl_drawing_area)) != NULL) return;

  /* destroy all the resources we created */
  if (self->vao != 0) glDeleteVertexArrays(1, &self->vao);
  if (self->vbo != 0) glDeleteBuffers(1, &self->vbo);
  if (self->ebo != 0) glDeleteBuffers(1, &self->ebo);
  if (self->program != 0) glDeleteProgram(self->program);
}

static void gl_model_draw(ViewerAppWindow *self) {
  glUseProgram(self->program);
  glUniformMatrix4fv(self->mvp_location, 1, GL_FALSE, self->mvp_matrix->mvp);
  glBindVertexArray(self->vao);
  glDrawElements(GL_LINE_LOOP, self->obj_file->surfacesCount * 6,
                 GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
  glUseProgram(0);
}

static gboolean gl_draw(ViewerAppWindow *self) {
  /* clear the viewport; the viewport is automatically resized when
   * the GtkGLArea gets a new size allocation
   */
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gl_model_draw(self);

  glFlush();

  return FALSE;
}

static void open_file(ViewerAppWindow *self, GFile *file) {
  if (self->obj_file->fileName != NULL) {
    removeObjFile(self->obj_file);
    initParser(self->obj_file);
  }

  if (file == NULL) {
    g_printerr("Unable to open file\n");
    return;
  }

  self->obj_file->fileName = g_file_peek_path(file);

  if (parse(self->obj_file) != OK) {
    g_printerr("Unable to parse “%s”\n", g_file_peek_path(file));
    return;
  }

  if (self->vao != 0) {
    glDeleteVertexArrays(1, &self->vao);
    self->vao = 0;
  }

  gl_init(self);

  init_mvp_matrix(self->mvp_matrix);

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
      GDK_BUTTON_PRIMARY) {  // Left mouse button
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

static void key_pressed_event(GtkEventControllerKey *controller, guint keyval,
                              guint keycode, GdkModifierType state,
                              ViewerAppWindow *self) {
  if (keyval == GDK_KEY_Alt_L || keyval == GDK_KEY_Alt_R) {
    self->alt_key = TRUE;
  }
}

static void key_released_event(GtkEventControllerKey *controller, guint keyval,
                               guint keycode, GdkModifierType state,
                               ViewerAppWindow *self) {
  if (keyval == GDK_KEY_Alt_L || keyval == GDK_KEY_Alt_R) {
    self->alt_key = FALSE;
  }
}

static void gl_scroll_event(GtkEventControllerScroll *controller, double dx,
                            double dy, ViewerAppWindow *self) {
  GdkModifierType state = gtk_event_controller_get_current_event_state(
      GTK_EVENT_CONTROLLER(controller));

  if (state & GDK_ALT_MASK) {
    self->matrix_movement.rotation_angles[2] += dy * 5.0;

    move_mvp_matrix(self->mvp_matrix, &self->matrix_movement);

    gtk_widget_queue_draw(self->gl_drawing_area);
  }
}

static void key_pressed_move_event(GtkEventControllerKey *controller,
                                   guint keyval, guint keycode,
                                   GdkModifierType state,
                                   ViewerAppWindow *self) {
  switch (keyval) {
    case GDK_KEY_Up:
      self->matrix_movement.translation_vector[1] += 0.1;
      break;
    case GDK_KEY_Down:
      self->matrix_movement.translation_vector[1] -= 0.1;
      break;
    case GDK_KEY_Left:
      self->matrix_movement.translation_vector[0] -= 0.1;
      break;
    case GDK_KEY_Right:
      self->matrix_movement.translation_vector[0] += 0.1;
      break;
  }

  move_mvp_matrix(self->mvp_matrix, &self->matrix_movement);

  gtk_widget_queue_draw(self->gl_drawing_area);
}

static void viewer_app_window_class_init(ViewerAppWindowClass *klass) {
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

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

  gtk_widget_class_bind_template_callback(widget_class, gl_init);
  gtk_widget_class_bind_template_callback(widget_class, gl_draw);
  gtk_widget_class_bind_template_callback(widget_class, gl_fini);
}

static void viewer_app_window_init(ViewerAppWindow *self) {
  self->obj_file = calloc(1, sizeof(ObjFile_t));
  self->mvp_matrix = calloc(1, sizeof(gl_matrix));
  initParser(self->obj_file);
  init_mvp_matrix(self->mvp_matrix);

  gtk_widget_init_template(GTK_WIDGET(self));

  GtkBuilder *builder = gtk_builder_new_from_resource(
      "/src/modules/ui/school21/gdy/_3dviewer/viewer-app-menu.ui");
  GMenuModel *menu = G_MENU_MODEL(gtk_builder_get_object(builder, "appmenu"));
  gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(self->primary_menu), menu);
  g_object_unref(builder);

  g_autoptr(GSimpleAction) open_action = g_simple_action_new("open", NULL);
  g_signal_connect(open_action, "activate",
                   G_CALLBACK(viewer_app_window__open_file_dialog), self);
  g_action_map_add_action(G_ACTION_MAP(self), G_ACTION(open_action));

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
      gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_VERTICAL);
  g_signal_connect(scroll_controller, "scroll", G_CALLBACK(gl_scroll_event),
                   self);
  gtk_widget_add_controller(GTK_WIDGET(self->gl_drawing_area),
                            scroll_controller);

  GtkEventController *key_controller = gtk_event_controller_key_new();
  g_signal_connect(key_controller, "key-pressed",
                   G_CALLBACK(key_pressed_move_event), self);
  gtk_widget_add_controller(GTK_WIDGET(self->gl_drawing_area), key_controller);

  gtk_window_set_icon_name(GTK_WINDOW(self), "viewer");
}

static void viewer_app_window_dispose(GObject *object) {
  ViewerAppWindow *win;

  win = VIEWER_APP_WINDOW(object);
  if (win->mvp_matrix) {
    free(win->mvp_matrix);
  }

  if (win->obj_file) {
    removeObjFile(win->obj_file);
    free(win->obj_file);
  }

  G_OBJECT_CLASS(win)->dispose(object);
}

GtkWidget *viewer_app_window_new(ViewerApp *app) {
  return g_object_new(viewer_app_window_get_type(), "application", app, NULL);
}
