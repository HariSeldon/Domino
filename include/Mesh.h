#pragma once

#include "Object.h"

#include <glm/fwd.hpp>

class Drawer;
class ObjParser;

class Mesh : public Object {
private:
  Mesh(const btTransform &transform, const btScalar mass, btVector3 &inertia,
       const std::string &meshFile);
  void parseObjFile(const std::string &meshFile);
  void fillMesh(const ObjParser &objParser);
  void setupBulletShape();

  friend class MeshBuilder;
};

//------------------------------------------------------------------------------
class MeshBuilder : public ObjectBuilder<MeshBuilder> {
public:
  MeshBuilder();

  MeshBuilder &setMeshFile(const std::string &meshFile);
  Mesh *create();

private:
  std::string m_meshFile;
};
