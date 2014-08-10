#version 330

uniform sampler2D texture;

in vec2 textureCoordinates;

out vec4 outputColor;

void main() {
  outputColor = vec4(texture2D(texture, textureCoordinates.xy));
//  outputColor = vec4(textureCoordinates.xy, 0, 1);
//  outputColor = vec4(0, 0, 1, 1);
}
