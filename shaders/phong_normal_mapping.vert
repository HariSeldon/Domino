#version 330

uniform mat4 mvpMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTextureCoordinates;
in vec3 vertexTangent;

out vec3 normal;
out vec3 position;
out vec2 textureCoordinates;
out vec3 tangent;

void main () {
  position = (modelViewMatrix * vec4(vertexPosition, 1.)).xyz;
  normal = normalize(normalMatrix * vertexNormal);
  // modelviewMatrix preserves the tangent, but not the normal.
  tangent = normalize(mat3(modelViewMatrix) * vertexTangent); 

  textureCoordinates = vertexTextureCoordinates;
  
  gl_Position = mvpMatrix * vec4(vertexPosition, 1.);
}
