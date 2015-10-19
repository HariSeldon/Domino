#include "TextShader.h"

#include <cassert>
#include <string>

std::vector<std::string> TextShader::uniformNames = {"color", "texture"};

TextShader::TextShader(const std::string &vertexShaderFileName,
                       const std::string &fragmentShaderFileName)
    : ShaderProgram(vertexShaderFileName, fragmentShaderFileName) {
  m_uniformLocations = createUniformTable(uniformNames);
  assert(m_uniformLocations.size() == uniformNames.size() &&
         "Number of uniform locations does not match number of uniform names");
}
