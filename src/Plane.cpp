#include "Plane.h"

#include "Drawer.h"
#include "MathUtils.h"

#include <glm/ext.hpp>
#include <iostream>

#include <LinearMath/btVector3.h>

template class ObjectBuilder<PlaneBuilder>;

//-----------------------------------------------------------------------------
Plane::Plane(const btTransform &transform, const btScalar mass, btVector3 &inertia,
             const btScalar side, const int textureRepetitions)
    : Object(transform, mass, inertia), textureRepetitions(textureRepetitions) {
  computePoints(side);
  setupBulletShape();
}

//-----------------------------------------------------------------------------
void Plane::computePoints(const btScalar side) {
  const btScalar halfSide = side / 2;

  glm::vec3 first(-1.f * halfSide, 0.f, halfSide);
  glm::vec3 second(halfSide, 0.f, halfSide);
  glm::vec3 third(halfSide, 0.f, -1.f * halfSide);
  glm::vec3 fourth(-1.f * halfSide, 0.f, -1.f * halfSide);

  points = { first, second, third, fourth };

  indices = { 0, 1, 2, 2, 3, 0 };

  textureCoos = {{0, textureRepetitions},
                 {textureRepetitions, textureRepetitions},
                 {textureRepetitions, 0},
                 {0, 0}};

  glm::vec3 normal = computeNormal(first, second, third);

  normals = { normal, normal, normal, normal }; 

  tangents.assign(4, {1, 0, 0});
}

//-----------------------------------------------------------------------------
void Plane::setupBulletShape() {
  collisionShape =
      new btConvexHullShape(getPoints(), getPointsNumber(), sizeof(glm::vec3));
//  collisionShape = new btStaticPlaneShape(
//      btVector3(normals[0].x, normals[0].y, normals[0].z), btScalar(0));
  collisionShape->calculateLocalInertia(mass, inertia);
  motionState = new btDefaultMotionState(transform);
  constructionInfo = new btRigidBody::btRigidBodyConstructionInfo(
      mass, motionState, collisionShape, inertia);
  rigidBody = new btRigidBody(*constructionInfo);
}

//-----------------------------------------------------------------------------
PlaneBuilder::PlaneBuilder()
    : ObjectBuilder(), side(btScalar(0.0f)), textureRepetitions(1) {}

Plane *PlaneBuilder::create() {
  Plane *plane = new Plane(transform, mass, inertia, side, textureRepetitions);
  ObjectBuilder::setColors(plane);
  plane->textureFile = textureFile;
  plane->normalTextureFile = normalTextureFile;
  return plane;
}
