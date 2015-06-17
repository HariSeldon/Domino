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
  uniformLocations = createUniformTable(uniformNames);
  assert(uniformLocations.size() == uniformNames.size() &&
         "Number of uniform locations does not match number of uniform names");
}
