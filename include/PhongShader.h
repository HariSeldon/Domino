#include "LightedObjectShader.h"

#include <iostream>

class PhongShader : public LightedObjectShader {
public:
  enum UniformName {
    mvpMatrix = 0,
    modelViewMatrix,
    normalMatrix,
    ambientColor,
    lightsNumber,
    lightMask,
    texture,
    materialAmbient,
    materialSpecular,
    materialShininess,
    uniformNamesNumber
  };

public:
  PhongShader(const std::string &vertexShaderFileName,
              const std::string &fragmentShaderFileName);
  ShaderType getType() const override {
    return ShaderType::shPhong;
  }

private:
  static std::vector<std::string> uniformNames;
};
