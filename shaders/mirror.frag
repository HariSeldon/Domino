#version 330

uniform sampler2D texture;

in vec2 textureCoordinates;
//flat in vec3 normal;
flat in int applyTexture;

out vec4 outputColor;

void main() {
  outputColor = (applyTexture == 0) 
                    ? vec4(0, 0, 0, 1)
                    : texture2D(texture, textureCoordinates);
}
