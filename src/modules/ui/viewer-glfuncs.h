#ifndef _VIEWER_GLFUNCS_H
#define _VIEWER_GLFUNCS_H

#include <epoxy/gl.h>
#include <gtk/gtk.h>

#include "../../include/parser.h"
#include "viewer-error.h"

typedef struct GLbuffers_t{
  GLuint vao;
  GLuint vbo;
  GLuint ebo;
} GLbuffers_t;

typedef struct GLshader_vars_t{
  GLuint program;
  GLuint mvp_location;
  GLuint loc_res;
  GLuint loc_factor;
  GLuint loc_pattern;
  GLuint loc_lineColor;
  GLuint rojection_location;
  GLuint color_location;
} GLshader_vars_t;

void init_buffers(ObjFile_t *objFile, guint position_index, guint color_index, GLbuffers_t *buffers_out);

void glDelProgram(GLuint* program);

void glDelBuffers(GLbuffers_t *buffer);

guint create_shader(int shader_type, const char *source, GError **error,
                    guint *shader_out);

gboolean init_shaders(GLshader_vars_t *vars_out,
                      GError **error);

#endif  // _VIEWER_GLFUNCS_H
