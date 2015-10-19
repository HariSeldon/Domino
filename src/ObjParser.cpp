#include "ObjParser.h"

#include "Mesh.h"
#include "SysDefines.h"
#include "SysUtils.h"

#include <map>
#include <sstream>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

//------------------------------------------------------------------------------
inline glm::vec3 floatTriple2vec3(const std::vector<std::string> &words) {
  if (words.size() != 3) {
    std::cerr << "Wrong triple format";
    exit(1);
  }
  return glm::vec3(stof(words[0]), stof(words[1]), stof(words[2]));
}

//------------------------------------------------------------------------------
inline glm::vec2 floatCouple2vec2(const std::vector<std::string> &words) {
  if (words.size() != 2) {
    std::cerr << "Wrong triple format";
    exit(1);
  }
  return glm::vec2(stof(words[0]), stof(words[1]));
}

//------------------------------------------------------------------------------
std::vector<std::string> split(const std::string &inputString, char delim) {
  std::stringstream ss(inputString);
  std::string item;
  std::vector<std::string> elems;
  while (std::getline(ss, item, delim))
    elems.push_back(item);
  return elems;
}

//------------------------------------------------------------------------------
ObjParser::ObjParser() {}

//------------------------------------------------------------------------------
void ObjParser::parse(const std::string &meshFile) {
  parseObj(meshFile);
  parseMtl(m_mtlFile);
}

//------------------------------------------------------------------------------
void ObjParser::parseObj(const std::string &meshFile) {
  std::string file = getFileContent(meshFile.c_str());

  const auto lines = split(file, '\n');
  for (const auto &line : lines) {
    parseObjLine(line);
  }
}

//------------------------------------------------------------------------------
void ObjParser::parseObjLine(const std::string &line) {
  const auto words = split(line, ' ');

  const std::string &firstWord = words[0];

  if (firstWord == "v")
    // Replace the creation of vectors with range-based.
    parseVertex(std::vector<std::string>(words.begin() + 1, words.end()));

  if (firstWord == "f")
    parseFace(std::vector<std::string>(words.begin() + 1, words.end()));

  if (firstWord == "vn")
    parseNormal(std::vector<std::string>(words.begin() + 1, words.end()));

  if (firstWord == "vt")
    parseTexture(std::vector<std::string>(words.begin() + 1, words.end()));

  if (firstWord == "mtllib")
    m_mtlFile = words[1];

  if (firstWord == "o")
    m_objName = words[1];

  if (firstWord == "usemtl")
    m_materialName = words[1];
}

void ObjParser::parseNormal(const std::vector<std::string> &words) {
  m_normals.emplace(m_normals.end(), floatTriple2vec3(words));
}

void ObjParser::parseVertex(const std::vector<std::string> &words) {
  m_points.emplace(m_points.end(), floatTriple2vec3(words));
}

void ObjParser::parseTexture(const std::vector<std::string> &words) {
  m_textureCoos.emplace(m_textureCoos.end(), floatCouple2vec2(words));
}

void ObjParser::parseFace(const std::vector<std::string> &words) {
  // Check how many components the words have.
  for (auto word : words) {
    const auto values = split(word, '/');

    int vertexIndex = stoi(values[0]);
    int normalIndex = stoi(values[2]);
    int textureIndex = -1;
    if (!values[1].empty())
      textureIndex = stoi(values[1]);

    m_indices.emplace_back(
        std::make_tuple(vertexIndex, textureIndex, normalIndex));
  }
}

//------------------------------------------------------------------------------
void ObjParser::parseMtl(const std::string &mtlFile) {
  std::string file = getFileContent(MESH_PATH + mtlFile);

  const auto lines = split(file, '\n');
  for (auto line : lines) {
    parseMtlLine(line);
  }
}

//------------------------------------------------------------------------------
void ObjParser::parseMtlLine(const std::string &line) {
  if (line == "")
    return;

  const auto words = split(line, ' ');

  const std::string &firstWord = words[0];

  if (firstWord == "newmtl")
    m_materialName = words[1];
  if (firstWord == "Ns")
    m_specularExponent = stof(words[1]);

  if (firstWord == "Ka")
    m_ambientColor = glm::vec4(floatTriple2vec3(std::vector<std::string>(
                                   words.begin() + 1, words.end())),
                               1.f);

  if (firstWord == "Kd")
    m_diffuseColor = glm::vec4(floatTriple2vec3(std::vector<std::string>(
                                   words.begin() + 1, words.end())),
                               1.f);

  if (firstWord == "Ks")
    m_specularColor = glm::vec4(floatTriple2vec3(std::vector<std::string>(
                                    words.begin() + 1, words.end())),
                                1.f);

  if (firstWord == "map_Kd")
    m_texFile = words[1];
}
