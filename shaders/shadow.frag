#version 330

out float depth;

void main() {
  depth = 0;
  if (gl_FrontFacing == true) 
    depth = 0.5 * gl_FragCoord.z + 0.5f;
}
