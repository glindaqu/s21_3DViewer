#ifndef _VIEWER_GLFUNCS_H
#define _VIEWER_GLFUNCS_H

#include <epoxy/gl.h>
#include <gtk/gtk.h>

#include "../../include/parser.h"
#include "viewer-app-window.h"
#include "viewer-error.h"

typedef struct {
  float position[3];
  float color[3];
} VertexInfo;

void init_buffers(ObjFile_t *objFile, guint position_index, guint color_index,
                  guint *vao_out, guint* vbo_out, guint* ebo_out);

guint create_shader(int shader_type, const char *source, GError **error,
                    guint *shader_out);

gboolean init_shaders(guint *program_out, guint *mvp_location_out,
                      guint *projection_location_out, guint *color_location_out, GLint* loc_res_out, GLint* loc_pattern_out, GLint* loc_factor_out, GLint* loc_thickness_out, 
                      GError **error);

#endif  // _VIEWER_GLFUNCS_H
