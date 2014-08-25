#version 330

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;

uniform mat4 mvpShadowMatrix;

in vec3 vertexPosition;
in vec3 vertexNormal;

out vec3 position;
out vec3 normal;
out vec3 shadowPosition;

void main () {
  position = (modelViewMatrix * vec4(vertexPosition, 1.)).xyz;
  normal = normalize(normalMatrix * vertexNormal);
  
  shadowPosition = (mvpShadowMatrix * vec4(vertexPosition, 1.)).xyz;
    
  gl_Position = projectionMatrix * modelViewMatrix * vec4(vertexPosition, 1.);
}
