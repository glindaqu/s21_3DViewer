#include "viewer-glfuncs.h"

void init_buffers(ObjFile_t *objFile, guint position_index, guint color_index,
                  guint *vao_out, guint* vbo_out, guint* ebo_out) {
  guint vao, vbo, ebo;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);

  Vertex_t *vertices = calloc(objFile->verticesCount, sizeof(Vertex_t));
  for (int i = 0; i < objFile->verticesCount; i++) {
    vertices[i] = *(objFile->vertices[i]);
  }
  GLuint *indices = calloc(objFile->surfacesCount * 3, sizeof(GLuint));
  for (int i = 0; i < objFile->surfacesCount; i++) {
    Surface_t *surface = objFile->surfaces[i];
    indices[i * 3] = surface->verticesIndices[0];
    indices[i * 3 + 1] = surface->verticesIndices[1];
    indices[i * 3 + 2] = surface->verticesIndices[2];
  }

  // Prepare vertex data
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, objFile->verticesCount * sizeof(Vertex_t),
               vertices, GL_STATIC_DRAW);

  // Prepare index data
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               objFile->surfacesCount * 3 * sizeof(GLuint), indices,
               GL_STATIC_DRAW);

  // Set up vertex attribute pointers
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_t),
                        (GLvoid *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  free(vertices);
  free(indices);

  if (vao_out != NULL) *vao_out = vao;
  if (vbo_out != NULL) *vbo_out = vbo;
  if (ebo_out != NULL) *ebo_out = ebo;
}

guint create_shader(int shader_type, const char *source, GError **error,
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

gboolean init_shaders(guint *program_out, guint *mvp_location_out,
                      guint *projection_location_out, guint *color_location_out, GLint* loc_res_out, GLint* loc_pattern_out, GLint* loc_factor_out, GLint* loc_thickness_out,
                      GError **error) {
  GBytes *source;
  guint program = 0;
  guint mvp_location = 0;
  guint vertex = 0, fragment = 0;
  guint projection_location = 0;
  guint color_location = 0;
  GLint loc_res = 0, loc_pattern = 0, loc_factor = 0, loc_thickness = 0;

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
    g_print("Linking failure in program: %s", buffer);

    g_free(buffer);

    glDeleteProgram(program);
    program = 0;

    goto out;
  }

  mvp_location = glGetUniformLocation(program, "mvp_matrix");

  loc_res     = glGetUniformLocation(program, "u_resolution");
  loc_pattern = glGetUniformLocation(program, "u_pattern");
  loc_factor  = glGetUniformLocation(program, "u_factor");
  loc_thickness = glGetUniformLocation(program, "lineColor");

  // projection_location = glGetUniformLocation(program, "projection_matrix");
  // color_location = glGetAttribLocation(program, "color");

  glDetachShader(program, vertex);
  glDetachShader(program, fragment);

out:
  if (vertex != 0) glDeleteShader(vertex);
  if (fragment != 0) glDeleteShader(fragment);

  if (program_out != NULL) *program_out = program;
  if (mvp_location_out != NULL) *mvp_location_out = mvp_location;
  if (projection_location_out != NULL)
    *projection_location_out = projection_location;
  if (color_location_out != NULL) *color_location_out = color_location;
  if (loc_res_out != NULL) *loc_res_out = loc_res;
  if (loc_pattern_out != NULL) *loc_pattern_out = loc_pattern;
  if (loc_thickness_out != NULL) *loc_thickness_out = loc_thickness;
  if (loc_factor_out != NULL) *loc_factor_out = loc_factor;

  return program != 0;
}
