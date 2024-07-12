#include "viewer-glarea.h"

#include "viewer-glfuncs.h"
#include "viewer-gif.h"
#include <glib/gthread.h>

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

void gl_draw_lines(ViewerAppWindow *self) {
  glUseProgram(self->shader_vars.program);

  self->shader_vars.loc_lineColor = glGetUniformLocation(self->shader_vars.program, "lineColor");

  glUniform3fv(self->shader_vars.loc_lineColor, 1, self->edge_color);
  glUniform1ui(self->shader_vars.loc_pattern, self->pattern);
  glUniform1f(self->shader_vars.loc_factor, self->factor);
  glUniform2f(self->shader_vars.loc_res, 800.f, 800.f);
  
  glUniformMatrix4fv(self->shader_vars.projection_location, 1, GL_FALSE, &self->projection_matrix[0][0]);
  glUniformMatrix4fv(self->shader_vars.mvp_location, 1, GL_FALSE,
                     &self->mvp_matrix->mvp[0]);
  
  glBindVertexArray(self->gl_buffers.vao);

  glLineWidth(self->edge_thickness); 
  glDrawElements(GL_LINES, self->obj_file->surfacesCount * 6, GL_UNSIGNED_INT,
                 0);
}

void gl_draw_points(ViewerAppWindow *self) {
  if (self->point_type == 1) {
  glUseProgram(self->shader_vars.point_program);

  self->shader_vars.loc_lineColor = glGetUniformLocation(self->shader_vars.point_program, "lineColor");
  glUniformMatrix4fv(self->shader_vars.mvp_location, 1, GL_FALSE,
                     &self->mvp_matrix->mvp[0]);
  glUniformMatrix4fv(self->shader_vars.projection_location, 1, GL_FALSE, &self->projection_matrix[0][0]);

  }
  if (self->point_type == 2 || self->point_type == 1) {
    glUniform3fv(self->shader_vars.loc_lineColor, 1, self->point_color);
    glPointSize(self->point_size);
    glDrawElements(GL_POINTS, self->obj_file->surfacesCount * 6, GL_UNSIGNED_INT,
                  0);
  }
}

typedef struct {
    ViewerAppWindow *self;
    uint8_t *frame_data;
    uint16_t width;
    uint16_t height;
} FrameData;

void gl_model_draw(ViewerAppWindow *self) {

  gl_draw_lines(self);

  gl_draw_points(self);

  glBindVertexArray(0);
  glUseProgram(0);
}

static void *process_frame_async(void *data) {
    FrameData *frame_data = (FrameData *)data;
    add_frame_to_gif(frame_data->self, frame_data->frame_data, frame_data->width, frame_data->height);
    free(frame_data->frame_data);
    g_free(frame_data);
    return NULL;
}
gboolean gl_draw(ViewerAppWindow *self) {
  glClearColor(self->background_color.red, self->background_color.green, self->background_color.blue, self->background_color.alpha);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gl_model_draw(self);

  glFlush();

  if (self->recording) {
    static guint64 last_frame_time = 0;
    guint64 current_time = g_get_real_time();
    if (current_time - last_frame_time >= G_USEC_PER_SEC / 30) {
      last_frame_time = current_time;

      GtkWidget *gl_area = GTK_WIDGET(self->gl_drawing_area);
      int width = gtk_widget_get_width(gl_area);
      int height = gtk_widget_get_height(gl_area);

      uint8_t* frame_data = capture_frame_from_opengl(width, height);
      FrameData *data = g_new(FrameData, 1);
      data->self = self;
      data->frame_data = frame_data;
      data->width = width;
      data->height = height;

      g_thread_new("frame_processor", process_frame_async, data);
    }
  }

  return TRUE;
}
