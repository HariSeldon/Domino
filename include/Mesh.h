#pragma once

#include "Object.h"

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

#include <tuple>
#include <vector>

class Drawer;

class Mesh : public Object {
private:
  Mesh(btTransform &transform, btScalar mass, btVector3 &inertia);

  friend class MeshBuilder;
};

typedef std::tuple<int, int, int> FaceIndices;

//------------------------------------------------------------------------------
class MeshBuilder : public ObjectBuilder<MeshBuilder> {
  typedef std::tuple<int, int, int> FaceIndices;

public:
  MeshBuilder();

  MeshBuilder &setObjFile(const std::string &filePath);
  Mesh *create();

private:
  void parseObjFile();
  void parseObjLine(const std::string &line);
  void parseNormal(const std::vector<std::string> &words);
  void parseVertex(const std::vector<std::string> &words);
  void parseFace(const std::vector<std::string> &words);
  void fillMesh(Mesh *mesh);

private:
  std::vector<glm::vec3> points;
  std::vector<glm::vec3> normals;
  std::vector<FaceIndices> indices;
  std::string objName;
  std::string materialName;
  std::string materialFile;
  std::string objFile;
};
