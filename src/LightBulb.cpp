#include "LightBulb.h"

#include "Light.h"
#include "MathUtils.h"

#include <LinearMath/btVector3.h>

#include <glm/mat4x4.hpp>

#include <algorithm>

#include <iostream>
#include <glm/ext.hpp>

template class ObjectBuilder<LightBulbBuilder>;

LightBulb::LightBulb(const btTransform &transform, const btScalar mass,
                     btVector3 &inertia, const btScalar side)
    : Object(transform, mass, inertia) {
  const btScalar halfSide = side / 2.f;
  collisionShape = new btBoxShape(btVector3(halfSide, halfSide, halfSide));
  collisionShape->calculateLocalInertia(mass, inertia);
  motionState = new btDefaultMotionState(transform);
  constructionInfo = new btRigidBody::btRigidBodyConstructionInfo(
      mass, motionState, collisionShape, inertia);
  rigidBody = new btRigidBody(*constructionInfo);
  setupFaces(halfSide);
}

void LightBulb::setupFaces(const btScalar halfSide) {
  glm::vec3 halfSides = glm::vec3(halfSide, halfSide, halfSide);
  glm::vec3 frontTopLeft = glm::vec3(-1, 1, -1) * halfSides;
  glm::vec3 frontTopRight = glm::vec3(1, 1, -1) * halfSides;
  glm::vec3 frontBottomLeft = glm::vec3(-1, -1, -1) * halfSides;
  glm::vec3 frontBottomRight = glm::vec3(1, -1, -1) * halfSides;
  glm::vec3 backTopLeft = glm::vec3(-1, 1, 1) * halfSides;
  glm::vec3 backTopRight = glm::vec3(1, 1, 1) * halfSides;
  glm::vec3 backBottomLeft = glm::vec3(-1, -1, 1) * halfSides;
  glm::vec3 backBottomRight = glm::vec3(1, -1, 1) * halfSides;

  points = { frontTopRight,    frontBottomRight,
             frontBottomLeft,  frontTopLeft, // Front.
             backTopRight,     backBottomRight,
             frontBottomRight, frontTopRight, // Right.
             backTopLeft,      backBottomLeft,
             backBottomRight,  backTopRight, // Back.
             frontTopLeft,     frontBottomLeft,
             backBottomLeft,   backTopLeft, // Left.
             backTopRight,     frontTopRight,
             frontTopLeft,     backTopLeft, // Top.
             backBottomLeft,   frontBottomLeft,
             frontBottomRight, backBottomRight // Bottom.
  };

  textureCoos = { {1, 1}, {1, 0}, {0, 0}, {0, 1},
                  {1, 1}, {1, 0}, {0, 0}, {0, 1},
                  {1, 1}, {1, 0}, {0, 0}, {0, 1},
                  {1, 1}, {1, 0}, {0, 0}, {0, 1},
                  {1, 1}, {1, 0}, {0, 0}, {0, 1},
                  {1, 1}, {1, 0}, {0, 0}, {0, 1}
  };

  indices = { 0,  1,  2,  2,  3,  0,  // Front.
              4,  5,  6,  6,  7,  4,  // Right.
              8,  9,  10, 10, 11, 8,  // Back.
              12, 13, 14, 14, 15, 12, // Left.
              16, 17, 18, 18, 19, 16, // Top.
              20, 21, 22, 22, 23, 20  // Bottom.
  };

  glm::vec3 frontNormal =
      glm::vec3(-1, -1, -1) * computeNormal(frontTopRight, frontBottomRight, frontBottomLeft);
  glm::vec3 rightNormal =
      glm::vec3(-1, -1, -1) * computeNormal(backTopRight, backBottomRight, frontBottomRight);
  glm::vec3 backNormal =
      glm::vec3(-1, -1, -1) * computeNormal(backTopLeft, backBottomLeft, backBottomRight);
  glm::vec3 leftNormal =
      glm::vec3(-1, -1, -1) * computeNormal(frontTopLeft, frontBottomLeft, backBottomLeft);
  glm::vec3 topNormal =
      glm::vec3(-1, -1, -1) * computeNormal(backTopRight, frontTopRight, frontTopLeft);
  glm::vec3 bottomNormal =
      glm::vec3(-1, -1, -1) * computeNormal(backBottomLeft, frontBottomLeft, frontBottomRight);

  normals = { frontNormal,  frontNormal,  frontNormal,  frontNormal,
              rightNormal,  rightNormal,  rightNormal,  rightNormal,
              backNormal,   backNormal,   backNormal,   backNormal,
              leftNormal,   leftNormal,   leftNormal,   leftNormal,
              topNormal,    topNormal,    topNormal,    topNormal,
              bottomNormal, bottomNormal, bottomNormal, bottomNormal };
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

LightBulbBuilder &LightBulbBuilder::setSide(btScalar side) {
  this->side = side;
  return *this;
}

LightBulbBuilder& LightBulbBuilder::setLight(PositionalLight *light) {
  this->light = light;
  return *this;
}

LightBulb *LightBulbBuilder::create() {
  LightBulb *bulb = new LightBulb(transform, mass, inertia, side);
  bulb->setLight(light);
  bulb->textureFile = textureFile;
  return bulb;
}
