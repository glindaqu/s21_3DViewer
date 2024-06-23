#include "viewer-app-window.h"

#include <epoxy/gl.h>
#include <math.h>

#include "../../include/parser.h"
#include "viewer-error.h"

enum { X_AXIS, Y_AXIS, Z_AXIS, N_AXES };

struct _ViewerAppWindow {
  GtkApplicationWindow parent_instance;

  GtkHeaderBar *header_bar;

  GtkAdjustment *x_adjustment;
  GtkAdjustment *y_adjustment;
  GtkAdjustment *z_adjustment;

  GtkWidget *primary_menu;
  GtkButton *open_button;

  // ObjFile_t obj_file;
  ObjFile_t obj_file;
  // Rotations angles
  double rotation_angles[N_AXES];

  /* model-view-projection matrix */
  float mvp[16];

  GtkWidget *gl_drawing_area;

  /* GL objects */
  guint vao;
  guint program;
  guint mvp_location;
  guint position_index;
  guint color_index;
};

struct _ViewerAppWindowClass {
  GtkApplicationWindowClass parent_class;
};

typedef struct {
  float position[3];
  float color[3];
} VertexInfo;

G_DEFINE_TYPE(ViewerAppWindow, viewer_app_window, GTK_TYPE_APPLICATION_WINDOW)

void init_buffers(ObjFile_t *objFile, guint position_index, guint color_index,
                  guint *vao_out) {
  guint vao, buffer;
  int i;

  VertexInfo *vertex_data =
      (VertexInfo *)malloc(objFile->verticesCount * sizeof(VertexInfo));
  for (i = 0; i < objFile->verticesCount; i++) {
    vertex_data[i].position[0] = objFile->vertices[i]->x;
    vertex_data[i].position[1] = objFile->vertices[i]->y;
    vertex_data[i].position[2] = objFile->vertices[i]->z;
    vertex_data[i].color[0] = (float)(i % 3 == 0);
    vertex_data[i].color[1] = (float)(i % 3 == 1);
    vertex_data[i].color[2] = (float)(i % 3 == 2);
  }
  for (int i = 0; i < objFile->verticesCount; i++) {
    g_print("vertex: %f %f %f\n", objFile->vertices[i]->x,
            objFile->vertices[i]->y, objFile->vertices[i]->z);
  }

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, objFile->verticesCount * sizeof(VertexInfo),
               vertex_data, GL_STATIC_DRAW);

  glEnableVertexAttribArray(position_index);
  glVertexAttribPointer(position_index, 3, GL_FLOAT, GL_FALSE,
                        sizeof(VertexInfo), (GLvoid *)0);

  glEnableVertexAttribArray(color_index);
  glVertexAttribPointer(color_index, 3, GL_FLOAT, GL_FALSE, sizeof(VertexInfo),
                        (GLvoid *)(3 * sizeof(float)));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glDeleteBuffers(1, &buffer);

  if (vao_out != NULL) *vao_out = vao;

  free(vertex_data);
}

static guint create_shader(int shader_type, const char *source, GError **error,
                           guint *shader_out) {
  guint shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  int status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    int log_len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);

    char *buffer = g_malloc(log_len + 1);
    glGetShaderInfoLog(shader, log_len, NULL, buffer);

    g_set_error(error, VIEWER_ERROR, VIEWER_ERROR_SHADER_COMPILATION,
                "Compilation failure in %s shader: %s",
                shader_type == GL_VERTEX_SHADER ? "vertex" : "fragment",
                buffer);

    g_free(buffer);

    glDeleteShader(shader);
    shader = 0;
  }

  if (shader_out != NULL) *shader_out = shader;

  return shader != 0;
}

