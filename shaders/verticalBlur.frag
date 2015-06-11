#version 330

uniform sampler2D inputTexture;

in vec2 textureCoordinates;

out vec4 outputColor;

const float GAUSS_VALUES[5] = float[5](
0.778800783071f,
0.367879441171f,
0.105399224562f,
0.018315638888f,
0.001930454136f);

void main() {
  outputColor = texture2D(inputTexture, textureCoordinates) + 
                GAUSS_VALUES[0] * texture2D(inputTexture, textureCoordinates - (0, 1 / 800.f)) +
                GAUSS_VALUES[1] * texture2D(inputTexture, textureCoordinates - (0, 2 / 800.f)) +
                GAUSS_VALUES[2] * texture2D(inputTexture, textureCoordinates - (0, 3 / 800.f)) +
                GAUSS_VALUES[3] * texture2D(inputTexture, textureCoordinates - (0, 4 / 800.f)) +
                GAUSS_VALUES[4] * texture2D(inputTexture, textureCoordinates - (0, 5 / 800.f)) +
                GAUSS_VALUES[4] * texture2D(inputTexture, textureCoordinates + (0, 5 / 800.f)) +
                GAUSS_VALUES[3] * texture2D(inputTexture, textureCoordinates + (0, 4 / 800.f)) +
                GAUSS_VALUES[2] * texture2D(inputTexture, textureCoordinates + (0, 3 / 800.f)) +
                GAUSS_VALUES[1] * texture2D(inputTexture, textureCoordinates + (0, 2 / 800.f)) +
                GAUSS_VALUES[0] * texture2D(inputTexture, textureCoordinates + (0, 1 / 800.f));
}
