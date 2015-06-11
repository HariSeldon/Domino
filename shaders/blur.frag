#version 330

uniform sampler2D inputTexture;

in vec2 textureCoordinates;

out vec4 outputColor;

const float GAUSS_VALUES[6] = float[6](
1.0f,
0.778800783071f,
0.367879441171f,
0.105399224562f,
0.018315638888f,
0.001930454136f);

const int FILTER_RADIUS = 5;
const float WIDTH = 1280.f;
const float HEIGHT = 800.f;

void main() {
  outputColor = vec4(0, 0, 0, 1);
  for (int row = -FILTER_RADIUS; row < FILTER_RADIUS; ++row) {
    for (int column = -FILTER_RADIUS; column < FILTER_RADIUS; ++column) {
      outputColor += 2 * GAUSS_VALUES[abs(row)] * GAUSS_VALUES[abs(column)] * texture2D(inputTexture, textureCoordinates + (row / HEIGHT, column / WIDTH)); 
    }
  }
//    outputColor += texture2D(inputTexture, textureCoordinates) + 
//                    GAUSS_VALUES[0] * texture2D(inputTexture, textureCoordinates + (row / HEIGHT, -1 / WIDTH)) +
//                    GAUSS_VALUES[1] * texture2D(inputTexture, textureCoordinates + (row / HEIGHT, -2 / WIDTH)) +
//                    GAUSS_VALUES[2] * texture2D(inputTexture, textureCoordinates + (row / HEIGHT, -3 / WIDTH)) +
//                    GAUSS_VALUES[3] * texture2D(inputTexture, textureCoordinates + (row / HEIGHT, -4 / WIDTH)) +
//                    GAUSS_VALUES[4] * texture2D(inputTexture, textureCoordinates + (row / HEIGHT, -5 / WIDTH)) +
//                    GAUSS_VALUES[4] * texture2D(inputTexture, textureCoordinates + (row / HEIGHT, 5 / WIDTH)) +
//                    GAUSS_VALUES[3] * texture2D(inputTexture, textureCoordinates + (row / HEIGHT, 4 / WIDTH)) +
//                    GAUSS_VALUES[2] * texture2D(inputTexture, textureCoordinates + (row / HEIGHT, 3 / WIDTH)) +
//                    GAUSS_VALUES[1] * texture2D(inputTexture, textureCoordinates + (row / HEIGHT, 2 / WIDTH)) +
//                    GAUSS_VALUES[0] * texture2D(inputTexture, textureCoordinates + (row / HEIGHT, 1 / WIDTH));
}
