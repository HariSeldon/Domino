#version 330

uniform int lightMask;
uniform int lightIndex;

out vec4 outputColor;

void main() { 
  int color = 1 << lightIndex & lightMask;
  outputColor = vec4(1, 1, 1, color); 
}