static gboolean init_shaders(guint *program_out, guint *mvp_location_out,
                             guint *position_location_out,
                             guint *color_location_out, GError **error) {
  GBytes *source;
  guint program = 0;
  guint mvp_location = 0;
  guint vertex = 0, fragment = 0;
  guint position_location = 0;
  guint color_location = 0;

  source = g_resources_lookup_data(
      "/src/modules/ui/school21/gdy/_3dviewer/glarea-vertex.glsl", 0, NULL);
  create_shader(GL_VERTEX_SHADER, g_bytes_get_data(source, NULL), error,
                &vertex);
  g_bytes_unref(source);
  if (vertex == 0) goto out;

  source = g_resources_lookup_data(
      "/src/modules/ui/school21/gdy/_3dviewer/glarea-fragment.glsl", 0, NULL);
  create_shader(GL_FRAGMENT_SHADER, g_bytes_get_data(source, NULL), error,
                &fragment);
  g_bytes_unref(source);
  if (fragment == 0) goto out;

  /* link the vertex and fragment shaders together */
  program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);

  int status = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    int log_len = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);

    char *buffer = g_malloc(log_len + 1);
    glGetProgramInfoLog(program, log_len, NULL, buffer);

    g_set_error(error, VIEWER_ERROR, VIEWER_ERROR_SHADER_LINK,
                "Linking failure in program: %s", buffer);

    g_free(buffer);

    glDeleteProgram(program);
    program = 0;

    goto out;
  }

  /* get the location of the "mvp" uniform */
  mvp_location = glGetUniformLocation(program, "mvp");

  /* get the location of the "position" and "color" attributes */
  position_location = glGetAttribLocation(program, "position");
  color_location = glGetAttribLocation(program, "color");

  /* the individual shaders can be detached and destroyed */
  glDetachShader(program, vertex);
  glDetachShader(program, fragment);

out:
  if (vertex != 0) glDeleteShader(vertex);
  if (fragment != 0) glDeleteShader(fragment);

  if (program_out != NULL) *program_out = program;
  if (mvp_location_out != NULL) *mvp_location_out = mvp_location;
  if (position_location_out != NULL) *position_location_out = position_location;
  if (color_location_out != NULL) *color_location_out = color_location;

  return program != 0;
}

void init_buffers_from_data(const VertexInfo *vertex_data, int vertex_count,
                            guint position_index, guint color_index,
                            guint *vao_out) {
  guint vao, buffer;

  // Создаем VAO
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Создаем VBO
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(VertexInfo), vertex_data,
               GL_STATIC_DRAW);

  // Включаем и настраиваем атрибут позиции
  glEnableVertexAttribArray(position_index);
  glVertexAttribPointer(position_index, 3, GL_FLOAT, GL_FALSE,
                        sizeof(VertexInfo), (GLvoid *)0);

  // Включаем и настраиваем атрибут цвета
  glEnableVertexAttribArray(color_index);
  glVertexAttribPointer(color_index, 3, GL_FLOAT, GL_FALSE, sizeof(VertexInfo),
                        (GLvoid *)(3 * sizeof(float)));

  // Сбрасываем состояние
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Удаляем VBO, так как он уже связан с VAO
  glDeleteBuffers(1, &buffer);

  if (vao_out != NULL) *vao_out = vao;
}

static void gl_init(ViewerAppWindow *self) {
  gtk_gl_area_make_current(GTK_GL_AREA(self->gl_drawing_area));
  if (gtk_gl_area_get_error(GTK_GL_AREA(self->gl_drawing_area)) != NULL) return;

  GError *error = NULL;
  if (!init_shaders(&self->program, &self->mvp_location, &self->position_index,
                    &self->color_index, &error)) {
    gtk_gl_area_set_error(GTK_GL_AREA(self->gl_drawing_area), error);
    g_error_free(error);
    return;
  }

  // Initialize with a default triangle if no file is loaded
  if (self->obj_file.verticesCount == 0) {
    VertexInfo triangle_data[] = {
        {{0.0f, 0.500f, 0.0f}, {1.f, 0.f, 0.f}},
        {{0.5f, -0.366f, 0.0f}, {0.f, 1.f, 0.f}},
        {{-0.5f, -0.366f, 0.0f}, {0.f, 0.f, 1.f}},
    };
    init_buffers_from_data(triangle_data, 3, self->position_index,
                           self->color_index, &self->vao);
  } else {
    init_buffers(&self->obj_file, self->position_index, self->color_index,
                 &self->vao);
  }
}

