#pragma once

#include <GL/glew.h>

#include <unordered_map>
#include <memory>
#include <string>
#include <stdexcept>

class VertexShader;
class FragmentShader;

class ShaderProgram {
public:
  ShaderProgram(const std::string &vertexShaderFileName,
                const std::string &fragmentShaderFileName);
  ~ShaderProgram();

public:
  GLuint getProgramId() const;
  void useProgram() const;
  
  // Uniform management.
  int getUniformLocation(const std::string &uniformName) const;
  template <typename type>
  void setUniform(const std::string &name, const type &value);

  // Attribute management.
  int getAttributeLocation(const std::string &name) const;
  void setAttribute(const std::string &name, int size, GLenum type) const;

private:
  void fillUniformMap();
  void fillAttributeMap();

private:
  GLuint programID;
  VertexShader *vertexShader;
  FragmentShader *fragmentShader;
  std::unordered_map<std::string, int> uniformLocationsMap;
  std::unordered_map<std::string, int> attributeLocationsMap;
};
