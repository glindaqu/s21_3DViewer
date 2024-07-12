#include "../../include/viewer-glfuncs.h"
#include "../../include/viewer-error.h"

void glDelBuffers(GLbuffers_t *buffer) {
  if (buffer->vao != 0) glDeleteVertexArrays(1, &buffer->vao);
  if (buffer->vbo != 0) glDeleteBuffers(1, &buffer->vbo);
  if (buffer->ebo != 0) glDeleteBuffers(1, &buffer->ebo);
}

void glDelProgram(GLuint* program) {
  if (*program != 0) glDeleteProgram(*program);
}

void init_buffers(ObjFile_t *objFile, GLbuffers_t *buffers_out) {
  GLbuffers_t buffers;

  glGenVertexArrays(1, &buffers.vao);
  glGenBuffers(1, &buffers.vbo);
  glGenBuffers(1, &buffers.ebo);

  glBindVertexArray(buffers.vao);

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
  glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo);
  glBufferData(GL_ARRAY_BUFFER, objFile->verticesCount * sizeof(Vertex_t),
               vertices, GL_STATIC_DRAW);

  // Prepare index data
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.ebo);
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

  if (buffers_out != NULL) *buffers_out = buffers;
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

gboolean init_shaders(GLshader_vars_t *vars_out, GError **error) {
    GBytes *source;
    GLshader_vars_t vars;
    guint vertex = 0, fragment = 0, point = 0;

    source = g_resources_lookup_data("/src/modules/ui/school21/gdy/_3dviewer/glarea-vertex.glsl", 0, NULL);
    if (!create_shader(GL_VERTEX_SHADER, g_bytes_get_data(source, NULL), error, &vertex)) {
        g_bytes_unref(source);
        return FALSE;
    }
    g_bytes_unref(source);

    source = g_resources_lookup_data("/src/modules/ui/school21/gdy/_3dviewer/glarea-fragment.glsl", 0, NULL);
    if (!create_shader(GL_FRAGMENT_SHADER, g_bytes_get_data(source, NULL), error, &fragment)) {
        g_bytes_unref(source);
        glDeleteShader(vertex);
        return FALSE;
    }
    g_bytes_unref(source);

    source = g_resources_lookup_data("/src/modules/ui/school21/gdy/_3dviewer/glarea-point.glsl", 0, NULL);
    if (!create_shader(GL_FRAGMENT_SHADER, g_bytes_get_data(source, NULL), error, &point)) {
        g_bytes_unref(source);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return FALSE;
    }
    g_bytes_unref(source);

    vars.program = glCreateProgram();
    vars.point_program = glCreateProgram();

    glAttachShader(vars.program, vertex);
    glAttachShader(vars.program, fragment);
    glLinkProgram(vars.program);

    int status = 0;
    glGetProgramiv(vars.program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        int log_len = 0;
        glGetProgramiv(vars.program, GL_INFO_LOG_LENGTH, &log_len);
        char *buffer = g_malloc(log_len + 1);
        glGetProgramInfoLog(vars.program, log_len, NULL, buffer);
        g_set_error(error, VIEWER_ERROR, VIEWER_ERROR_SHADER_LINK, "Linking failure in program: %s", buffer);
        g_print("Linking failure in program: %s", buffer);
        g_free(buffer);
        glDeleteProgram(vars.program);
        vars.program = 0;
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteShader(point);
        if (vars_out != NULL) *vars_out = vars;
        return FALSE;
    }

    glAttachShader(vars.point_program, vertex);
    glAttachShader(vars.point_program, point);
    glLinkProgram(vars.point_program);

    status = 0;
    glGetProgramiv(vars.point_program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        int log_len = 0;
        glGetProgramiv(vars.point_program, GL_INFO_LOG_LENGTH, &log_len);
        char *buffer = g_malloc(log_len + 1);
        glGetProgramInfoLog(vars.point_program, log_len, NULL, buffer);
        g_set_error(error, VIEWER_ERROR, VIEWER_ERROR_SHADER_LINK, "Linking failure in program: %s", buffer);
        g_print("Linking failure in program: %s", buffer);
        g_free(buffer);
        glDeleteProgram(vars.point_program);
        vars.point_program = 0;
        glDeleteProgram(vars.program);
        vars.program = 0;
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteShader(point);
        if (vars_out != NULL) *vars_out = vars;
        return FALSE;
    }

    vars.mvp_location = glGetUniformLocation(vars.program, "mvp_matrix");
    vars.loc_res = glGetUniformLocation(vars.program, "u_resolution");
    vars.loc_pattern = glGetUniformLocation(vars.program, "u_pattern");
    vars.loc_factor = glGetUniformLocation(vars.program, "u_factor");
    vars.loc_lineColor = glGetUniformLocation(vars.program, "lineColor");
    vars.projection_location = glGetUniformLocation(vars.program, "projection_matrix");

    glDetachShader(vars.point_program, vertex);
    glDetachShader(vars.point_program, point);
    glDetachShader(vars.program, vertex);
    glDetachShader(vars.program, fragment);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glDeleteShader(point);

    if (vars_out != NULL) *vars_out = vars;

    return vars.program != 0;
}
