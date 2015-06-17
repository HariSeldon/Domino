#include "CharacterAtlas.h"

#include "SysUtils.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>

constexpr int FIRST_VISIBLE_CHAR = 32;
constexpr int LAST_VISIBLE_CHAR = 128;

CharacterAtlas::CharacterAtlas(const std::string &fontFileName, int fontHeight)
    : textureId(0), width(0), height(0),
      glyphs(LAST_VISIBLE_CHAR - FIRST_VISIBLE_CHAR, Glyph()) {
  if (FT_Init_FreeType(&freeType)) {
    std::cerr << "Could not init freetype library\n";
    exit(1);
  }

  if (FT_New_Face(freeType, fontFileName.data(), 0, &fontFace)) {
    std::cerr << "Could not open font: " << fontFileName << "\n";
    exit(1);
  }

  // Set the size of the font.
  FT_Set_Pixel_Sizes(fontFace, 0, fontHeight);
  computeTextureSize(fontFace);
  createTexture();
  fillTexture(fontFace);
}

// -----------------------------------------------------------------------------
void CharacterAtlas::computeTextureSize(FT_Face fontFace) {
  FT_GlyphSlot glyph = fontFace->glyph;

  // Find minimum size for a texture holding all visible ASCII glyphs.
  for (int character = FIRST_VISIBLE_CHAR; character < LAST_VISIBLE_CHAR;
       ++character) {

    int loadResult = FT_Load_Char(fontFace, character, FT_LOAD_RENDER);
    if (loadResult != 0) {
      std::cerr << "Error loading character " << character << "\n";
      exit(1);
    }

    // Leave one pixel of horizonal space between letters.
    width += glyph->bitmap.width + 1;
    // The height of the texture is going to be the height of the tallest
    // character.
    height = std::max(height, static_cast<int>(glyph->bitmap.rows));
  }
}

// -----------------------------------------------------------------------------
void CharacterAtlas::createTexture() {
  // Create a texture that will be used to hold all ASCII glyphs.
  glGenTextures(1, &textureId);
  checkOpenGLError("CharacterAtlas: glGenTextures");
  glBindTexture(GL_TEXTURE_2D, textureId);
  checkOpenGLError("CharacterAtlas: glBindTexture");

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED,
               GL_UNSIGNED_BYTE, nullptr);
  checkOpenGLError("CharacterAtlas glTexImage2D");

  // 1 Byte alignment.
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  checkOpenGLError("CharacterAtlas glPixelStorei");

  // Clamp at the edges.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  checkOpenGLError("CharacterAtlas glTexParameteri");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  checkOpenGLError("CharacterAtlas glTexParameteri");

  // Use linear filtering.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  checkOpenGLError("CharacterAtlas glTexParameteri");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  checkOpenGLError("CharacterAtlas glTexParameteri");

  // Unbind the texture.
  glBindTexture(GL_TEXTURE_2D, 0);
}

// -----------------------------------------------------------------------------
void CharacterAtlas::fillTexture(FT_Face fontFace) {

  glBindTexture(GL_TEXTURE_2D, textureId);

  FT_GlyphSlot glyph = fontFace->glyph;

  // Paste all glyph bitmaps into the texture, remembering the offset.
  int offset = 0;

  for (int character = FIRST_VISIBLE_CHAR; character < LAST_VISIBLE_CHAR;
       ++character) {
    int loadResult = FT_Load_Char(fontFace, character, FT_LOAD_RENDER);
    if (loadResult != 0) {
      std::cerr << "Error loading character " << character << "\n";
      exit(1);
    }

#ifdef DEBUG
    // Check the size of the texture. If the size is 0 means that no texture
    // is currently bound.
    int currentWidth = -1;
    int currentHeight = -1;

    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &currentWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT,
                             &currentHeight);

    assert("Current texture is empty" && currentWidth != 0 &&
           currentHeight != 0);
#endif

    glTexSubImage2D(GL_TEXTURE_2D, 0, offset, 0, glyph->bitmap.width,
                    glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE,
                    glyph->bitmap.buffer);
    checkOpenGLError("glTexSubImage2D");

    glyphs[character - FIRST_VISIBLE_CHAR].width = glyph->bitmap.width;
    glyphs[character - FIRST_VISIBLE_CHAR].height = glyph->bitmap.rows;

    glyphs[character - FIRST_VISIBLE_CHAR].offsetX = glyph->bitmap_left;
    glyphs[character - FIRST_VISIBLE_CHAR].offsetY = glyph->bitmap_top;

    // The advancement is in 1/64th of a pixel.
    glyphs[character - FIRST_VISIBLE_CHAR].advanceX = glyph->advance.x >> 6;
    glyphs[character - FIRST_VISIBLE_CHAR].advanceY = glyph->advance.y >> 6;

    glyphs[character - FIRST_VISIBLE_CHAR].textureHorizontal0 =
        static_cast<float>(offset) / static_cast<float>(width);
    glyphs[character - FIRST_VISIBLE_CHAR].textureHorizontal1 =
        static_cast<float>(offset + glyph->bitmap.width) /
        static_cast<float>(width);
    glyphs[character - FIRST_VISIBLE_CHAR].textureVertical0 = 0.f;
    glyphs[character - FIRST_VISIBLE_CHAR].textureVertical1 =
        static_cast<float>(glyph->bitmap.rows) / static_cast<float>(height);

    offset += glyph->bitmap.width + 1;
  }
  glBindTexture(GL_TEXTURE_2D, 0);
}

// -----------------------------------------------------------------------------
GLuint CharacterAtlas::getTextureId() const { return textureId; }

// -----------------------------------------------------------------------------
int CharacterAtlas::getWidth() const { return width; }

// -----------------------------------------------------------------------------
int CharacterAtlas::getHeight() const { return height; }

// -----------------------------------------------------------------------------
const Glyph &CharacterAtlas::getGlyph(char character) const {
  return glyphs[character - FIRST_VISIBLE_CHAR];
}

// -----------------------------------------------------------------------------
CharacterAtlas::~CharacterAtlas() {
  glDeleteTextures(1, &textureId);
  FT_Done_Face(fontFace);
  FT_Done_FreeType(freeType);
}
