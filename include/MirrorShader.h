#pragma once

#include "ShaderProgram.h"

#include <vector>

class MirrorShader : public ShaderProgram {
public:
  enum UniformName {
    mvpMatrix,
    normalMatrix,
    mirrorSize,
    mirrorNormal,
    texture
  };

public:
  MirrorShader(const std::string &vertexShaderFileName,
               const std::string &fragmentShaderFileName);

public:
  ShaderType getType() const override { return ShaderType::shMirror; }

private:
  static std::vector<std::string> uniformNames;
};
