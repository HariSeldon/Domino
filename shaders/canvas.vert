#version 330

in vec2 vertexPosition;

out vec2 textureCoordinates;

void main() {
  textureCoordinates = (vertexPosition + vec2(1, 1)) / 2;
  gl_Position = vec4(vertexPosition, 0, 1);
}
