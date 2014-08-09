#include "Plane.h"

#include "Drawer.h"
#include "MathUtils.h"

#include <LinearMath/btVector3.h>

template class ObjectBuilder<PlaneBuilder>;

Plane::Plane(btTransform &transform, btScalar mass, btVector3 &inertia,
             const btScalar &side)
    : Object(transform, mass, inertia) {
  computePoints(side);
  collisionShape = new btStaticPlaneShape(
      btVector3(normals[0].x, normals[0].y, normals[0].z), btScalar(0));
  motionState = new btDefaultMotionState(transform);
  constructionInfo = new btRigidBody::btRigidBodyConstructionInfo(
      0, motionState, collisionShape, inertia);
  rigidBody = new btRigidBody(*constructionInfo);
}

void Plane::computePoints(const btScalar &side) {
  btScalar halfSide = side / 2;

  glm::vec3 first(-1.f * halfSide, 0.f, halfSide);
  glm::vec3 second(halfSide, 0.f, halfSide);
  glm::vec3 third(halfSide, 0.f, -1.f * halfSide);
  glm::vec3 fourth(-1.f * halfSide, 0.f, -1.f * halfSide);

  points = { first, second, third, fourth };

  indices = { 0, 1, 2, 2, 3, 0 };

  glm::vec3 normal = computeNormal(first, second, third);

  normals = { normal, normal, normal, normal }; 
}

//-----------------------------------------------------------------------------
PlaneBuilder::PlaneBuilder() : ObjectBuilder(), side(btScalar(0.0)) {}

PlaneBuilder &PlaneBuilder::setSide(btScalar side) {
  this->side = side;
  return *this;
}

Plane *PlaneBuilder::create() {
  Plane *plane = new Plane(transform, mass, inertia, side);
  ObjectBuilder::setColors(plane);
  return plane;
}
