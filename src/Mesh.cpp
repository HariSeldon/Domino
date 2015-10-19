#include "Mesh.h"

#include "ObjParser.h"
#include "SysDefines.h"
#include "SysUtils.h"

#include <LinearMath/btVector3.h>

#include <glm/vec2.hpp>
#include <glm/ext.hpp>

#include <algorithm>
#include <map>

template class ObjectBuilder<MeshBuilder>;

//------------------------------------------------------------------------------
Mesh::Mesh(const btTransform &transform, const btScalar mass,
           btVector3 &inertia, const std::string &meshFile)
    : Object(transform, mass, inertia) {
  ObjParser objParser;
  objParser.parse(meshFile);
  fillMesh(objParser);
  setupBulletShape();
}

//------------------------------------------------------------------------------
void Mesh::setupBulletShape() {
  m_collisionShape =
      new btConvexHullShape(getPoints(), getPointsNumber(), sizeof(glm::vec3));
  m_collisionShape->calculateLocalInertia(m_mass, m_inertia);
  m_motionState = new btDefaultMotionState(m_transform);
  m_constructionInfo = new btRigidBody::btRigidBodyConstructionInfo(
      m_mass, m_motionState, m_collisionShape, m_inertia);
  m_rigidBody = new btRigidBody(*m_constructionInfo);
}

//------------------------------------------------------------------------------
void Mesh::fillMesh(const ObjParser &parser) {
  const auto &parserPoints = parser.getPoints();
  const auto &parserNormals = parser.getNormals();
  const auto &parserTextureCoos = parser.getTextureCoos();
  const auto &parserIndices = parser.getIndices();

  // Mapping between a touple identifying the point/normal/texture and the
  // new index.
  std::map<ObjParser::FaceIndices, int> indexMap;

  m_indices.reserve(parserIndices.size());
  m_points.reserve(parserIndices.size() / 2);
  m_normals.reserve(parserIndices.size() / 2);
  m_textureCoos.reserve(parserIndices.size() / 2);

  int counter = 0;

  // Go over the indices in the obj file.
  for (auto &faceIndices : parserIndices) {
    // Check if a vertex with the current position/normal/texture is already present.
    auto iterator = indexMap.find(faceIndices);
    // If yes the index of the touple to the output index buffer.     
    if (iterator != indexMap.end()) {
      m_indices.push_back(iterator->second);
    } 
    // If not create a new instance.
    else {
      // Get the current point. 
      glm::vec3 vertex = parserPoints[std::get<0>(faceIndices) - 1];
      m_points.push_back(vertex);

      // If there is a texture coordinate add it.
      if(std::get<1>(faceIndices) != -1) {
        glm::vec2 textureCoo = parserTextureCoos[std::get<1>(faceIndices) - 1];
        m_textureCoos.push_back(textureCoo);
      }

      // Add normal.
      glm::vec3 normal = parserNormals[std::get<2>(faceIndices) - 1];
      m_normals.push_back(normal);

      // Create a new counter.
      m_indices.push_back(counter);
      indexMap[faceIndices] = counter;
      counter++;
    }
  }

  m_textureFile = parser.getTexFile();
  m_ambientColor = parser.getAmbientColor();
  m_diffuseColor = parser.getDiffuseColor();
  m_specularColor = parser.getSpecularColor();
  m_shininess = parser.getSpecularExponent();
}

//------------------------------------------------------------------------------
MeshBuilder::MeshBuilder() : ObjectBuilder() {}

MeshBuilder &MeshBuilder::setMeshFile(const std::string &meshFile) {
  m_meshFile = MESH_PATH + meshFile;
  return *this;
}

Mesh *MeshBuilder::create() {
  Mesh *mesh = new Mesh(m_transform, m_mass, m_inertia, m_meshFile);
  return mesh;
}
