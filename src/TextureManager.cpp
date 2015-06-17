#include "TextureManager.h"

#include "SysDefines.h"
#include "SysUtils.h"

#include "SDL2/SDL_image.h"
#include "SDL2/SDL_pixels.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <memory>

GLuint createTextureFromFile(const std::string &fileName);
GLuint createTextureArrayFromFiles(const std::vector<std::string> &fileNames);
GLint getTextureFormat(SDL_Surface *texSurface);
std::unique_ptr<unsigned char[]>
createUnifiedBuffer(const std::vector<SDL_Surface *> &texSurfaces);

//------------------------------------------------------------------------------
GLuint TextureManager::createTexture(const std::string &fileName) {
  auto textureIter = fileTextureMap.find(fileName);
  if (textureIter != fileTextureMap.end()) {
    return textureIter->second;
  }
  auto textureId = createTextureFromFile(fileName);
  fileTextureMap[fileName] = textureId;
  return textureId;
}

//------------------------------------------------------------------------------
GLuint
TextureManager::createTextureArray(const std::vector<std::string> &fileNames) {
  std::string concatenation;
  for (auto &fileName : fileNames) {
    concatenation += fileName; 
  }

  auto textureIter = fileTextureArrayMap.find(concatenation);
  if (textureIter != fileTextureArrayMap.end()) {
    return textureIter->second;
  }
  auto textureId = createTextureArrayFromFiles(fileNames);
  fileTextureArrayMap[concatenation] = textureId;
  return textureId;
}

//------------------------------------------------------------------------------
GLuint createTextureArrayFromFiles(const std::vector<std::string> &fileNames) {
  if (fileNames.size() == 0) {
    std::cerr << "Trying to create a texture array without input files.\n";
    exit(1);
  }

  std::vector<SDL_Surface *> texSurfaces;
  texSurfaces.reserve(fileNames.size());

  for (auto &fileName : fileNames) {
    SDL_Surface *texSurface = IMG_Load((TEXTURE_PATH + fileName).c_str());
    if (texSurface == nullptr) {
      std::cerr << "Cannot load texture from: " << fileName << "\n";
      exit(1);
    }

    texSurfaces.push_back(texSurface);
  }

  const auto textureCount = texSurfaces.size();
  auto format = getTextureFormat(texSurfaces[0]);
  auto height = texSurfaces[0]->h;
  auto width = texSurfaces[0]->w;
  for (auto surface : texSurfaces) {
    auto currentFormat = getTextureFormat(surface);
    if (currentFormat != format) {
      std::cerr << "The textures in the array must all have the same format"
                << "\n";
      exit(1);
    }

    auto currentHeight = surface->h;
    auto currentWidth = surface->w;
    if (currentHeight != height || currentWidth != width) {
      std::cerr << "The textures in the array must all have the same size\n";
      exit(1);
    }
  }
  auto buffer = createUnifiedBuffer(texSurfaces);

  std::for_each(texSurfaces.begin(), texSurfaces.end(),
                [](auto surface) { SDL_FreeSurface(surface); });

  GLuint arrayId = 0;
  // Create the texture object.
  glGenTextures(1, &arrayId);
  checkOpenGLError("Drawer: glGenTextures");

  glBindTexture(GL_TEXTURE_2D_ARRAY, arrayId);
  checkOpenGLError("Drawer: glBindTexture");

  glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, width, height, textureCount, 0,
               format, GL_UNSIGNED_BYTE,
               reinterpret_cast<void *>(buffer.get()));
  checkOpenGLError("Drawer: glTexSubImage3D");

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
  checkOpenGLError("Drawer: glTexParameteri");
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
  checkOpenGLError("Drawer: glTexParameteri");
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  checkOpenGLError("Drawer: glTexParameteri");
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  checkOpenGLError("Drawer: glTexParameteri");
  glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
  checkOpenGLError("Drawer: generateMipMap");

  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
  return arrayId;
}

//------------------------------------------------------------------------------
GLint getTextureFormat(SDL_Surface *texSurface) {
  GLint format = 0;
  switch (texSurface->format->BytesPerPixel) {
  case 3:
    format = GL_RGB;
    break;
  case 4:
    format = GL_RGBA;
    break;
  default:
    std::cerr << "Unknown texture format.\n";
    exit(1);
  };
  return format;
}

//------------------------------------------------------------------------------
GLuint createTextureFromFile(const std::string &fileName) {
  SDL_Surface *texSurface = IMG_Load((TEXTURE_PATH + fileName).c_str());
  if (texSurface == nullptr) {
    std::cerr << "Cannot load texture from: " << fileName << "\n";
    return 0;
  }

  GLuint currentTexture = 0;
  // Create the texture object.
  glGenTextures(1, &currentTexture);
  checkOpenGLError("Drawer: glGenTextures");

  glBindTexture(GL_TEXTURE_2D, currentTexture);
  checkOpenGLError("Drawer: glBindTexture");

  GLint format = getTextureFormat(texSurface);

  glTexImage2D(GL_TEXTURE_2D, 0, format, texSurface->w, texSurface->h, 0,
               format, GL_UNSIGNED_BYTE, texSurface->pixels);
  checkOpenGLError("Drawer: glTexImage2D");
  SDL_FreeSurface(texSurface);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  checkOpenGLError("Drawer: glTexParameteri");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  checkOpenGLError("Drawer: glTexParameteri");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  checkOpenGLError("Drawer: glTexParameteri");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  checkOpenGLError("Drawer: glTexParameteri");
  glGenerateMipmap(GL_TEXTURE_2D);
  checkOpenGLError("Drawer: generateMipMap");

  glBindTexture(GL_TEXTURE_2D, 0);
  return currentTexture;
}

//------------------------------------------------------------------------------
std::unique_ptr<unsigned char[]>
createUnifiedBuffer(const std::vector<SDL_Surface *> &texSurfaces) {
  auto surfaceSize = texSurfaces[0]->w * texSurfaces[0]->h *
                     texSurfaces[0]->format->BytesPerPixel;
  auto totalSize = texSurfaces.size() * surfaceSize;
  auto buffer = new unsigned char[totalSize];
  for (auto index = 0; index < texSurfaces.size(); ++index) {
    std::memcpy(buffer + index * surfaceSize, texSurfaces[index]->pixels,
                surfaceSize);
  }
  return std::unique_ptr<unsigned char[]>(buffer);
}
