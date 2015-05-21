#include "LightBulbShader.h"

#include <cassert>
#include <iostream>
#include <string>

std::vector<std::string> LightBulbShader::uniformNames{"mvpMatrix", "lightMask", "lightIndex"};

// -----------------------------------------------------------------------------
LightBulbShader::LightBulbShader(const std::string &vertexShaderFileName,
                                 const std::string &fragmentShaderFileName)
    : ShaderProgram(vertexShaderFileName, fragmentShaderFileName) {
  uniformLocations = createUniformTable(uniformNames);
  assert(uniformLocations.size() == uniformNames.size() &&
          "Number of uniform locations does not match number of uniform names");
}
