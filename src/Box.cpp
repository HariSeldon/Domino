#include "Box.h"

#include "MathUtils.h"

#include <LinearMath/btVector3.h>

#include <glm/mat4x4.hpp>

#include <algorithm>

template class ObjectBuilder<BoxBuilder>;

Box::Box(const btTransform &transform, const btScalar mass, btVector3 &inertia,
         const btVector3 &sidesLengths)
    : Object(transform, mass, inertia) {
  const btVector3 tmpHalfSides = sidesLengths / 2;

  collisionShape = new btBoxShape(tmpHalfSides);
  collisionShape->calculateLocalInertia(mass, inertia);
  motionState = new btDefaultMotionState(transform);
  constructionInfo = new btRigidBody::btRigidBodyConstructionInfo(
      mass, motionState, collisionShape, inertia);
  rigidBody = new btRigidBody(*constructionInfo);
  setupFaces({ tmpHalfSides.x(), tmpHalfSides.y(), tmpHalfSides.z() });
}

void Box::setupFaces(const glm::vec3 &halfSides) {
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

  indices = { 0,  1,  2,  2,  3,  0,  // Front.
              4,  5,  6,  6,  7,  4,  // Right.
              8,  9,  10, 10, 11, 8,  // Back.
              12, 13, 14, 14, 15, 12, // Left.
              16, 17, 18, 18, 19, 16, // Top.
              20, 21, 22, 22, 23, 20  // Bottom.
  };

  glm::vec3 frontNormal =
      computeNormal(frontTopRight, frontBottomRight, frontBottomLeft);
  glm::vec3 rightNormal =
      computeNormal(backTopRight, backBottomRight, frontBottomRight);
  glm::vec3 backNormal =
      computeNormal(backTopLeft, backBottomLeft, backBottomRight);
  glm::vec3 leftNormal =
      computeNormal(frontTopLeft, frontBottomLeft, backBottomLeft);
  glm::vec3 topNormal =
      computeNormal(backTopRight, frontTopRight, frontTopLeft);
  glm::vec3 bottomNormal =
      computeNormal(backBottomLeft, frontBottomLeft, frontBottomRight);

  normals = { frontNormal,  frontNormal,  frontNormal,  frontNormal,
              rightNormal,  rightNormal,  rightNormal,  rightNormal,
              backNormal,   backNormal,   backNormal,   backNormal,
              leftNormal,   leftNormal,   leftNormal,   leftNormal,
              topNormal,    topNormal,    topNormal,    topNormal,
              bottomNormal, bottomNormal, bottomNormal, bottomNormal };
}

//------------------------------------------------------------------------------
BoxBuilder::BoxBuilder() : ObjectBuilder() {}

BoxBuilder &BoxBuilder::setSides(const btVector3 &sidesLengths) {
  this->sidesLengths = sidesLengths;
  return *this;
}

Box *BoxBuilder::create() {
  Box *box = new Box(transform, mass, inertia, sidesLengths);
  ObjectBuilder::setColors(box);
  return box;
}
