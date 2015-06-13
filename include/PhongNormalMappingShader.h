#include "LightedObjectShader.h"

class PhongNormalMappingShader : public LightedObjectShader {
public:
  enum UniformName {
    mvpMatrix = 0,
    modelViewMatrix,
    normalMatrix,
    ambientColor,
    lightsNumber,
    lightMask,
    texture,
    normalTexture,
    materialAmbient,
    materialSpecular,
    materialShininess,
    uniformNamesNumber
  };

public:
  PhongNormalMappingShader(const std::string &vertexShaderFileName,
              const std::string &fragmentShaderFileName);
  ShaderType getType() const override {
    return ShaderType::shPhongNormalMapping;
  }

private:
  static std::vector<std::string> uniformNames;
};
