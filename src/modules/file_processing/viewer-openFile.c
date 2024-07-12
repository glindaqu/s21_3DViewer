#include "../../include/viewer-openFile.h"

#include "../../include/viewer-setLabel.h"

void open_file(ViewerAppWindow *self, GFile *file) {
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

  glDelBuffers(&self->gl_buffers);

  init_buffers(self->obj_file, &self->gl_buffers);

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

void on_open_response(GObject *source, GAsyncResult *result,
                      gpointer user_data) {
  GtkFileDialog *dialog = GTK_FILE_DIALOG(source);
  ViewerAppWindow *self = user_data;

  g_autoptr(GFile) file = gtk_file_dialog_open_finish(dialog, result, NULL);

  if (file != NULL) open_file(self, file);
}

void viewer_app_window__open_file_dialog(GAction *action, GVariant *parameter,
                                         ViewerAppWindow *self) {
  g_autoptr(GtkFileDialog) dialog = gtk_file_dialog_new();

  gtk_file_dialog_open(dialog, GTK_WINDOW(self), NULL, on_open_response, self);
}
