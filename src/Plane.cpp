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
    : Object(transform, mass, inertia), m_textureRepetitions(textureRepetitions) {
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

  m_points = { first, second, third, fourth };

  m_indices = { 0, 1, 2, 2, 3, 0 };

  m_textureCoos = {{0, m_textureRepetitions},
                 {m_textureRepetitions, m_textureRepetitions},
                 {m_textureRepetitions, 0},
                 {0, 0}};

  glm::vec3 normal = computeNormal(first, second, third);

  m_normals = { normal, normal, normal, normal }; 

  m_tangents.assign(4, {1, 0, 0});
}

//-----------------------------------------------------------------------------
void Plane::setupBulletShape() {
  m_collisionShape =
      new btConvexHullShape(getPoints(), getPointsNumber(), sizeof(glm::vec3));
  m_collisionShape->calculateLocalInertia(m_mass, m_inertia);
  m_motionState = new btDefaultMotionState(m_transform);
  m_constructionInfo = new btRigidBody::btRigidBodyConstructionInfo(
      m_mass, m_motionState, m_collisionShape, m_inertia);
  m_rigidBody = new btRigidBody(*m_constructionInfo);
}

//-----------------------------------------------------------------------------
PlaneBuilder::PlaneBuilder()
    : ObjectBuilder() {}

Plane *PlaneBuilder::create() {
  Plane *plane = new Plane(m_transform, m_mass, m_inertia, m_side, m_textureRepetitions);
  ObjectBuilder::setColors(plane);
  plane->m_textureFile = m_textureFile;
  plane->m_normalTextureFile = m_normalTextureFile;
  return plane;
}