static void gl_fini(ViewerAppWindow *self) {
  /* we need to ensure that the GdkGLContext is set before calling GL API */
  gtk_gl_area_make_current(GTK_GL_AREA(self->gl_drawing_area));

  /* skip everything if we're in error state */
  if (gtk_gl_area_get_error(GTK_GL_AREA(self->gl_drawing_area)) != NULL) return;

  /* destroy all the resources we created */
  if (self->vao != 0) glDeleteVertexArrays(1, &self->vao);
  if (self->program != 0) glDeleteProgram(self->program);
}

static void draw_triangle(ViewerAppWindow *self) {
  if (self->program == 0 || self->vao == 0) return;

  glUseProgram(self->program);
  glUniformMatrix4fv(self->mvp_location, 1, GL_FALSE, &(self->mvp[0]));
  glBindVertexArray(self->vao);

  if (self->obj_file.verticesCount > 0) {
    glDrawArrays(GL_TRIANGLES, 0, self->obj_file.verticesCount);
  } else {
    glDrawArrays(GL_TRIANGLES, 0, 3);  // Draw the default triangle
  }

  glBindVertexArray(0);
  glUseProgram(0);
}

static gboolean gl_draw(ViewerAppWindow *self) {
  /* clear the viewport; the viewport is automatically resized when
   * the GtkGLArea gets a new size allocation
   */
  glClearColor(0.5, 0.5, 0.5, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  /* draw our object */
  draw_triangle(self);

  /* flush the contents of the pipeline */
  glFlush();

  return FALSE;
}

static void init_mvp(float *res) {
  /* initialize a matrix as an identity matrix */
  res[0] = 1.f;
  res[4] = 0.f;
  res[8] = 0.f;
  res[12] = 0.f;
  res[1] = 0.f;
  res[5] = 1.f;
  res[9] = 0.f;
  res[13] = 0.f;
  res[2] = 0.f;
  res[6] = 0.f;
  res[10] = 1.f;
  res[14] = 0.f;
  res[3] = 0.f;
  res[7] = 0.f;
  res[11] = 0.f;
  res[15] = 1.f;
}

static void compute_mvp(float *res, float phi, float theta, float psi) {
  float x = phi * (G_PI / 180.f);
  float y = theta * (G_PI / 180.f);
  float z = psi * (G_PI / 180.f);
  float c1 = cosf(x), s1 = sinf(x);
  float c2 = cosf(y), s2 = sinf(y);
  float c3 = cosf(z), s3 = sinf(z);
  float c3c2 = c3 * c2;
  float s3c1 = s3 * c1;
  float c3s2s1 = c3 * s2 * s1;
  float s3s1 = s3 * s1;
  float c3s2c1 = c3 * s2 * c1;
  float s3c2 = s3 * c2;
  float c3c1 = c3 * c1;
  float s3s2s1 = s3 * s2 * s1;
  float c3s1 = c3 * s1;
  float s3s2c1 = s3 * s2 * c1;
  float c2s1 = c2 * s1;
  float c2c1 = c2 * c1;

  /* apply all three Euler angles rotations using the three matrices:
   *
   * ⎡  c3 s3 0 ⎤ ⎡ c2  0 -s2 ⎤ ⎡ 1   0  0 ⎤
   * ⎢ -s3 c3 0 ⎥ ⎢  0  1   0 ⎥ ⎢ 0  c1 s1 ⎥
   * ⎣   0  0 1 ⎦ ⎣ s2  0  c2 ⎦ ⎣ 0 -s1 c1 ⎦
   */
  res[0] = c3c2;
  res[4] = s3c1 + c3s2s1;
  res[8] = s3s1 - c3s2c1;
  res[12] = 0.f;
  res[1] = -s3c2;
  res[5] = c3c1 - s3s2s1;
  res[9] = c3s1 + s3s2c1;
  res[13] = 0.f;
  res[2] = s2;
  res[6] = -c2s1;
  res[10] = c2c1;
  res[14] = 0.f;
  res[3] = 0.f;
  res[7] = 0.f;
  res[11] = 0.f;
  res[15] = 1.f;
}

static void adjustment_changed(ViewerAppWindow *self, GtkAdjustment *adj) {
  double value = gtk_adjustment_get_value(adj);

  /* update the rotation angles */
  if (adj == self->x_adjustment) self->rotation_angles[X_AXIS] = value;

  if (adj == self->y_adjustment) self->rotation_angles[Y_AXIS] = value;

  if (adj == self->z_adjustment) self->rotation_angles[Z_AXIS] = value;

  /* recompute the mvp matrix */
  compute_mvp(self->mvp, self->rotation_angles[X_AXIS],
              self->rotation_angles[Y_AXIS], self->rotation_angles[Z_AXIS]);

  /* queue a redraw on the GtkGLArea to ensure its contents
   * get redrawn in the next frame
   */
  gtk_widget_queue_draw(self->gl_drawing_area);
}

static void open_file_complete(GObject *source_object, GAsyncResult *result,
                               ViewerAppWindow *self) {
  GFile *file = G_FILE(source_object);

  g_autofree char *contents = NULL;
  gsize length = 0;

  g_autoptr(GError) error = NULL;

  g_file_load_contents_finish(file, result, &contents, &length, NULL, &error);

  if (error != NULL) {
    g_printerr("Unable to open “%s”: %s\n", g_file_peek_path(file),
               error->message);
    return;
  }

  if (contents == NULL) {
    g_printerr("File “%s” was empty\n", g_file_peek_path(file));
    return;
  }
  self->obj_file.fileName = g_file_peek_path(file);
  if (parse(&self->obj_file) != OK) {
    g_printerr("Unable to parse “%s”\n", g_file_peek_path(file));
    return;
  }
  init_buffers(&self->obj_file, self->position_index, self->color_index,
               &self->vao);

  gtk_widget_queue_draw(self->gl_drawing_area);
}

static void open_file(ViewerAppWindow *self, GFile *file) {
  g_file_load_contents_async(file, NULL,
                             (GAsyncReadyCallback)open_file_complete, self);
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
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow,
                                       x_adjustment);
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow,
                                       y_adjustment);
  gtk_widget_class_bind_template_child(widget_class, ViewerAppWindow,
                                       z_adjustment);

  gtk_widget_class_bind_template_callback(widget_class, adjustment_changed);
  gtk_widget_class_bind_template_callback(widget_class, gl_init);
  gtk_widget_class_bind_template_callback(widget_class, gl_fini);
  gtk_widget_class_bind_template_callback(widget_class, gl_draw);
}

