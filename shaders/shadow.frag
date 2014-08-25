#version 330

out float depth;
//out vec4 outputColor;

void main() {
  depth = 0.5 * gl_FragCoord.z + 0.5f;
//  outputColor = vec4(0.5 * gl_FragCoord.z + 0.5f, 0, 0, 1);
}
