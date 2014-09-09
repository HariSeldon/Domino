#include "Mesh.h"

#include "SysUtils.h"
#include "ObjParser.h"

#include <LinearMath/btVector3.h>

#include <glm/vec2.hpp>

#include <algorithm>
#include <iostream>
#include <map>

template class ObjectBuilder<MeshBuilder>;

Mesh::Mesh(const btTransform &transform, const btScalar mass,
           btVector3 &inertia, const std::string &meshFile)
    : Object(transform, mass, inertia) {

  ObjParser objParser;
  objParser.parse(meshFile);
  
  fillMesh(objParser);

  collisionShape =
      new btConvexHullShape(getPoints(), getPointsNumber(), sizeof(glm::vec3));
  collisionShape->calculateLocalInertia(mass, inertia);

  motionState = new btDefaultMotionState(transform);
  constructionInfo = new btRigidBody::btRigidBodyConstructionInfo(
      mass, motionState, collisionShape, inertia);
  rigidBody = new btRigidBody(*constructionInfo);
}

//------------------------------------------------------------------------------
void Mesh::fillMesh(const ObjParser &parser) {
  const auto &parserPoints = parser.getPoints();
  const auto &parserNormals = parser.getNormals();
  const auto &parserTextureCoos = parser.getTextureCoos();
  const auto &parserIndices = parser.getIndices();

  std::map<ObjParser::FaceIndices, int> indexMap;

  const int indexNumber = parserIndices.size();

  int counter = 0;

  for (int index = 0; index < indexNumber; ++index) {
    ObjParser::FaceIndices faceIndices = parserIndices[index];

    glm::vec3 vertex = parserPoints[std::get<0>(faceIndices) - 1];
    glm::vec2 textureCoo = parserTextureCoos[std::get<1>(faceIndices) - 1];
    glm::vec3 normal = parserNormals[std::get<2>(faceIndices) - 1];

    auto iterator = indexMap.find(faceIndices);
    if (iterator != indexMap.end()) {
      indices.push_back(iterator->second);
    } else {
      points.push_back(vertex);
      normals.push_back(normal);
      textureCoos.push_back(textureCoo);
      indices.push_back(counter);
      indexMap[faceIndices] = counter;
      counter++;
    }
  } 

  textureFile = parser.getTexFile();

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
