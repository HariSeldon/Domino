#include "SysUtils.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include <GL/glew.h>

// -----------------------------------------------------------------------------
std::string readStream(std::ifstream &fileStream) {
  std::string text;

  // Get file size.
  fileStream.seekg(0, std::ios::end);
  unsigned long int fileSize = fileStream.tellg();

  // Allocate buffer.
  text.resize(fileSize + 1);

  // Rewind.
  fileStream.seekg(0, std::ios::beg);

  // Read.
  fileStream.read(&text[0], fileSize);
  fileStream.close();

  text[fileSize] = '\0';
  return text;
}

// -----------------------------------------------------------------------------
std::string getFileContent(const std::string &filePath) {
  std::ifstream fileStream(filePath.c_str());
  if (fileStream.is_open()) {
    return readStream(fileStream);
  } else {
    std::cerr << "Cannot open: " + filePath << "\n";
    exit(1);
  }
}

// -----------------------------------------------------------------------------
void dumpGLInfo() {
  auto majorVersion = 0;
  glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
  auto minorVersion = 0;
  glGetIntegerv(GL_MINOR_VERSION, &minorVersion); 

  std::cout << "GL Version:   " << majorVersion << "." << minorVersion << "\n";
}

// -----------------------------------------------------------------------------
int getTextureSize(GLuint inputTexture, GLint format) {
  const int TEX_LEVEL = 0;

  glBindTexture(GL_TEXTURE_2D, inputTexture);

  int elementSize = 0;
  GLint width, height;

  // GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, and GL_BGRA.
  switch (format) {
  case GL_RED: {
    GLint redSize = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, TEX_LEVEL, GL_TEXTURE_RED_SIZE,
                             &redSize);
    elementSize = redSize;
    break;
  }
  case GL_RG: {
    GLint redSize = 0, greenSize = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, TEX_LEVEL, GL_TEXTURE_RED_SIZE,
                             &redSize);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, TEX_LEVEL, GL_TEXTURE_GREEN_SIZE,
                             &greenSize);
    elementSize = redSize + greenSize;
    break;
  }
  case GL_RGB:
  case GL_BGR: {
    GLint redSize = 0, greenSize = 0, blueSize = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, TEX_LEVEL, GL_TEXTURE_RED_SIZE,
                             &redSize);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, TEX_LEVEL, GL_TEXTURE_GREEN_SIZE,
                             &greenSize);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, TEX_LEVEL, GL_TEXTURE_BLUE_SIZE,
                             &blueSize);
    elementSize = redSize + greenSize + blueSize;
    break;
  }
  case GL_RGBA:
  case GL_BGRA: {
    GLint redSize = 0, greenSize = 0, blueSize = 0, alphaSize = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, TEX_LEVEL, GL_TEXTURE_RED_SIZE,
                             &redSize);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, TEX_LEVEL, GL_TEXTURE_GREEN_SIZE,
                             &greenSize);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, TEX_LEVEL, GL_TEXTURE_BLUE_SIZE,
                             &blueSize);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, TEX_LEVEL, GL_TEXTURE_ALPHA_SIZE,
                             &alphaSize);
    elementSize = redSize + greenSize + blueSize + alphaSize;
    break;
  }
  case GL_DEPTH_COMPONENT16: {
    elementSize = 16;
    break;
  }
  case GL_DEPTH_COMPONENT24: {
    elementSize = 24;
    break;
  }
  default:
    assert(false && "Unknown format");
    break;
  }

  glGetTexLevelParameteriv(GL_TEXTURE_2D, TEX_LEVEL, GL_TEXTURE_RED_SIZE,
                           &format);

  glGetTexLevelParameteriv(GL_TEXTURE_2D, TEX_LEVEL, GL_TEXTURE_WIDTH, &width);
  checkOpenGLError("dumpTextureToFile: glGetTexLevelParameteriv");
  glGetTexLevelParameteriv(GL_TEXTURE_2D, TEX_LEVEL, GL_TEXTURE_HEIGHT,
                           &height);
  checkOpenGLError("dumpTextureToFile: glGetTexLevelParameteriv");

  glBindTexture(GL_TEXTURE_2D, 0);

  return width * height * elementSize;
}

// -----------------------------------------------------------------------------
void dumpTextureToFile(GLuint inputTexture) {
  const int TEX_LEVEL = 0;
  glBindTexture(GL_TEXTURE_2D, inputTexture);
  checkOpenGLError("dumpTextureToFile: glBindTexture");

  GLint format = 0;
  glGetTexLevelParameteriv(GL_TEXTURE_2D, TEX_LEVEL, GL_TEXTURE_INTERNAL_FORMAT,
                           &format);
  checkOpenGLError("dumpTextureToFile: glGetTexLevelParameteriv");

  int size = getTextureSize(inputTexture, format);
  std::cout << "Size :" << size << "\n";

  std::vector<unsigned char> pixels(size, 0);
  glGetTexImage(GL_TEXTURE_2D, 0, format, GL_FLOAT,
                static_cast<void *>(pixels.data()));
}
