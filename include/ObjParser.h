#pragma once

#include <string>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class Mesh;

class ObjParser {
public:
  ObjParser();
  typedef std::tuple<int, int, int> FaceIndices;

public:
  void parse(const std::string &objFile);

  inline const std::vector<glm::vec3> &getPoints() const { return m_points; }
  inline const std::vector<glm::vec3> &getNormals() const { return m_normals; }
  inline const std::vector<glm::vec2> &getTextureCoos() const {
    return m_textureCoos;
  }
  inline const std::vector<FaceIndices> &getIndices() const { return m_indices; }
  inline const glm::vec4 getAmbientColor() const { return m_ambientColor; }
  inline const glm::vec4 getDiffuseColor() const { return m_diffuseColor; }
  inline const glm::vec4 getSpecularColor() const { return m_specularColor; }
  inline float getSpecularExponent() const { return m_specularExponent; }

  inline std::string getTexFile() const { return m_texFile; }

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
  std::string m_objName;

  // Shape.
  std::vector<glm::vec3> m_points;
  std::vector<glm::vec3> m_normals;
  std::vector<glm::vec2> m_textureCoos;
  std::vector<glm::vec3> m_tangents;
  std::vector<FaceIndices> m_indices;

  // Material.
  std::string m_materialName;
  std::string m_mtlFile;
  float m_specularExponent = 0.f;
  glm::vec4 m_ambientColor;
  glm::vec4 m_diffuseColor;
  glm::vec4 m_specularColor;
  std::string m_texFile;
};
