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
               const btVector3 &sides)
    : Box(transform, mass, inertia, sides), sides{sides.x(), sides.y()} {}

//-----------------------------------------------------------------------------
glm::vec3 Mirror::computeNormal() const {
  // FIXME find a better way.
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
  return glm::vec3(glm::normalize(center - eye));
}

//-----------------------------------------------------------------------------
glm::mat4 Mirror::computeMirrorView() const {
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

MirrorBuilder &MirrorBuilder::setSides(const btVector3 &sides) {
  this->sides = sides; 
  return *this;
}

Mirror *MirrorBuilder::create() {
  Mirror *mirror = new Mirror(transform, mass, inertia, sides);
  ObjectBuilder::setColors(mirror);
  return mirror;
}
