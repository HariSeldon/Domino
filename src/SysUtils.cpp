#include "SysUtils.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>

#include <GL/glew.h>

// -----------------------------------------------------------------------------
std::string readStream(std::ifstream& fileStream) {
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
std::string getFileContent(const char* filePath) {
  std::ifstream fileStream(filePath);
  if (fileStream.is_open()) {
    return readStream(fileStream);
  } else {
    std::stringstream ss;
    ss << "Cannot open: " << filePath;
    throw std::runtime_error(ss.str());
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
//  exit(1);
}
