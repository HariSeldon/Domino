#version 330

uniform sampler2D texture;

in vec2 textureCoordinates;

out vec4 outputColor;

void main() {
  outputColor = (gl_FrontFacing) ? vec4(0,0,0,1) : texture2D(texture, textureCoordinates);
}
