#include "Mesh.h"

#include "SysUtils.h"

#include <LinearMath/btVector3.h>

#include <algorithm>
#include <chrono>
#include <iostream>

template class ObjectBuilder<MeshBuilder>;

Mesh::Mesh(const btTransform &transform, const btScalar mass,
           btVector3 &inertia, const std::string &meshFile)
    : Object(transform, mass, inertia) {

  std::vector<int> times;
  auto start = std::chrono::high_resolution_clock::now();
  parseX3DFile(meshFile);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);

  collisionShape =
      new btConvexHullShape(getPoints(), getPointsNumber(), sizeof(glm::vec3));
  collisionShape->calculateLocalInertia(mass, inertia);

  motionState = new btDefaultMotionState(transform);
  constructionInfo = new btRigidBody::btRigidBodyConstructionInfo(
      mass, motionState, collisionShape, inertia);
  rigidBody = new btRigidBody(*constructionInfo);
}

//------------------------------------------------------------------------------
MeshBuilder::MeshBuilder() : ObjectBuilder() {}

MeshBuilder &MeshBuilder::setMeshFile(const std::string& meshFile) {
  this->meshFile = meshFile;
  return *this;
}

Mesh *MeshBuilder::create() {
  Mesh *mesh = new Mesh(transform, mass, inertia, meshFile);
  ObjectBuilder::setColors(mesh);
  return mesh;
}
