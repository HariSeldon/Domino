#include "LightedObjectShader.h"

#include "SceneManager.h"

#include <iostream>

std::vector<std::string> LightedObjectShader::uniformLightNames{
    "ambient", "diffuse", "specular", "constantAttenuation",
    "linearAttenuation", "quadraticAttenuation", "spotCutOff", "spotCosCutOff",
    "spotExponent", "spotDirection", "position",
};

// -----------------------------------------------------------------------------
LightedObjectShader::LightedObjectShader(
    const std::string &vertexShaderFileName,
    const std::string &fragmentShaderFileName)
    : ShaderProgram(vertexShaderFileName, fragmentShaderFileName) {
  uniformLightLocations = createLightUniformTable(uniformLightNames);
}

// -----------------------------------------------------------------------------
std::vector<int> LightedObjectShader::createLightUniformTable(
    std::vector<std::string> &uniformLightNames) {
  const auto numberOfUniforms = uniformLightNames.size();
  std::vector<int> uniformLocations(SceneManager::MAX_LIGHTS_NUMBER *
                                    numberOfUniforms);
  for (auto lightNumber = 0; lightNumber < SceneManager::MAX_LIGHTS_NUMBER;
       ++lightNumber) {
    for (auto index = 0u; index < numberOfUniforms; ++index) {
      auto uniformName = "lights[" + std::to_string(lightNumber) + "]." +
                         uniformLightNames[index];
      GLint uniformLocation =
          queryUniformLocation(programID, uniformName.c_str());
      uniformLocations[lightNumber * numberOfUniforms + index] =
          uniformLocation;
    }
  }

  return uniformLocations;
}

// -----------------------------------------------------------------------------
template void LightedObjectShader::setLightUniform(int lightIndex,
                                                   int nameIndex,
                                                   const float &value) const;
template void LightedObjectShader::setLightUniform(int lightIndex,
                                                   int nameIndex,
                                                   const int &value) const;
template void
LightedObjectShader::setLightUniform(int lightIndex, int nameIndex,
                                     const unsigned int &value) const;
template void
LightedObjectShader::setLightUniform(int lightIndex, int nameIndex,
                                     const glm::vec4 &value) const;
template void
LightedObjectShader::setLightUniform(int lightIndex, int nameIndex,
                                     const glm::vec3 &value) const;
template void
LightedObjectShader::setLightUniform(int lightIndex, int nameIndex,
                                     const glm::mat4 &value) const;
template void
LightedObjectShader::setLightUniform(int lightIndex, int nameIndex,
                                     const glm::mat3 &value) const;

template <typename type>
void LightedObjectShader::setLightUniform(int lightIndex, const int nameIndex,
                                          const type &value) const {
  setUniformValue(uniformLightLocations[lightIndex *
                                   LightedObjectShader::lightUniformNumber +
                                   nameIndex],
             value);
}
