#include "ShaderProgram.h"

#include "Shader.h"
#include "SysDefines.h"
#include "SysUtils.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <iostream>

int queryUniformsNumber(GLuint programID);
int queryUniformLocation(GLuint programID, const char *uniformName);
std::vector<int> queryUniformNamesLength(GLuint programID);
template <typename type>
void setUniformValue(GLint location, const type &value);

int queryAttributesNumber(GLuint programID);
int queryAttributeLocation(GLuint programID, const char *attributeName);

void checkErrors(GLuint programID);

// -----------------------------------------------------------------------------
ShaderProgram::ShaderProgram(const std::string &vertexShaderFileName,
                             const std::string &fragmentShaderFileName)
    : vertexShader(new VertexShader(SHADER_PATH + vertexShaderFileName)),
      fragmentShader(new FragmentShader(SHADER_PATH + fragmentShaderFileName)) {

  programID = glCreateProgram();
  glAttachShader(programID, vertexShader->getID());
  glAttachShader(programID, fragmentShader->getID());
  glLinkProgram(programID);
  checkErrors(programID);
  fillUniformMap();
  fillAttributeMap();
}

ShaderProgram::~ShaderProgram() {
  glUseProgram(0);
  delete vertexShader;
  delete fragmentShader;
  glDeleteProgram(programID);
}

GLuint ShaderProgram::getProgramId() const { return programID; }

void ShaderProgram::useProgram() const {
  glUseProgram(programID);
  checkOpenGLError("ShaderProgram: useProgram-glUseProgram");
}

// #############################################################################
// Uniform management.
int ShaderProgram::getUniformLocation(const std::string &uniformName) const {
  return uniformLocationsMap.at(uniformName);
}

GLint queryUniformLocation(GLuint programID, const char *uniformName) {
  GLint location = glGetUniformLocation(programID, uniformName);
  assert(location != -1 && "Error querying uniform location");
  return location;
}

template void ShaderProgram::setUniform(const std::string &name,
                                        const float &value);
template void ShaderProgram::setUniform(const std::string &name,
                                        const int &value);
template void ShaderProgram::setUniform(const std::string &name,
                                        const unsigned char &value);
template void ShaderProgram::setUniform(const std::string &name,
                                        const unsigned int &value);
template void ShaderProgram::setUniform(const std::string &name,
                                        const glm::vec4 &value);
template void ShaderProgram::setUniform(const std::string &name,
                                        const glm::vec3 &value);
template void ShaderProgram::setUniform(const std::string &name,
                                        const glm::mat4 &value);
template void ShaderProgram::setUniform(const std::string &name,
                                        const glm::mat3 &value);
template <typename type>
void ShaderProgram::setUniform(const std::string &name, const type &value) {
  auto iter = uniformLocationsMap.find(name);
  if (iter == uniformLocationsMap.end()) {
    std::cout << "Error setting: " << name << "\n";
    return;
  }
  setUniformValue<type>(uniformLocationsMap[name], value);
}

template <typename type> void setUniformValue(GLint, const type &) { return; }

template <> void setUniformValue(GLint location, const unsigned int &value) {
  glUniform1i(location, value);
}

template <> void setUniformValue(GLint location, const int &value) {
  glUniform1i(location, value);
}

template <> void setUniformValue(GLint location, const float &value) {
  glUniform1f(location, value);
}

template <> void setUniformValue(GLint location, const glm::vec3 &vector) {
  glUniform3f(location, vector.x, vector.y, vector.z);
}

