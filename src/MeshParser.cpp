#include "Mesh.h"

#include "ext/pugixml.hpp"

#include <algorithm> 
#include <iostream>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

// ----------------------------------------------------------------------------
int countSpaceNumber(const std::string &input) {
  return std::count(input.begin(), input.end(), ' ');
}

// ----------------------------------------------------------------------------
glm::vec3 string2vec3(std::string &&input) {
  size_t floatEnd = 0;
  float r = std::stof(input, &floatEnd);
  input = input.substr(floatEnd);
  float g = std::stof(input, &floatEnd);
  input = input.substr(floatEnd);
  float b = std::stof(input, &floatEnd);
  return glm::vec3(r, g, b);
}

//------------------------------------------------------------------------------
template <typename type> std::vector<type> string2vector(std::string &&input);

//------------------------------------------------------------------------------
template <>
std::vector<glm::vec3> string2vector(std::string &&input) {
  size_t floatNumber = countSpaceNumber(input);
  std::vector<glm::vec3> result;
  result.reserve(floatNumber / 3);
  size_t floatEnd = 0;
  while(floatEnd < input.size()) {
    float x = std::stof(input, &floatEnd);
    input = input.substr(floatEnd);
    float y = std::stof(input, &floatEnd);
    input = input.substr(floatEnd);
    float z = std::stof(input, &floatEnd);
    input = input.substr(floatEnd);
    result.emplace(result.end(), x, y, z);
  }
  return result;
}

//------------------------------------------------------------------------------
template <>
std::vector<glm::vec2> string2vector(std::string &&input) {
  size_t floatNumber = countSpaceNumber(input);
  std::vector<glm::vec2> result;
  result.reserve(floatNumber / 2);
  size_t floatEnd = 0;
  while(floatEnd < input.size()) {
    float x = std::stof(input, &floatEnd);
    input = input.substr(floatEnd);
    float y = std::stof(input, &floatEnd);
    input = input.substr(floatEnd);
    result.emplace(result.end(), x, y);
  }
  return result;
}

//------------------------------------------------------------------------------
template <>
std::vector<unsigned int> string2vector(std::string &&input) {
  size_t intNumber = countSpaceNumber(input);
  std::vector<unsigned int> result;
  result.reserve(intNumber);
  size_t intEnd = 0;
  while(intEnd < input.size()) {
    float x = std::stoi(input, &intEnd);
    input = input.substr(intEnd);
    result.emplace(result.end(), x);
  }
  return result;
}

//------------------------------------------------------------------------------
void Mesh::parseX3DFile(const std::string &meshFile) {
  pugi::xml_document doc;

  pugi::xml_parse_result result = doc.load_file(meshFile.c_str());
  if(!result) {
    std::cout << "Error parsing the x3d file!\n";
    exit(1);
  }

  pugi::xml_node scene = doc.child("X3D").child("Scene");

  for (pugi::xml_node trans = scene.child("Transform"); trans; trans = trans.next_sibling("Transform")) {
    pugi::xml_node innerTrans = trans.child("Transform");
    if(innerTrans == 0)
      continue;

    pugi::xml_node shape = innerTrans.child("Group").child("Shape");
    pugi::xml_node appearance = shape.child("Appearance");
    pugi::xml_node material = appearance.child("Material");

    textureFile = appearance.child("ImageTexture").attribute("url").value();

    pugi::xml_node triangles = shape.child("IndexedTriangleSet");
    pugi::xml_node coordinate = triangles.child("Coordinate");
    pugi::xml_node normal = triangles.child("Normal");
    pugi::xml_node textureCoo = triangles.child("TextureCoordinate");
    indices = string2vector<unsigned int>(triangles.attribute("index").value());
    points = string2vector<glm::vec3>(coordinate.attribute("point").value());
    normals = string2vector<glm::vec3>(normal.attribute("vector").value());
    textureCoos = string2vector<glm::vec2>(textureCoo.attribute("point").value());
  }
}
