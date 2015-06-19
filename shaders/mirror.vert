#version 330

uniform mat4 mvpMatrix;
uniform mat3 normalMatrix;

uniform vec2 mirrorSize;
uniform vec3 mirrorNormal;

in vec3 vertexPosition;
in vec3 vertexNormal;

//flat out vec3 normal;
flat out int applyTexture;
out vec2 textureCoordinates;

void main () {
  vec3 normal = normalMatrix * vertexNormal;
  applyTexture = int(length(mirrorNormal - normal) < 0.001);

  textureCoordinates = vec2(1, 0) + vec2(-1, 1) * ((vertexPosition.xy) / mirrorSize + vec2(0.5, 0.5));
  gl_Position = mvpMatrix * vec4(vertexPosition, 1.);
}
