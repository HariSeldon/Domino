#version 330

uniform sampler2D inputTexture;

in vec2 textureCoordinates;

out vec4 outputColor;

void main() {
  outputColor = (texture2D(inputTexture, textureCoordinates) + 
                 texture2D(inputTexture, textureCoordinates - (0, 1.f / 800.f)) +
                 texture2D(inputTexture, textureCoordinates - (0, 2.f / 800.f)) +
                 texture2D(inputTexture, textureCoordinates - (0, 3.f / 800.f)) +
                 texture2D(inputTexture, textureCoordinates - (0, 4.f / 800.f)) +
                 texture2D(inputTexture, textureCoordinates + (0, 4.f / 800.f)) +
                 texture2D(inputTexture, textureCoordinates + (0, 3.f / 800.f)) +
                 texture2D(inputTexture, textureCoordinates + (0, 2.f / 800.f)) +
                 texture2D(inputTexture, textureCoordinates + (0, 1.f / 800.f))) / 9;
}
