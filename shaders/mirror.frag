#version 330

uniform sampler2D texture;

in vec2 textureCoordinates;

out vec4 outputColor;

void main() {
  outputColor = vec4(texture2D(texture, textureCoordinates.xy));
}
