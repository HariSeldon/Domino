#pragma once

#include "Object.h"

#include <glm/fwd.hpp>

class Drawer;

class Mesh : public Object {
private:
  Mesh(const btTransform &transform, const btScalar mass, btVector3 &inertia,
       const std::string &meshFile);
  void parseX3DFile(const std::string &meshFile);

  friend class MeshBuilder;
};

//------------------------------------------------------------------------------
class MeshBuilder : public ObjectBuilder<MeshBuilder> {
public:
  MeshBuilder();

  MeshBuilder &setMeshFile(const std::string &meshFile);
  Mesh *create();

private:
  std::string meshFile;
};
