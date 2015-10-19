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
  m_uniformLightLocations = createLightUniformTable(uniformLightNames);
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
          queryUniformLocation(m_programID, uniformName.c_str());
      uniformLocations[lightNumber * numberOfUniforms + index] =
          uniformLocation;
    }
  }

  return uniformLocations;
}

// -----------------------------------------------------------------------------
template void LightedObjectShader::setLightUniform(int, int,
                                                   const float &) const;
template void LightedObjectShader::setLightUniform(int, int, const int &) const;
template void LightedObjectShader::setLightUniform(int, int,
                                                   const unsigned int &) const;
template void LightedObjectShader::setLightUniform(int, int,
                                                   const glm::vec4 &) const;
template void LightedObjectShader::setLightUniform(int, int,
                                                   const glm::vec3 &) const;
template void LightedObjectShader::setLightUniform(int, int,
                                                   const glm::mat4 &) const;
template void LightedObjectShader::setLightUniform(int, int,
                                                   const glm::mat3 &) const;

template <typename type>
void LightedObjectShader::setLightUniform(int lightIndex, const int nameIndex,
                                          const type &value) const {
  setUniformValue(
      m_uniformLightLocations[lightIndex *
                                  LightedObjectShader::lightUniformNumber +
                              nameIndex],
      value);
}
