#include "Shader.h"

#include <GL/glew.h>

#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

#include "SysUtils.h"

// Support functions.
void checkErrors(GLuint shaderId, const std::string &fileName);

// -----------------------------------------------------------------------------
Shader::Shader(const std::string &filePath, GLenum type) : type(type) {
  fileName = filePath.substr(filePath.find_last_of('/') + 1);
  shaderID = glCreateShader(type);
  std::string shaderSource = getFileContent(filePath.c_str());
  const GLchar *shaderContent = shaderSource.data();
  glShaderSource(shaderID, 1, &shaderContent, nullptr);
  glCompileShader(shaderID);
  checkErrors(shaderID, fileName);
}

Shader::~Shader() { 
  glDeleteShader(shaderID); }

GLenum Shader::getType() const { return type; }

const std::string &Shader::getFileName() const { return fileName; }

GLuint Shader::getID() const { return shaderID; }

// -----------------------------------------------------------------------------
VertexShader::VertexShader(const std::string &filePath)
    : Shader(filePath, GL_VERTEX_SHADER) {}

// -----------------------------------------------------------------------------
FragmentShader::FragmentShader(const std::string &filePath)
    : Shader(filePath, GL_FRAGMENT_SHADER) {}

// -----------------------------------------------------------------------------
void checkErrors(GLuint shaderId, const std::string &fileName) {
  if (!glIsShader(shaderId)) {
    throw std::runtime_error("Error creating shader from file: " + fileName);
  }

  int maxLength = 0;
  int errorLength = 0;
  glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);
  std::vector<char> errorMessage(maxLength);
  glGetShaderInfoLog(shaderId, maxLength, &errorLength, errorMessage.data());

  if (errorLength > 0) {
    throw std::runtime_error("Error compiling " + fileName + "\n" +
                             errorMessage.data());
  }
}
