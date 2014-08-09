#version 330

in vec3 vertexPosition;
in vec2 vertexTextureCoordinates;

out vec2 textureCoordinates; 

void main() {
  textureCoordinates = vertexTextureCoordinates; 
  gl_Position = vec4(vertexPosition, 1.f);
}

