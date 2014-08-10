#version 330

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

in vec3 vertexPosition;
in vec2 vertexTextureCoordinates;

out vec2 textureCoordinates;

void main () {
  textureCoordinates = vertexTextureCoordinates;
  gl_Position = projectionMatrix * modelViewMatrix * vec4(vertexPosition, 1.);
}
