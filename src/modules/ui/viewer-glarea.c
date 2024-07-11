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
  glEnable(GL_BLEND);
  glDepthFunc(GL_LESS);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_DEPTH_TEST);


  glEnable(GL_PROGRAM_POINT_SIZE);
  
  glEnable(GL_LINE_SMOOTH);

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

  glUniform3fv(self->shader_vars.loc_lineColor, 1, self->edge_color);
  glUniform1ui(self->shader_vars.loc_pattern, self->pattern);
  glUniform1f(self->shader_vars.loc_factor, self->factor);
  glUniform2f(self->shader_vars.loc_res, 800.f, 800.f);
  
  glUniformMatrix4fv(self->shader_vars.mvp_location, 1, GL_FALSE,
                     &self->mvp_matrix->mvp[0]);
                     
  glBindVertexArray(self->gl_buffers.vao);

  glLineWidth(self->edge_thickness); 
  glDrawElements(GL_LINES, self->obj_file->surfacesCount * 6, GL_UNSIGNED_INT,
                 0);

  if (self->point_type == 1) {
  glUseProgram(self->shader_vars.point_program);
  
  glUniformMatrix4fv(self->shader_vars.mvp_location, 1, GL_FALSE,
                     &self->mvp_matrix->mvp[0]);
  }
  if (self->point_type == 2 || self->point_type == 1) {
    glPointSize(self->point_size);
    glDrawElements(GL_POINTS, self->obj_file->surfacesCount * 6, GL_UNSIGNED_INT,
                  0);
  }
  glBindVertexArray(0);
  glUseProgram(0);
}

gboolean gl_draw(ViewerAppWindow *self) {
  glClearColor(self->background_color.red, self->background_color.green, self->background_color.blue, self->background_color.alpha);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gl_model_draw(self);

  glFlush();

  return TRUE;
}
