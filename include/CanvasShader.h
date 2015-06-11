#pragma once

#include "ShaderProgram.h"

#include <vector>

class CanvasShader : public ShaderProgram {
public:
  enum UniformName {
    firstTexture,
    secondTexture
  };

public:
  CanvasShader(const std::string &vertexShaderFileName,
               const std::string &fragmentShaderFileName);

public:
  ShaderType getType() const override { return ShaderType::shCanvas; }

private:
  static std::vector<std::string> uniformNames;
};
