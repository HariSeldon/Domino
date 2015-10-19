#include "PhongNormalMappingShader.h"

#include <cassert>
#include <string>

std::vector<std::string> PhongNormalMappingShader::uniformNames {
    "mvpMatrix", "modelViewMatrix",
    "normalMatrix",
    "ambientColor",
    "lightsNumber", "lightMask", 
    "textureArray", 
    "material.ambient",
    "material.specular", "material.shininess",
};

// -----------------------------------------------------------------------------
PhongNormalMappingShader::PhongNormalMappingShader(
    const std::string &vertexShaderFileName,
    const std::string &fragmentShaderFileName)
    : LightedObjectShader(vertexShaderFileName, fragmentShaderFileName) {
  m_uniformLocations = createUniformTable(uniformNames);
  assert(m_uniformLocations.size() == uniformNames.size() &&
         "Number of uniform locations does not match number of uniform names");
}
