#version 460

layout (location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

uniform mat4 mvp_matrix;
uniform mat4 projection_matrix;

flat out vec3 startPos;
out vec3 vertPos;
uniform vec3 lineColor;

out vec3 vColor;

void main() {
  vec4 pos = projection_matrix * mvp_matrix * vec4(aPos, 1.0);
  gl_Position = pos;
  vertPos = pos.xyz / pos.w;
  startPos = vertPos;
  vColor = lineColor;
}