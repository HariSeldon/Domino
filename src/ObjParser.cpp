#include "ObjParser.h"

#include "Mesh.h"
#include "SysDefines.h"
#include "SysUtils.h"

#include <map>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <boost/tokenizer.hpp>

#include <iostream>

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
ObjParser::ObjParser() : specularExponent(0.0) {}

//------------------------------------------------------------------------------
void ObjParser::parse(const std::string &meshFile) {
  parseObj(meshFile);
  parseMtl(mtlFile);
}

//------------------------------------------------------------------------------
void ObjParser::parseObj(const std::string &meshFile) {
  std::string data = getFileContent(meshFile.c_str());

  boost::char_separator<char> newLine("\n");
  boost::tokenizer<boost::char_separator<char> > tokenizer(data, newLine);
  for (auto line : tokenizer) {
    parseObjLine(line);
  }
}

//------------------------------------------------------------------------------
void ObjParser::parseObjLine(const std::string &line) {
  boost::char_separator<char> space(" ");
  boost::tokenizer<boost::char_separator<char> > tokens(line, space);

  std::vector<std::string> words;
  std::copy(tokens.begin(), tokens.end(),
            std::back_inserter<std::vector<std::string> >(words));

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
    mtlFile = words[1];

  if (firstWord == "o")
    objName = words[1];

  if (firstWord == "usemtl")
    materialName = words[1];
}

void ObjParser::parseNormal(const std::vector<std::string> &words) {
  normals.emplace(normals.end(), floatTriple2vec3(words));
}

void ObjParser::parseVertex(const std::vector<std::string> &words) {
  points.emplace(points.end(), floatTriple2vec3(words));
}

void ObjParser::parseTexture(const std::vector<std::string> &words) {
  textureCoos.emplace(textureCoos.end(), floatCouple2vec2(words));
}

void ObjParser::parseFace(const std::vector<std::string> &words) {
  // Check how many components the words have.
  boost::char_separator<char> slashSeparator("/", "",
                                             boost::keep_empty_tokens);
  for (auto word : words) {
    boost::tokenizer<boost::char_separator<char> > tokens(word, slashSeparator);
    std::vector<std::string> values;
    values.reserve(3);
    std::copy(tokens.begin(), tokens.end(),
              std::back_inserter<std::vector<std::string>>(values));

    int vertexIndex = stoi(values[0]);
    int normalIndex = stoi(values[2]);
    int textureIndex = -1;
    if (!values[1].empty())
      textureIndex = stoi(values[1]);

    indices.emplace_back(
        std::make_tuple(vertexIndex, textureIndex, normalIndex));
  }
}

//------------------------------------------------------------------------------
void ObjParser::parseMtl(const std::string &mtlFile) {
  std::string data = getFileContent(MESH_PATH + mtlFile);

  boost::char_separator<char> newLine("\n");
  boost::tokenizer<boost::char_separator<char> > tokenizer(data, newLine);
  for (auto line : tokenizer) {
    parseMtlLine(line);
  }
}

//------------------------------------------------------------------------------
void ObjParser::parseMtlLine(const std::string &line) {
  boost::char_separator<char> space(" ");
  boost::tokenizer<boost::char_separator<char> > tokens(line, space);

  std::vector<std::string> words;
  std::copy(tokens.begin(), tokens.end(),
            std::back_inserter<std::vector<std::string> >(words));

  const std::string &firstWord = words[0];

  if (firstWord == "newmtl")
    materialName = words[1];
  if (firstWord == "Ns")
    specularExponent = stof(words[1]);

  if (firstWord == "Ka")
    ambientColor = glm::vec4(floatTriple2vec3(std::vector<std::string>(
                                 words.begin() + 1, words.end())),
                             1.f);

  if (firstWord == "Kd")
    diffuseColor = glm::vec4(floatTriple2vec3(std::vector<std::string>(
                                 words.begin() + 1, words.end())),
                             1.f);

  if (firstWord == "Ks")
    specularColor = glm::vec4(floatTriple2vec3(std::vector<std::string>(
                                  words.begin() + 1, words.end())),
                              1.f);

  if (firstWord == "map_Kd")
    texFile = TEXTURE_PATH + words[1];
}
