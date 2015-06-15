#version 330

uniform mat4 mvpMatrix;

in vec3 vertexPosition;

out vec2 textureCoordinates;

void main () {
  textureCoordinates = vec2(1, 0) + vec2(-1, 1) * ((vertexPosition.xy) / vec2(16, 10) + vec2(0.5, 0.5));
  gl_Position = mvpMatrix * vec4(vertexPosition, 1.);
}
