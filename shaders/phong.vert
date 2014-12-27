#version 330

// FIXME: replace the projectionMatrix with mvpMatrix -> this saves a MM.
uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTextureCoordinates;

out vec3 normal;
out vec3 position;
out vec2 textureCoordinates;

void main () {
  position = (modelViewMatrix * vec4(vertexPosition, 1.)).xyz;
  normal = normalize(normalMatrix * vertexNormal);

  textureCoordinates = vertexTextureCoordinates;
  
  gl_Position = projectionMatrix * modelViewMatrix * vec4(vertexPosition, 1.);
}
