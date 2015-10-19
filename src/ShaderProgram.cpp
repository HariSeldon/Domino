#include "ShaderProgram.h"

#include "Shader.h"
#include "SysDefines.h"
#include "SysUtils.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

int queryUniformsNumber(GLuint programID);
std::vector<int> queryUniformNamesLength(GLuint programID);

int queryAttributesNumber(GLuint programID);
int queryAttributeLocation(GLuint programID, const char *attributeName);

void checkErrors(GLuint programID);

// -----------------------------------------------------------------------------
ShaderProgram::ShaderProgram(const std::string &vertexShaderFileName,
                             const std::string &fragmentShaderFileName)
    : m_vertexShader(new VertexShader(SHADER_PATH + vertexShaderFileName)),
      m_fragmentShader(new FragmentShader(SHADER_PATH + fragmentShaderFileName)) {

  m_programID = glCreateProgram();
  glAttachShader(m_programID, m_vertexShader->getID());
  glAttachShader(m_programID, m_fragmentShader->getID());
  glLinkProgram(m_programID);

  checkErrors(m_programID);

  GLint output;
  glGetProgramiv(m_programID, GL_LINK_STATUS, &output);
  assert(output == GL_TRUE);

  fillAttributeMap();
}

// -----------------------------------------------------------------------------
ShaderProgram::~ShaderProgram() {
  glUseProgram(0);
  delete m_vertexShader;
  delete m_fragmentShader;
  glDeleteProgram(m_programID);
}

// #############################################################################
// Uniform management.
GLint ShaderProgram::queryUniformLocation(GLuint programID,
                                          const char *uniformName) {
  GLint location = glGetUniformLocation(programID, uniformName);
  if (location == -1) {
    std::cerr << "Cannot query uniform: " << uniformName << "\n";
    exit(1);
  }
  return location;
}

template void ShaderProgram::setUniform(int nameIndex,
                                        const float &value) const;
template void ShaderProgram::setUniform(int nameIndex, const int &value) const;
template void ShaderProgram::setUniform(int nameIndex,
                                        const unsigned int &value) const;
template void ShaderProgram::setUniform(int nameIndex,
                                        const glm::vec2 &value) const;
template void ShaderProgram::setUniform(int nameIndex,
                                        const glm::vec3 &value) const;
template void ShaderProgram::setUniform(int nameIndex,
                                        const glm::vec4 &value) const;
template void ShaderProgram::setUniform(int nameIndex,
                                        const glm::mat4 &value) const;
template void ShaderProgram::setUniform(int nameIndex,
                                        const glm::mat3 &value) const;

template <typename type>
void ShaderProgram::setUniform(int nameIndex, const type &value) const {
  setUniformValue<type>(m_uniformLocations[nameIndex], value);
}

template <typename type>
void ShaderProgram::setUniformValue(GLint, const type &) const {
  return;
}

template <>
void ShaderProgram::setUniformValue(GLint location,
                                    const unsigned int &value) const {
  glUniform1i(location, value);
  checkOpenGLError("set Uniform with location: " + std::to_string(location));
}

template <>
void ShaderProgram::setUniformValue(GLint location, const int &value) const {
  glUniform1i(location, value);
  checkOpenGLError("set Uniform with location: " + std::to_string(location));
}

template <>
void ShaderProgram::setUniformValue(GLint location, const float &value) const {
  glUniform1f(location, value);
  checkOpenGLError("set Uniform with location: " + std::to_string(location));
}

template <>
void ShaderProgram::setUniformValue(GLint location,
                                    const glm::vec2 &vector) const {
  glUniform2f(location, vector.x, vector.y);
  checkOpenGLError("set Uniform with location: " + std::to_string(location));
}

template <>
void ShaderProgram::setUniformValue(GLint location,
                                    const glm::vec3 &vector) const {
  glUniform3f(location, vector.x, vector.y, vector.z);
  checkOpenGLError("set Uniform with location: " + std::to_string(location));
}

template <>
void ShaderProgram::setUniformValue(GLint location,
                                    const glm::vec4 &vector) const {
  glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
  checkOpenGLError("set Uniform with location: " + std::to_string(location));
}

template <>
void ShaderProgram::setUniformValue(GLint location,
                                    const glm::mat4 &matrix) const {
  glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
  checkOpenGLError("set Uniform with location: " + std::to_string(location));
}

template <>
void ShaderProgram::setUniformValue(GLint location,
                                    const glm::mat3 &matrix) const {
  glUniformMatrix3fv(location, 1, GL_FALSE, &matrix[0][0]);
  checkOpenGLError("set Uniform with location: " + std::to_string(location));
}

// -----------------------------------------------------------------------------
// This could return a vector.
std::vector<int> ShaderProgram::createUniformTable(
    const std::vector<std::string> &uniformNames) const {
  int numberOfUniforms = uniformNames.size();
  std::vector<int> uniformLocations(numberOfUniforms);
  for (int index = 0; index < numberOfUniforms; ++index) {
    GLint uniformLocation =
        queryUniformLocation(m_programID, uniformNames[index].c_str());
    uniformLocations[index] = uniformLocation;
  }

  return uniformLocations;
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
  if (attributeLocationsMap.find(name) == attributeLocationsMap.end()) {
    std::cerr << "Cannot set attribute: " << name << "\n";
  } else {
    int location = attributeLocationsMap.at(name);
    glVertexAttribPointer(location, size, type, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(location);
  }
}

// -----------------------------------------------------------------------------
void ShaderProgram::fillAttributeMap() {
  const auto ATTRIBUTE_NAME_MAX_SIZE = 256;
  std::vector<GLchar> nameRawData(ATTRIBUTE_NAME_MAX_SIZE);
  GLint arraySize = 0;
  GLenum type = 0;
  auto attributesNumber = queryAttributesNumber(m_programID);
  for (auto index = 0; index < attributesNumber; ++index) {
    GLsizei actualSize = 0;
    glGetActiveAttrib(m_programID, index, nameRawData.size(), &actualSize,
                      &arraySize, &type, &nameRawData[0]);
    std::string name(nameRawData.data(), actualSize);
    auto location = queryAttributeLocation(m_programID, name.c_str());
    attributeLocationsMap[name] = location;
  }
}

// -----------------------------------------------------------------------------
void ShaderProgram::printUniformLocations(
    const std::vector<std::string> &names,
    const std::vector<int> &locations) const {
  assert(locations.size() % names.size() == 0);
  std::cout << "-------------------------------------------------\n";
  for (auto index = 0u; index < locations.size(); ++index) {
    std::cout << names[index % names.size()] << " " << locations[index] << "\n";
  }
  std::cout << "-------------------------------------------------\n";
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
  if (!glIsProgram(programID))
    std::cerr << "Error in creating shader program"
              << "\n";

  int errorLength = 0;
  int maxLength = 0;
  glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);
  std::vector<char> errorMessage(maxLength);
  glGetProgramInfoLog(programID, maxLength, &errorLength, errorMessage.data());

  if (errorLength > 0)
    std::cerr << "Error in shader program:\n"
              << std::string(errorMessage.data()) << "\n";
}
