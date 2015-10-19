#include "Shader.h"

#include <GL/glew.h>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "SysUtils.h"

// Support functions.
void checkErrors(GLuint shaderId, const std::string &fileName);

// -----------------------------------------------------------------------------
Shader::Shader(const std::string &filePath, GLenum type) : m_type(type) {
  m_fileName = filePath.substr(filePath.find_last_of('/') + 1);
  m_shaderID = glCreateShader(type);
  std::string shaderSource = getFileContent(filePath.c_str());
  const GLchar *shaderContent = shaderSource.data();
  glShaderSource(m_shaderID, 1, &shaderContent, nullptr);
  glCompileShader(m_shaderID);
  checkErrors(m_shaderID, m_fileName);
}

Shader::~Shader() { glDeleteShader(m_shaderID); }

GLenum Shader::getType() const { return m_type; }

const std::string &Shader::getFileName() const { return m_fileName; }

GLuint Shader::getID() const { return m_shaderID; }

// -----------------------------------------------------------------------------
VertexShader::VertexShader(const std::string &filePath)
    : Shader(filePath, GL_VERTEX_SHADER) {}

// -----------------------------------------------------------------------------
FragmentShader::FragmentShader(const std::string &filePath)
    : Shader(filePath, GL_FRAGMENT_SHADER) {}

// -----------------------------------------------------------------------------
void checkErrors(GLuint shaderId, const std::string &fileName) {
  if (!glIsShader(shaderId))
    std::cerr << "Error creating shader from file: " + fileName + "\n";

  int maxLength = 0;
  int errorLength = 0;
  glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);
  std::vector<char> errorMessage(maxLength);
  glGetShaderInfoLog(shaderId, maxLength, &errorLength, errorMessage.data());

  if (errorLength > 0)
    std::cerr << "Error compiling " + fileName + "\n" + errorMessage.data();
}
