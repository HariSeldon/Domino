#include <map>
#include <string>
#include <vector>

#include <GL/glew.h>

class TextureManager {
public:
  GLuint createTexture(const std::string &fileName);
  GLuint createTextureArray(const std::vector<std::string> &fileNames);

private:
  std::map<const std::string, GLuint> fileTextureMap;
  std::map<const std::string, GLuint> fileTextureArrayMap;
};
