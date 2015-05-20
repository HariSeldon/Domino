#pragma once

#include "Object.h"

#include <LinearMath/btScalar.h>
#include <LinearMath/btVector3.h>

#include <glm/fwd.hpp>

class Box : public Object {
private:
  Box(const btTransform &transform, const btScalar mass, btVector3 &inertia,
      const btVector3 &sides);

public:
  btVector3 getSides() const;
  void setSides(btVector3 sides);

private:
  void setupFaces(const glm::vec3 &halfSides);

  friend class BoxBuilder;
};

//-----------------------------------------------------------------------------
class BoxBuilder : public ObjectBuilder<BoxBuilder> {
public:
  BoxBuilder();

  BoxBuilder &setSides(const btVector3 &sidesLength);

  Box *create();

private:
  btVector3 sidesLengths;
};
