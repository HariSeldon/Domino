#version 330

uniform mat4 mvpMatrix;

in vec3 vertexPosition;

void main () {
  gl_Position = mvpMatrix * vec4(vertexPosition, 1.);
}
