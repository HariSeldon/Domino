#include "Entity.h"

#include<LinearMath/btQuaternion.h>
#include<LinearMath/btScalar.h>
#include<LinearMath/btVector3.h>

Entity::Entity(const btTransform& transform) :
               transform(transform) { }

Entity::~Entity() { }

void Entity::setPosition(const btVector3& position) {
  transform.setOrigin(position);
}
const btVector3& Entity::getPosition() const {
  return transform.getOrigin(); 
}

void Entity::setOrientation(const btQuaternion& orientation) {
  transform.setRotation(orientation);
}
const btQuaternion Entity::getOrientation() const {
  return transform.getRotation();
}

void Entity::setTransform(const btTransform& transform) {
  this->transform = transform;
}
const btTransform& Entity::getTransform() const {
  return transform;
}