static void viewer_app_window_init(ViewerAppWindow *self) {
  GtkBuilder *builder;
  GMenuModel *menu;
  // GAction *action;

  gtk_widget_init_template(GTK_WIDGET(self));

  builder = gtk_builder_new_from_resource(
      "/src/modules/ui/school21/gdy/_3dviewer/viewer-app-menu.ui");
  menu = G_MENU_MODEL(gtk_builder_get_object(builder, "appmenu"));
  gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(self->primary_menu), menu);
  g_object_unref(builder);

  g_autoptr(GSimpleAction) open_action = g_simple_action_new("open", NULL);
  g_signal_connect(open_action, "activate",
                   G_CALLBACK(viewer_app_window__open_file_dialog), self);
  g_action_map_add_action(G_ACTION_MAP(self), G_ACTION(open_action));

  /* reset the mvp matrix */
  init_mvp(self->mvp);
  // init_obj(self->obj_file);

  gtk_window_set_icon_name(GTK_WINDOW(self), "viewer");
}

// static void
// viewer_app_window_dispose (GObject *object)
// {
// ViewerAppWindow *win;

// win = VIEWER_APP_WINDOW (object);
//
// g_clear_object (&win->settings);

// G_OBJECT_CLASS (vie)->dispose (object);
// }

GtkWidget *viewer_app_window_new(ViewerApp *app) {
  return g_object_new(viewer_app_window_get_type(), "application", app, NULL);
}
