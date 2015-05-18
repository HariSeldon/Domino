#pragma once

#include "ShaderProgram.h"

#include <vector>

class LightBulbShader : public ShaderProgram {
public:
  enum UniformName {
    mvpMatrix = 0,
  };

public:
  LightBulbShader(const std::string &vertexShaderFileName,
                  const std::string &fragmentShaderFileName);

public:
  ShaderType getType() const override {
    return ShaderType::shLightBulb;
  }

private:
  static std::vector<std::string> uniformNames;
};
