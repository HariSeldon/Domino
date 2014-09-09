#pragma once

#include <string>
#include <vector>

#include <glm/vec3.hpp>

class Mesh;

class ObjParser {
public:
  ObjParser();
  typedef std::tuple<int, int, int> FaceIndices;

public:
  void parse(const std::string &objFile);

  inline const std::vector<glm::vec3> &getPoints() const { return points; }
  inline const std::vector<glm::vec3> &getNormals() const { return normals; }
  inline const std::vector<glm::vec2> &getTextureCoos() const {
    return textureCoos;
  }
  inline const std::vector<FaceIndices> &getIndices() const { return indices; }
  inline std::string getTexFile() const { return texFile; }

private:
  void parseObj(const std::string &file); 
  void parseObjLine(const std::string &line);
  void parseNormal(const std::vector<std::string> &words);
  void parseVertex(const std::vector<std::string> &words);
  void parseTexture(const std::vector<std::string> &words);
  void parseFace(const std::vector<std::string> &words);

  void parseMtl(const std::string &file);
  void parseMtlLine(const std::string &line);

private:
  std::string objName;

  // Shape.
  std::vector<glm::vec3> points;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> textureCoos;
  std::vector<FaceIndices> indices;

  // Material.
  std::string materialName;
  std::string mtlFile;
  float specularExponent;
  glm::vec3 ambientColor;
  glm::vec3 diffuseColor;
  glm::vec3 specularColor;
  std::string texFile;
};
