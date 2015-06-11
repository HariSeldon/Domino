#version 330

uniform sampler2D firstTexture;
uniform sampler2D secondTexture;

in vec2 textureCoordinates;

out vec4 outputColor;

void main() {
  outputColor = texture2D(firstTexture, textureCoordinates) + 
                texture2D(secondTexture, textureCoordinates);
}
