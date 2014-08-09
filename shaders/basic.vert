#version 330

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;

in vec4 vertexPosition;
in vec4 vertexNormal;

//out vec3 normal;
//out vec3 position;

void main () {
//  position = (modelViewMatrix * vertexPosition).xyz;
//  normal = normalize(normalMatrix * vertexNormal.xyz);

  gl_Position = projectionMatrix * modelViewMatrix * vertexPosition + 0.0001 * vec4(normalMatrix * vec3(vertexNormal), 1);
}
