#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <vector>

#include <GL/glew.h>

// -----------------------------------------------------------------------------
struct Glyph {
  // Sizes of the texture glyph.
  int width, height;
  // By how much to advance the pen for drawing this glyph.
  int offsetX, offsetY;
  // By how much to advance the pen for the next glyph.
  int advanceX, advanceY;
  // Boundaries of the texture in the atlas.
  float textureHorizontal0, textureHorizontal1;
  float textureVertical0, textureVertical1;
};

// -----------------------------------------------------------------------------
class CharacterAtlas {
public:
  CharacterAtlas(const std::string &fontFileName, int fontHeight);
  ~CharacterAtlas();

public:
  GLuint getTextureId() const;
  int getWidth() const;
  int getHeight() const;
  const Glyph& getGlyph(char character) const;

private:
  void computeTextureSize(FT_Face fontFace);
  void createTexture(FT_Face fontFace);
  void fillTexture(FT_Face fontFace);

private:
  FT_Face fontFace;
  FT_Library freeType;
  GLuint textureId;
  int width;
  int height;
  std::vector<Glyph> glyphs;
};
