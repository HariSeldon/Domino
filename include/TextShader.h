#pragma once

#include "ShaderProgram.h"

class TextShader : public ShaderProgram {
public:
  // Make sure that the fields of the enum match the names in uniformNames.
  enum UniformName {
    color = 0,
    texture,
    uniformNamesNumber
  };

public:
  TextShader(const std::string &vertexShaderFileName,
             const std::string &fragmentShaderFileName);

  ShaderType getType() const override {
    return ShaderType::shText;
  }

private:
  static std::vector<std::string> uniformNames;
};
