#version 330

in vec2 vertexPosition;

void main() {
  gl_Position = vec4(vertexPosition, 0, 3);
}
