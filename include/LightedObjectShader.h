#pragma once

#include "ShaderProgram.h"

#include <vector>

class LightedObjectShader : public ShaderProgram {
public:
  enum LightUniformName {
    lightAmbient = 0,
    lightDiffuse,
    lightSpecular,
    lightConstantAttenuation,
    lightLinearAttenuation,
    lightQuadraticAttenuation,
    lightSpotCutOff,
    lightSpotCosCutOff,
    lightSpotExponent,
    lightSpotDirection,
    lightPosition,
    lightUniformNumber
  };

public:
  LightedObjectShader(const std::string &vertexShaderFileName,
                      const std::string &fragmentShaderFileName);

public:
  template <typename type>
  void setLightUniform(int lightIndex, int nameIndex, const type &value) const;
  virtual ShaderType getType() const override = 0; 

protected:
  std::vector<int>
  createLightUniformTable(std::vector<std::string> &uniformLightNames);

protected:
  std::vector<int> uniformLightLocations;
  static std::vector<std::string> uniformLightNames;
};
