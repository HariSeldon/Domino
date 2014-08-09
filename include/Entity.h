#pragma once

#include <LinearMath/btScalar.h>
#include <LinearMath/btTransform.h>

class Entity {
public:
  Entity(const btTransform& transform);
  virtual ~Entity() = 0;

protected:
  btTransform transform; 

public:
  void setPosition(const btVector3& position);
  const btVector3& getPosition() const;

  void setOrientation(const btQuaternion& orientation);
  const btQuaternion getOrientation() const;

  void setTransform(const btTransform& transform);
  const btTransform& getTransform() const;
};