template <> void setUniformValue(GLint location, const glm::vec4 &vector) {
  glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

template <> void setUniformValue(GLint location, const glm::mat4 &matrix) {
  glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
}

template <> void setUniformValue(GLint location, const glm::mat3 &matrix) {
  glUniformMatrix3fv(location, 1, GL_FALSE, &matrix[0][0]);
}

// -----------------------------------------------------------------------------
void ShaderProgram::fillUniformMap() {
  std::vector<int> sizes = queryUniformNamesLength(programID);
  GLenum type = 0;
  GLint arraySize = 0;
  for (size_t index = 0; index < sizes.size(); ++index) {
    GLsizei actualSize = 0;
    std::vector<GLchar> nameRawData(sizes[index]);
    glGetActiveUniform(programID, index, nameRawData.size(), &actualSize,
                       &arraySize, &type, nameRawData.data());
    std::string name(nameRawData.data(), actualSize);
    GLint uniformLocation = queryUniformLocation(programID, name.c_str());
    uniformLocationsMap[name] = static_cast<int>(uniformLocation);
  }
}

// -----------------------------------------------------------------------------
int queryUniformsNumber(GLuint programID) {
  GLint uniformsNumber = -1;
  glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &uniformsNumber);
  assert(uniformsNumber != -1 &&
         "Error querying the number of active uniforms");
  return static_cast<int>(uniformsNumber);
}

// -----------------------------------------------------------------------------
std::vector<int> queryUniformNamesLength(GLuint programID) {
  int uniformsNumber = queryUniformsNumber(programID);
  // Uniform indices to be queries.
  std::vector<unsigned int> uniformIndices(uniformsNumber, 1);
  std::iota(uniformIndices.begin(), uniformIndices.end(), 0);

  // Sizes of uniformNames.
  std::vector<int> sizes(uniformsNumber, 0);

  // Query the sizes of the uniform names.
  glGetActiveUniformsiv(programID, uniformsNumber, uniformIndices.data(),
                        GL_UNIFORM_NAME_LENGTH, sizes.data());
  return sizes;
}

// #############################################################################
// Attribute management.
int ShaderProgram::getAttributeLocation(const std::string &name) const {
  return attributeLocationsMap.at(name);
}

// -----------------------------------------------------------------------------
void ShaderProgram::setAttribute(const std::string &name, int size,
                                 GLenum type) const {
  int location = 0;
  try {
    location = attributeLocationsMap.at(name);
  }
  catch (std::out_of_range &exception) {
    std::cout << "Cannot set attibute: " << name << "\n";
    return;
  }

  glVertexAttribPointer(location, size, type, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(location);
}

// -----------------------------------------------------------------------------
void ShaderProgram::fillAttributeMap() {
  int attributesNumber = queryAttributesNumber(programID);
  const int ATTRIBUTE_NAME_MAX_SIZE = 256;
  std::vector<GLchar> nameRawData(ATTRIBUTE_NAME_MAX_SIZE);
  GLint arraySize = 0;
  GLenum type = 0;
  for (int index = 0; index < attributesNumber; ++index) {
    GLsizei actualSize = 0;
    glGetActiveAttrib(programID, index, nameRawData.size(), &actualSize,
                      &arraySize, &type, &nameRawData[0]);
    std::string name(nameRawData.data(), actualSize);
    int location = queryAttributeLocation(programID, name.c_str());
    attributeLocationsMap[name] = location;
  }
}

// -----------------------------------------------------------------------------
int queryAttributesNumber(GLuint programID) {
  GLint attributesNumber = -1;
  glGetProgramiv(programID, GL_ACTIVE_ATTRIBUTES, &attributesNumber);
  assert(attributesNumber != -1 &&
         "Error querying the number of active attributes");
  return static_cast<int>(attributesNumber);
}

// -----------------------------------------------------------------------------
int queryAttributeLocation(GLuint programID, const char *attributeName) {
  GLint location = glGetAttribLocation(programID, attributeName);
  assert(location != -1 && "Error querying attribute location");
  return location;
}

// -----------------------------------------------------------------------------
void checkErrors(GLuint programID) {
  if (!glIsProgram(programID)) {
    throw std::runtime_error("Error in creating shader program");
  }

  int errorLength = 0;
  int maxLength = 0;
  glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);
  std::vector<char> errorMessage(maxLength);
  glGetProgramInfoLog(programID, maxLength, &errorLength, errorMessage.data());

  if (errorLength > 0) {
    throw std::runtime_error("Error in shader program:\n" +
                             std::string(errorMessage.data()));
  }
}
