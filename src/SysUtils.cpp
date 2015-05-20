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
    std::cout << "Cannot open: " + filePath << "\n";
  }
}

// -----------------------------------------------------------------------------
void dumpGLInfo() {
  std::cout << "GL Version:   " << glGetString(GL_VERSION) << "\n";
  std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
            << "\n";
}

// -----------------------------------------------------------------------------
void checkOpenGLError(const std::string &description) {
  #ifndef NDEBUG
  GLenum error = glGetError();
  if (error == GL_NO_ERROR)
    return;

  std::string errorString;
  switch (error) {
  case GL_INVALID_ENUM: {
    errorString = "GL_INVALID_ENUM";
    break;
  }
  case GL_INVALID_VALUE: {
    errorString = "GL_INVALID_VALUE";
    break;
  }
  case GL_INVALID_OPERATION: {
    errorString = "GL_INVALID_OPERATION";
    break;
  }
  case GL_INVALID_FRAMEBUFFER_OPERATION: {
    errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
    break;
  }
  case GL_OUT_OF_MEMORY: {
    errorString = "GL_OUT_OF_MEMORY";
    break;
  }
  case GL_STACK_UNDERFLOW: {
    errorString = "GL_STACK_UNDERFLOW";
    break;
  }
  case GL_STACK_OVERFLOW: {
    errorString = "GL_STACK_OVERFLOW";
    break;
  }
  default: {
    errorString = "Unknown error";
    break;
  }
  }
  std::cerr << description << " Error: " << errorString << "\n";
  #endif
}

// -----------------------------------------------------------------------------
int getTextureSize(GLuint inputTexture, GLint format) {
  const int TEX_LEVEL = 0;

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
