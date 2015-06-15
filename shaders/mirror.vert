#version 330

uniform mat4 mvpMatrix;

in vec3 vertexPosition;
in vec2 vertexTextureCoordinates;

out vec2 textureCoordinates;

void main () {
  textureCoordinates = vertexTextureCoordinates;
  gl_Position = mvpMatrix * vec4(vertexPosition, 1.);
}
