#include "viewer-glarea.h"

#include "viewer-glfuncs.h"

void gl_init(ViewerAppWindow *self) {
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

  init_buffers(self->obj_file, 0, 0, &self->gl_buffers);

  error = NULL;
  if (!init_shaders(&self->shader_vars, &error)) {
    gtk_gl_area_set_error(GTK_GL_AREA(self->gl_drawing_area), error);
    g_error_free(error);
    return;
  }
}

void gl_fini(ViewerAppWindow *self) {
  gtk_gl_area_make_current(GTK_GL_AREA(self->gl_drawing_area));
  
  if (gtk_gl_area_get_error(GTK_GL_AREA(self->gl_drawing_area)) != NULL) return;

  glDelBuffers(&self->gl_buffers);
  glDelProgram(&self->shader_vars.program);
}

void gl_model_draw(ViewerAppWindow *self) {
  glUseProgram(self->shader_vars.program);

  glUniform3fv(self->shader_vars.loc_lineColor, 1, self->lineColor);
  glUniform1ui(self->shader_vars.loc_pattern, self->pattern);
  glUniform1f(self->shader_vars.loc_factor, self->factor);
  glUniform2f(self->shader_vars.loc_res, 800.f, 800.f);
  
  glUniformMatrix4fv(self->shader_vars.mvp_location, 1, GL_FALSE,
                     &self->mvp_matrix->mvp[0]);
                     
  glBindVertexArray(self->gl_buffers.vao);

  glDrawElements(GL_LINES, self->obj_file->surfacesCount * 6, GL_UNSIGNED_INT,
                 0);

  glBindVertexArray(0);
  glUseProgram(0);
}

gboolean gl_draw(ViewerAppWindow *self) {
  glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gl_model_draw(self);

  glFlush();

  return TRUE;
}
