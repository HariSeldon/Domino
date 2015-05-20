#pragma once

#include "CharacterAtlas.h"
#include "TextShader.h"

#include <string>
#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class TextManager {
public:
  TextManager(const std::string &fileName, int fontHeight,
              const glm::ivec2 &screenSize);
  ~TextManager();

public:
  void addText(const std::string &text, glm::vec2 penPosition);
  void renderText();

private:
  void setupVertexVBO();
  void setupTextureVBO();
  void setupIndexVBO();

private:
  CharacterAtlas atlas;
  TextShader textShader;
  glm::vec4 color;

  // Maps the size of a pixel onto the screen size.
  float vScale;
  float hScale;

  GLuint vaoId;
  GLuint vertexVBOId;
  GLuint textureVBOId;
  GLuint indexVBOId;

  // Buffer
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> textureCoos;
  std::vector<unsigned int> indices;
};
