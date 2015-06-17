#version 330

uniform mat4 mvpMatrix;
uniform vec2 mirrorSize;

in vec3 vertexPosition;

out vec2 textureCoordinates;

void main () {
  textureCoordinates = vec2(1, 0) + vec2(-1, 1) * ((vertexPosition.xy) / mirrorSize + vec2(0.5, 0.5));
  gl_Position = mvpMatrix * vec4(vertexPosition, 1.);
}
