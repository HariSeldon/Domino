#include "Mirror.h"

#include "MathUtils.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

#include <GL/glew.h>

#include <iostream>

const std::string Mirror::MIRROR_VERTEX_SHADER = "mirror.vert";
const std::string Mirror::MIRROR_FRAGMENT_SHADER = "mirror.frag";

Mirror::Mirror(btTransform &transform, btScalar mass, btVector3 &inertia,
               const btScalar &side)
    : Object(transform, mass, inertia), fboId(0), dboId(0), mirrorTexture(0),
      shader(MIRROR_VERTEX_SHADER, MIRROR_FRAGMENT_SHADER) {
  computePoints(side);
  setupBulletShape();

  createFBO();
  createDBO();
  createMirrorTexture();
  attachTexture();
}

//-----------------------------------------------------------------------------
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

  textureCoos = { { 0.f, 1.f }, { 1.f, 1.f }, { 1.f, 0.f }, { 0.f, 0.f } };
}

//-----------------------------------------------------------------------------
void Mirror::setupBulletShape() {
  // FIXME: verify that this is correct.
  // It will matter when I enable the engine.
  collisionShape =
      new btConvexHullShape((btScalar *)points.data(), 4, sizeof(glm::vec3));

  motionState = new btDefaultMotionState(transform);
  constructionInfo = new btRigidBody::btRigidBodyConstructionInfo(
      mass, motionState, collisionShape, inertia);
  rigidBody = new btRigidBody(*constructionInfo);
}

//-----------------------------------------------------------------------------
void Mirror::createFBO() {
  glGenFramebuffers(1, &fboId);
  checkOpenGLError("Mirror: glGenFramebuffers");
}

//-----------------------------------------------------------------------------
void Mirror::createDBO() {
  glGenRenderbuffers(1, &dboId);
  checkOpenGLError("Mirror: glGenRenderbuffers");

  glBindRenderbuffer(GL_RENDERBUFFER, dboId);
  checkOpenGLError("Mirror: glBindRenderbuffer");

  // Define the size of the render buffer.
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 800);
  checkOpenGLError("Mirror: glRenderbufferStorage");

  glBindFramebuffer(GL_FRAMEBUFFER, fboId);
  checkOpenGLError("Mirror: glBindFramebuffer");
  // Attach the render buffer to the currently bound framebuffer.
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, dboId);
  checkOpenGLError("Mirror: glFramebufferRenderbuffer");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//-----------------------------------------------------------------------------
void Mirror::createMirrorTexture() {
  glGenTextures(1, &mirrorTexture);
  checkOpenGLError("Mirror: glGenTextures");

  glBindTexture(GL_TEXTURE_2D, mirrorTexture);
  checkOpenGLError("Mirror: glBindTexture");

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 800, 0, GL_RGB, GL_UNSIGNED_BYTE,
               nullptr);
  checkOpenGLError("Mirror: glTexImage2D");

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  checkOpenGLError("Mirror: glTexParameteri");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  checkOpenGLError("Mirror: glTexParameteri");

  glBindTexture(GL_TEXTURE_2D, 0);
}

//-----------------------------------------------------------------------------
void Mirror::attachTexture() {
  glBindTexture(GL_TEXTURE_2D, mirrorTexture);
  checkOpenGLError("Mirror: glBindTexture");
  glBindFramebuffer(GL_FRAMEBUFFER, fboId);
  checkOpenGLError("Mirror: glBindFramebuffer");
  glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mirrorTexture,
                       0);
  checkOpenGLError("Mirror: glFramebufferTexture");

  GLenum drawBuffers = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers(1, &drawBuffers);
  checkOpenGLError("Mirror: glDrawBuffers");

  assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE &&
         "Error setting frame buffer");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//-----------------------------------------------------------------------------
void Mirror::enableMirror() const {
  glBindFramebuffer(GL_FRAMEBUFFER, fboId);
  glViewport(0, 0, 1280, 800);
  glBindTexture(GL_TEXTURE_2D, mirrorTexture);
}

//-----------------------------------------------------------------------------
void Mirror::disableMirror() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

//-----------------------------------------------------------------------------
ShaderProgram &Mirror::getShaderProgram() { return shader; }

//-----------------------------------------------------------------------------
glm::mat4 Mirror::getModelView() const {
  // Get the mirror transformation.
  btScalar oglTransform[16];
  getOpenGLMatrix(oglTransform);
  glm::mat4 currentTransform = glm::make_mat4x4(oglTransform);

  // Get eye.
  glm::vec4 eye = currentTransform * glm::vec4(0.f, 0.f, 0.f, 1.f);

  // Compute center.
  glm::mat4 centerMatrix = currentTransform;
  centerMatrix = glm::translate(centerMatrix, glm::vec3(0.f, 0.f, 1.f));
  glm::vec4 center = centerMatrix * glm::vec4(0.f, 0.f, 0.f, 1.f);

  // The second column of the center matrix contains the up vector.
  glm::vec4 up = centerMatrix[1];
  glm::mat4 lookAtMatrix =
      glm::lookAt(glm::vec3(eye), glm::vec3(center), glm::vec3(up));

  return lookAtMatrix;
}

//-----------------------------------------------------------------------------
GLuint Mirror::getTextureId() const { return mirrorTexture; }

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
