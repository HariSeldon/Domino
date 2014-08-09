#version 330

uniform vec4 color;
uniform sampler2D texture;

in vec2 textureCoordinates;

out vec4 outputColor;

void main() {
  float alpha = texture2D(texture, textureCoordinates.xy).r;
  outputColor = vec4(1, 1, 1, alpha) * color;
}
