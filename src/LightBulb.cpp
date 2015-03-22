#include "LightBulb.h"

#include "Light.h"
#include "MathUtils.h"
#include "ObjParser.h"
#include "SysDefines.h"

#include <LinearMath/btVector3.h>

#include <algorithm>
#include <map>

template class ObjectBuilder<LightBulbBuilder>;

LightBulb::LightBulb(const btTransform &transform, const btScalar mass,
                     btVector3 &inertia)
    : Object(transform, mass, inertia) {
  ObjParser objParser;
  objParser.parse(MESH_PATH "sphere.obj");

  fillMesh(objParser);

  collisionShape = new btSphereShape(1);
  collisionShape->calculateLocalInertia(mass, inertia);
  motionState = new btDefaultMotionState(transform);
  constructionInfo = new btRigidBody::btRigidBodyConstructionInfo(
      mass, motionState, collisionShape, inertia);
  rigidBody = new btRigidBody(*constructionInfo);
}

//------------------------------------------------------------------------------
void LightBulb::fillMesh(const ObjParser &parser) {
  const auto &parserPoints = parser.getPoints();
  const auto &parserNormals = parser.getNormals();
  const auto &parserTextureCoos = parser.getTextureCoos();
  const auto &parserIndices = parser.getIndices();

  std::map<ObjParser::FaceIndices, int> indexMap;

  const int indexNumber = parserIndices.size();

  indices.reserve(parserIndices.size());
  points.reserve(parserIndices.size() / 2);
  normals.reserve(parserIndices.size() / 2);
  textureCoos.reserve(parserIndices.size() / 2);

  int counter = 0;

  for (auto &faceIndices : parserIndices) {
    auto iterator = indexMap.find(faceIndices);
    if (iterator != indexMap.end()) {
      indices.push_back(iterator->second);
    } else {
      glm::vec3 vertex = parserPoints[std::get<0>(faceIndices) - 1];
      points.push_back(vertex);

      if(std::get<1>(faceIndices) != -1) {
        glm::vec2 textureCoo = parserTextureCoos[std::get<1>(faceIndices) - 1];
        textureCoos.push_back(textureCoo);
      }

      glm::vec3 normal = parserNormals[std::get<2>(faceIndices) - 1];
      normals.push_back(normal);

      indices.push_back(counter);
      indexMap[faceIndices] = counter;
      counter++;
    }
  }
}

//------------------------------------------------------------------------------
void LightBulb::setLight(PositionalLight *light) {
  this->light = light; 
}

PositionalLight *LightBulb::getLight() const {
  return light;
}

//------------------------------------------------------------------------------
LightBulbBuilder::LightBulbBuilder() : ObjectBuilder() {}

LightBulbBuilder& LightBulbBuilder::setLight(PositionalLight *light) {
  this->light = light;
  return *this;
}

LightBulb *LightBulbBuilder::create() {
  LightBulb *bulb = new LightBulb(transform, mass, inertia);
  bulb->setLight(light);
  bulb->textureFile = textureFile;
  return bulb;
}
