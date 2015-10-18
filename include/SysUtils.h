#pragma once

#include <chrono>
#include <iostream>
#include <string>

#include <GL/glew.h>

std::string getFileContent(const std::string &filePath);
void dumpGLInfo();
// -----------------------------------------------------------------------------
inline void checkOpenGLError(
    const std::string &description) {
#ifdef DEBUG
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

void dumpTextureToFile(GLuint inputTexture);
