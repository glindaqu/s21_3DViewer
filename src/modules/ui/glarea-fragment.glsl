#version 300 es

precision mediump float; // Add precision qualifier for floating-point types

in vec4 vertexColor;

out vec4 outputColor;

void main() {
  outputColor = vertexColor;
}