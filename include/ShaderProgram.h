#pragma once

#include <GL/glew.h>

#include "SysUtils.h"

#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class VertexShader;
class FragmentShader;

enum class ShaderType {
  shText,
  shPhong,
  shPhongNormalMapping,
  shLightBulb,
  shCanvas,
  shMirror
};

class ShaderProgram {
public:
  static std::vector<std::string> uniformNames;
  static std::vector<std::string> lightUniformNames;

public:
  ShaderProgram(const std::string &vertexShaderFileName,
                const std::string &fragmentShaderFileName);
  virtual ~ShaderProgram();

public:
  inline GLuint getProgramId() const {
    return m_programID;
  }
  inline void useProgram() const {
    glUseProgram(m_programID);
    checkOpenGLError("ShaderProgram: useProgram-glUseProgram");
  }

  virtual ShaderType getType() const = 0 ;

  // Uniform management.
  template <typename type>
  void setUniform(int nameIndex, const type &value) const;

  // Attribute management.
  int getAttributeLocation(const std::string &name) const;
  void setAttribute(const std::string &name, int size, GLenum type) const;

protected:
  std::vector<int>
  createUniformTable(const std::vector<std::string> &uniformNames) const;
  void fillAttributeMap();
  static int queryUniformLocation(GLuint programID, const char *uniformName);
  void printUniformLocations(const std::vector<std::string> &names,
                             const std::vector<int> &locations) const;
  template <typename type>
    void setUniformValue(GLint location, const type &value) const;

protected:
  std::vector<int> m_uniformLocations;

  GLuint m_programID = 0;
  VertexShader *m_vertexShader = nullptr;
  FragmentShader *m_fragmentShader = nullptr;
  std::unordered_map<std::string, int> attributeLocationsMap;
};
