#include "TextManager.h"

#ifndef WINDOWS

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <algorithm>
#include <functional>

// -----------------------------------------------------------------------------
TextManager::TextManager(const std::string &fileName, int fontHeight,
                         const glm::ivec2 &screenSize)
    : atlas(fileName, fontHeight),
      textShader("text.vert", "text.frag"),
      color({ 1, 1, 1, 1 }), vScale(2.f / screenSize.y),
      hScale(2.f / screenSize.x) {
  glGenVertexArrays(1, &vaoId);
  glGenBuffers(1, &indexVBOId);
  glGenBuffers(1, &textureVBOId);
  glGenBuffers(1, &vertexVBOId);
}

// -----------------------------------------------------------------------------
TextManager::~TextManager() {
  glBindVertexArray(0);

  glDeleteVertexArrays(1, &vaoId);

  glDeleteBuffers(1, &vertexVBOId);
  glDeleteBuffers(1, &textureVBOId);
  glDeleteBuffers(1, &indexVBOId);
}

// -----------------------------------------------------------------------------
void TextManager::addText(const std::string &text, glm::vec2 penPosition) {

  // Scale and traslate the pen position.
  penPosition = penPosition * glm::vec2(hScale, vScale) - glm::vec2(1, 1);

  int charIndex = 0;
  for (const char &character : text) {
    const Glyph &glyph = atlas.getGlyph(character);

    if (glyph.width == 0 || glyph.height == 0) {
      penPosition.x += hScale * glyph.advanceX;
      continue;
    }

    float glyphStartX = penPosition.x + hScale * glyph.offsetX;
    float glyphStartY = penPosition.y + vScale * glyph.offsetY;
    float glyphEndX = glyphStartX + hScale * glyph.width;
    float glyphEndY = glyphStartY - vScale * glyph.height;

    float textureHorizontal0 = glyph.textureHorizontal0;
    float textureHorizontal1 = glyph.textureHorizontal1;
    float textureVertical0 = glyph.textureVertical0;
    float textureVertical1 = glyph.textureVertical1;

    vertices.insert(vertices.end(), { { glyphStartX, glyphStartY, 0.f },
                                      { glyphEndX, glyphStartY, 0.f },
                                      { glyphEndX, glyphEndY, 0.f },
                                      { glyphStartX, glyphEndY, 0.f } });

    textureCoos.insert(textureCoos.end(),
                       { { textureHorizontal0, textureVertical0 },
                         { textureHorizontal1, textureVertical0 },
                         { textureHorizontal1, textureVertical1 },
                         { textureHorizontal0, textureVertical1 } });

    std::vector<unsigned int> currentIndices = { 0, 1, 2, 2, 3, 0 };
    std::vector<unsigned int> currentOffset(currentIndices.size(),
                                            4 * charIndex);
    std::transform(currentIndices.begin(), currentIndices.end(),
                   currentOffset.begin(), currentIndices.begin(),
                   std::plus<int>());

    indices.insert(indices.end(), currentIndices.begin(), currentIndices.end());

    penPosition.x += hScale * glyph.advanceX;

    ++charIndex;
  }
}

// -----------------------------------------------------------------------------
void TextManager::renderText() {
  textShader.useProgram();
  glBindVertexArray(vaoId);

  setupVertexVBO();
  setupTextureVBO();
  setupIndexVBO();

  glBindTexture(GL_TEXTURE_2D, atlas.getTextureId());

  // FIXME: Check this 0.
  textShader.setUniform(TextShader::texture, 0);
  textShader.setUniform(TextShader::color, color);

  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);

  // Clear the cache buffer.
  vertices.clear();
  textureCoos.clear();
  indices.clear();
}

// -----------------------------------------------------------------------------
void TextManager::setupVertexVBO() {
  glBindBuffer(GL_ARRAY_BUFFER, vertexVBOId);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
               vertices.data(), GL_STATIC_DRAW);
  textShader.setAttribute("vertexPosition", 3, GL_FLOAT);
}

// -----------------------------------------------------------------------------
void TextManager::setupTextureVBO() {
  glBindBuffer(GL_ARRAY_BUFFER, textureVBOId);
  glBufferData(GL_ARRAY_BUFFER, textureCoos.size() * sizeof(glm::vec2),
               textureCoos.data(), GL_STATIC_DRAW);
  textShader.setAttribute("vertexTextureCoordinates", 2, GL_FLOAT);
}

// -----------------------------------------------------------------------------
void TextManager::setupIndexVBO() {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);
}

#endif // WINDOWS
