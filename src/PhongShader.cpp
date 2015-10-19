#include "PhongShader.h"

#include <cassert>
#include <string>

std::vector<std::string> PhongShader::uniformNames{
    "mvpMatrix", "modelViewMatrix", "normalMatrix", "ambientColor",
    "lightsNumber", "lightMask", "texture", "material.ambient",
    "material.specular", "material.shininess",
};

// -----------------------------------------------------------------------------
PhongShader::PhongShader(const std::string &vertexShaderFileName,
                         const std::string &fragmentShaderFileName)
    : LightedObjectShader(vertexShaderFileName, fragmentShaderFileName) {
  m_uniformLocations = createUniformTable(uniformNames);
  assert(m_uniformLocations.size() == uniformNames.size() &&
         "Number of uniform locations does not match number of uniform names");
}
