#ifndef _VIEWER_GLFUNCS_H
#define _VIEWER_GLFUNCS_H

#include <epoxy/gl.h>
#include <gtk/gtk.h>

#include "parser.h"

/** 
 * @brief OpenGL буферы
 *
 * @param vao - vertex array object
 * @param vbo - vertex buffer object
 * @param ebo - element buffer object
 * @return void
 *
 * @author yukikoqo
*/
typedef struct GLbuffers_t{
  GLuint vao;
  GLuint vbo;
  GLuint ebo;
} GLbuffers_t;

/** 
 * @brief OpenGL шейдеры
 *
 * @param program - vertex shader
 * @param point_program - point shader
 * @param mvp_location - uniform location
 * @param loc_res - uniform location
 * @param loc_factor - uniform location
 * @param loc_pattern - uniform location
 * @param loc_lineColor - uniform location
 * @param projection_location - uniform location
 * @param color_location - uniform location
 * @return void
 *
 * @author yukikoqo
*/
typedef struct GLshader_vars_t{
  GLuint program;
  GLuint point_program;
  GLuint mvp_location;
  GLuint loc_res;
  GLuint loc_factor;
  GLuint loc_pattern;
  GLuint loc_lineColor;
  GLuint projection_location;
  GLuint color_location;
} GLshader_vars_t;


/** 
 * @brief OpenGL функция инициализации буферов vao, vbo, ebo
 *
 * @param objFile - данные модели
 * @param buffers_out - буферы OpenGL
 * @return void
 *
 * @author yukikoqo
*/
void init_buffers(ObjFile_t *objFile, GLbuffers_t *buffers_out);
/** 
 * @brief OpenGL функция удаления шейдеров
 *
 * @param program - vertex shader
 * @return void
 *
 * @author yukikoqo
*/
void glDelProgram(GLuint* program);
/** 
 * @brief OpenGL функция удаления буферов vao, vbo, ebo
 *
 * @param buffer - буфер OpenGL
 * @return void
 *
 * @author yukikoqo
*/
void glDelBuffers(GLbuffers_t *buffer);

/** 
 * @brief OpenGL функция создания шейдеров
 *
 * @param shader_type - тип шейдера
 * @param source - исходник шейдера
 * @param error - ошибки
 * @param shader_out - выходной шейдер
 * @return void
 *
 * @author yukikoqo
*/
guint create_shader(int shader_type, const char *source, GError **error,
                    guint *shader_out);

/** 
 * @brief OpenGL функция инициализации шейдеров
 *
 * @param vars_out - шейдеры OpenGL
 * @param error - ошибки
 * @return void
 *
 * @author yukikoqo
*/
gboolean init_shaders(GLshader_vars_t *vars_out,
                      GError **error);
#endif  // _VIEWER_GLFUNCS_H
