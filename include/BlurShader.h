#pragma once

#include "ShaderProgram.h"

#include <vector>

class BlurShader : public ShaderProgram {
public:
  enum UniformName {
    inputTexture,
  };

public:
  BlurShader(const std::string &vertexShaderFileName,
             const std::string &fragmentShaderFileName);

public:
  ShaderType getType() const override { return ShaderType::shCanvas; }

private:
  static std::vector<std::string> uniformNames;
};
