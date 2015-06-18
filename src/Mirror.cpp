#include "Mirror.h"

#include "MathUtils.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

#include <GL/glew.h>

#include <glm/vec2.hpp>

#include <iostream>

Mirror::Mirror(btTransform &transform, btScalar mass, btVector3 &inertia,
               const glm::vec2 &sides)
    : Object(transform, mass, inertia), sides(sides) {
  computePoints(sides);
  setupBulletShape();
}

//-----------------------------------------------------------------------------
void Mirror::computePoints(const glm::vec2 &sides) {
  const glm::vec2 halfSides = sides / 2.f;

  glm::vec3 first(-1.f * halfSides.x, halfSides.y, 0.f);
  glm::vec3 second(halfSides.x, halfSides.y, 0.f);
  glm::vec3 third(halfSides.x, -1.f * halfSides.y, 0.f);
  glm::vec3 fourth(-1.f * halfSides.x, -1.f * halfSides.y, 0.f);

  points = { first, second, third, fourth };

  indices = { 0, 1, 2, 2, 3, 0 };

  glm::vec3 normal = computeNormal(first, second, third);
}

//-----------------------------------------------------------------------------
void Mirror::setupBulletShape() {
  collisionShape =
      new btConvexHullShape(getPoints(), getPointsNumber(), sizeof(glm::vec3));
  motionState = new btDefaultMotionState(transform);
  constructionInfo = new btRigidBody::btRigidBodyConstructionInfo(
      mass, motionState, collisionShape, inertia);
  rigidBody = new btRigidBody(*constructionInfo);
}

//-----------------------------------------------------------------------------
glm::mat4 Mirror::getModelView() const {
  // Get the mirror transformation.
  btScalar oglTransform[16];
  getOpenGLMatrix(oglTransform);
  glm::mat4 currentTransform = glm::make_mat4x4(oglTransform);
  // Get eye.
  glm::vec4 eye = currentTransform[3];
  // Compute center.
  glm::mat4 centerMatrix =
      glm::translate(currentTransform, glm::vec3(0.f, 0.f, 1.f));
  glm::vec4 center = centerMatrix[3];
  // The second column of the center matrix contains the up vector.
  glm::vec4 up = centerMatrix[1];
  return glm::lookAt(glm::vec3(eye), glm::vec3(center), glm::vec3(up));
}

//-----------------------------------------------------------------------------
MirrorBuilder::MirrorBuilder() : ObjectBuilder() {}

MirrorBuilder &MirrorBuilder::setSides(const glm::vec2 &sides) {
  this->sides = sides;
  return *this;
}

Mirror *MirrorBuilder::create() {
  Mirror *mirror = new Mirror(transform, mass, inertia, sides);
  ObjectBuilder::setColors(mirror);
  return mirror;
}
