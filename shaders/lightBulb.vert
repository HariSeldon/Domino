#version 330

uniform mat4 mvpMatrix;

in vec4 vertexPosition;

void main () {
  gl_Position = mvpMatrix * vertexPosition;
}
