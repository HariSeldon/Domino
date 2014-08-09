#include "Mirror.h"

#include "MathUtils.h"

#include <iostream>

Mirror::Mirror(btTransform &transform, btScalar mass, btVector3 &inertia,
               const btScalar &side)
    : Object(transform, mass, inertia) {
  computePoints(side);

  // FIXME: verify that this is correct.
  // It will matter when I enable the engine.
  collisionShape =
      new btConvexHullShape((btScalar*)points.data(), 4, sizeof(glm::vec3));

  motionState = new btDefaultMotionState(transform);
  constructionInfo = new btRigidBody::btRigidBodyConstructionInfo(
      mass, motionState, collisionShape, inertia);
  rigidBody = new btRigidBody(*constructionInfo);
}

void Mirror::computePoints(const btScalar &side) {
  btScalar halfSide = side / 2;

  glm::vec3 first(-1.f * halfSide, halfSide, 0.f);
  glm::vec3 second(halfSide, halfSide, 0.f);
  glm::vec3 third(halfSide, -1.f * halfSide, 0.f);
  glm::vec3 fourth(-1.f * halfSide, -1.f * halfSide, 0.f);

  points = { first, second, third, fourth };

  indices = { 0, 1, 2, 2, 3, 0 };

  glm::vec3 normal = computeNormal(first, second, third);

  normals = { normal, normal, normal, normal }; 
}

//-----------------------------------------------------------------------------
MirrorBuilder::MirrorBuilder() : ObjectBuilder(), side(btScalar(0.0)) {}

MirrorBuilder &MirrorBuilder::setSide(btScalar side) {
  this->side = side;
  return *this;
}

Mirror *MirrorBuilder::create() {
  Mirror *mirror = new Mirror(transform, mass, inertia, side);
  ObjectBuilder::setColors(mirror);
  return mirror;
}
