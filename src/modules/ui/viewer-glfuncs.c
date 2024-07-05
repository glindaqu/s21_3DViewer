#include "viewer-glfuncs.h"

void init_buffers(ObjFile_t *objFile, guint position_index, guint color_index,
                  guint *vao_out) {
  guint vao, vbo, ebo;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);

  Vertex_t *vertices = calloc(objFile->verticesCount, sizeof(Vertex_t));
  for (int i = 0; i < objFile->verticesCount; i++) {
    vertices[i] = *(objFile->vertices[i]);
  }

  for (int i = 0; i < objFile->verticesCount; i++) {
    g_print("Vertex %d: (%f, %f, %f)\n", i, vertices[i].x, vertices[i].y,
            vertices[i].z);
  }

  // Create index data
  GLuint *indices = calloc(objFile->surfacesCount * 3, sizeof(GLuint));
  for (int i = 0; i < objFile->surfacesCount; i++) {
    Surface_t *surface = objFile->surfaces[i];
    indices[i * 3] = surface->verticesIndices[0];
    indices[i * 3 + 1] = surface->verticesIndices[1];
    indices[i * 3 + 2] = surface->verticesIndices[2];
  }

  for (int i = 0; i < objFile->surfacesCount * 3; i++) {
    g_print("Index %d: %d\n", i, indices[i]);
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
  glVertexAttribPointer(position_index, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_t),
                        (GLvoid *)0);
  glEnableVertexAttribArray(position_index);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    g_print("OpenGL error: %d\n", err);
  }

  free(vertices);
  free(indices);

  if (vao_out != NULL) *vao_out = vao;
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
                      guint *position_location_out, guint *color_location_out,
                      GError **error) {
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

  mvp_location = glGetUniformLocation(program, "mvp_matrix");

  // position_location = glGetAttribLocation(program, "position");
  // color_location = glGetAttribLocation(program, "color");

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
