#include "Object.h"

#include "Box.h"
#include "Drawer.h"
#include "LightBulb.h"
#include "MathUtils.h"
#include "Mirror.h"
#include "Mesh.h"
#include "Plane.h"
#include "ShaderProgram.h"
#include "SysDefines.h"

#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>

#include <glm/ext.hpp>

#include <utility>

const glm::vec3 Object::DEFAULT_POSITION = {0.f, 0.f, 0.f};
const glm::vec3 Object::DEFAULT_ROTATION = {0.f, 0.f, 0.f};
const glm::vec4 Object::DEFAULT_AMBIENT_COLOR = {0.f, 0.f, 0.f, 1.f};
const glm::vec4 Object::DEFAULT_DIFFUSE_COLOR = {0.f, 0.f, 0.f, 1.f};
const glm::vec4 Object::DEFAULT_SPECULAR_COLOR = {0.f, 0.f, 0.f, 1.f};

Object::Object(const btTransform &transform, btScalar mass, btVector3 &inertia)
    : Entity(transform), mass(mass), inertia(inertia) {}

Object::~Object() {
  delete collisionShape;
  delete motionState;
  delete constructionInfo;
}

const float *Object::getPoints() const {
  return reinterpret_cast<const float *>(points.data());
}
int Object::getPointsNumber() const { return points.size(); }

const float *Object::getNormals() const {
  return reinterpret_cast<const float *>(normals.data());
}
const unsigned int *Object::getIndices() const { return indices.data(); }

const float *Object::getTextureCoos() const {
  return reinterpret_cast<const float *>(textureCoos.data());
}

const float *Object::getTangents() const {
  return reinterpret_cast<const float *>(tangents.data());
}

void Object::setMass(btScalar mass) { this->mass = mass; }

void Object::setInertia(const btVector3 &inertia) { this->inertia = inertia; }

void Object::setAmbientColor(const glm::vec4 &color) { ambientColor = color; }

void Object::setDiffuseColor(const glm::vec4 &color) { diffuseColor = color; }

void Object::setSpecularColor(const glm::vec4 &color) { specularColor = color; }

void Object::setShininess(float shininess) { this->shininess = shininess; }

btCollisionShape *Object::getCollisionShape() const { return collisionShape; }
void Object::setCollisionShape(btCollisionShape *collisionShape) {
  this->collisionShape = collisionShape;
}

void Object::setRigidBody(btRigidBody *rigidBody) {
  this->rigidBody = rigidBody;
}

btDefaultMotionState *Object::getMotionState() const { return motionState; }
void Object::setMotionState(btDefaultMotionState *motionState) {
  this->motionState = motionState;
}

btRigidBody::btRigidBodyConstructionInfo *Object::getConstructionInfo() const {
  return constructionInfo;
}
void Object::setConstructionInfo(
    btRigidBody::btRigidBodyConstructionInfo *constructionInfo) {
  this->constructionInfo = constructionInfo;
}

//-----------------------------------------------------------------------------

template <class Subtype>
ObjectBuilder<Subtype>::ObjectBuilder()
    : ambientColor(glm::vec4(1.f, 1.f, 1.f, 1.f)),
      diffuseColor(glm::vec4(0.f, 0.f, 0.f, 1.f)),
      specularColor(glm::vec4(0.f, 0.f, 0.f, 1.f)), shininess(0.0f) {}

template <class Subtype>
Subtype &ObjectBuilder<Subtype>::setTransform(const btTransform &transform) {
  this->transform = transform;
  return static_cast<Subtype &>(*this);
}

template <class Subtype>
Subtype &ObjectBuilder<Subtype>::setMass(btScalar mass) {
  this->mass = mass;
  return static_cast<Subtype &>(*this);
}

template <class Subtype>
Subtype &ObjectBuilder<Subtype>::setInertia(const btVector3 &inertia) {
  this->inertia = inertia;
  return static_cast<Subtype &>(*this);
}

template <class Subtype>
Subtype &ObjectBuilder<Subtype>::setAmbientColor(const glm::vec4 &color) {
  ambientColor = color;
  return static_cast<Subtype &>(*this);
}

template <class Subtype>
Subtype &ObjectBuilder<Subtype>::setDiffuseColor(const glm::vec4 &color) {
  diffuseColor = color;
  return static_cast<Subtype &>(*this);
}

template <class Subtype>
Subtype &ObjectBuilder<Subtype>::setSpecularColor(const glm::vec4 &color) {
  specularColor = color;
  return static_cast<Subtype &>(*this);
}

template <class Subtype>
Subtype &ObjectBuilder<Subtype>::setShininess(float shininess) {
  this->shininess = shininess;
  return static_cast<Subtype &>(*this);
}

template <class Subtype>
Subtype &ObjectBuilder<Subtype>::setTextureFile(std::string textureFile) {
  this->textureFile = textureFile;
  return static_cast<Subtype &>(*this);
}

template <class Subtype>
Subtype &
ObjectBuilder<Subtype>::setNormalTextureFile(std::string normalTextureFile) {
  this->normalTextureFile = normalTextureFile;
  return static_cast<Subtype &>(*this);
}

template <class Subtype>
void ObjectBuilder<Subtype>::setColors(Object *object) {
  object->setAmbientColor(ambientColor);
  object->setDiffuseColor(diffuseColor);
  object->setSpecularColor(specularColor);
  object->setShininess(shininess);
}

template class ObjectBuilder<PlaneBuilder>;
template class ObjectBuilder<BoxBuilder>;
template class ObjectBuilder<MirrorBuilder>;
template class ObjectBuilder<MeshBuilder>;
template class ObjectBuilder<LightBulbBuilder>;
