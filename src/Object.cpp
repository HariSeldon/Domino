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
    : Entity(transform), m_mass(mass), m_inertia(inertia) {}

Object::~Object() {
  delete m_collisionShape;
  delete m_motionState;
  delete m_constructionInfo;
}

const float *Object::getPoints() const {
  return reinterpret_cast<const float *>(m_points.data());
}
int Object::getPointsNumber() const { return m_points.size(); }

const float *Object::getNormals() const {
  return reinterpret_cast<const float *>(m_normals.data());
}
const unsigned int *Object::getIndices() const { return m_indices.data(); }

const float *Object::getTextureCoos() const {
  return reinterpret_cast<const float *>(m_textureCoos.data());
}

const float *Object::getTangents() const {
  return reinterpret_cast<const float *>(m_tangents.data());
}

void Object::setMass(btScalar mass) { m_mass = mass; }

void Object::setInertia(const btVector3 &inertia) { m_inertia = inertia; }

void Object::setAmbientColor(const glm::vec4 &color) { m_ambientColor = color; }

void Object::setDiffuseColor(const glm::vec4 &color) { m_diffuseColor = color; }

void Object::setSpecularColor(const glm::vec4 &color) { m_specularColor = color; }

void Object::setShininess(float shininess) { m_shininess = shininess; }

btCollisionShape *Object::getCollisionShape() const { return m_collisionShape; }
void Object::setCollisionShape(btCollisionShape *collisionShape) {
  m_collisionShape = collisionShape;
}

void Object::setRigidBody(btRigidBody *rigidBody) {
  m_rigidBody = rigidBody;
}

btDefaultMotionState *Object::getMotionState() const { return m_motionState; }
void Object::setMotionState(btDefaultMotionState *motionState) {
  m_motionState = motionState;
}

btRigidBody::btRigidBodyConstructionInfo *Object::getConstructionInfo() const {
  return m_constructionInfo;
}
void Object::setConstructionInfo(
    btRigidBody::btRigidBodyConstructionInfo *constructionInfo) {
  m_constructionInfo = constructionInfo;
}

//-----------------------------------------------------------------------------

template <class Subtype>
ObjectBuilder<Subtype>::ObjectBuilder() {}

template <class Subtype>
ObjectBuilder<Subtype>::~ObjectBuilder() {}

template <class Subtype>
Subtype &ObjectBuilder<Subtype>::setTransform(const btTransform &transform) {
  m_transform = transform;
  return static_cast<Subtype &>(*this);
}

template <class Subtype>
Subtype &ObjectBuilder<Subtype>::setMass(btScalar mass) {
  m_mass = mass;
  return static_cast<Subtype &>(*this);
}

template <class Subtype>
Subtype &ObjectBuilder<Subtype>::setInertia(const btVector3 &inertia) {
  m_inertia = inertia;
  return static_cast<Subtype &>(*this);
}

template <class Subtype>
Subtype &ObjectBuilder<Subtype>::setAmbientColor(const glm::vec4 &color) {
  m_ambientColor = color;
  return static_cast<Subtype &>(*this);
}

template <class Subtype>
Subtype &ObjectBuilder<Subtype>::setDiffuseColor(const glm::vec4 &color) {
  m_diffuseColor = color;
  return static_cast<Subtype &>(*this);
}

template <class Subtype>
Subtype &ObjectBuilder<Subtype>::setSpecularColor(const glm::vec4 &color) {
  m_specularColor = color;
  return static_cast<Subtype &>(*this);
}

template <class Subtype>
Subtype &ObjectBuilder<Subtype>::setShininess(float shininess) {
  m_shininess = shininess;
  return static_cast<Subtype &>(*this);
}

template <class Subtype>
Subtype &ObjectBuilder<Subtype>::setTextureFile(std::string textureFile) {
  m_textureFile = textureFile;
  return static_cast<Subtype &>(*this);
}

template <class Subtype>
Subtype &
ObjectBuilder<Subtype>::setNormalTextureFile(std::string normalTextureFile) {
  m_normalTextureFile = normalTextureFile;
  return static_cast<Subtype &>(*this);
}

template <class Subtype>
void ObjectBuilder<Subtype>::setColors(Object *object) {
  object->setAmbientColor(m_ambientColor);
  object->setDiffuseColor(m_diffuseColor);
  object->setSpecularColor(m_specularColor);
  object->setShininess(m_shininess);
}

template class ObjectBuilder<PlaneBuilder>;
template class ObjectBuilder<BoxBuilder>;
template class ObjectBuilder<MirrorBuilder>;
template class ObjectBuilder<MeshBuilder>;
template class ObjectBuilder<LightBulbBuilder>;
