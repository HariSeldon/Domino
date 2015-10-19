#include "Entity.h"

#include <LinearMath/btQuaternion.h>
#include <LinearMath/btScalar.h>
#include <LinearMath/btVector3.h>

Entity::Entity(const btTransform &transform) : m_transform(transform) {}

Entity::~Entity() {}

void Entity::setPosition(const btVector3 &position) {
  m_transform.setOrigin(position);
}
const btVector3 &Entity::getPosition() const { return m_transform.getOrigin(); }

void Entity::setOrientation(const btQuaternion &orientation) {
  m_transform.setRotation(orientation);
}
const btQuaternion Entity::getOrientation() const {
  return m_transform.getRotation();
}

void Entity::setTransform(const btTransform &transform) {
  m_transform = transform;
}
const btTransform &Entity::getTransform() const { return m_transform; }
