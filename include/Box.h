#pragma once

#include "Object.h"

#include <LinearMath/btScalar.h>
#include <LinearMath/btVector3.h>

#include <glm/fwd.hpp>

class Box : public Object {
protected:
  Box(const btTransform &transform, const btScalar mass, btVector3 &inertia,
      const btVector3 &sides);

protected:
  void setupFaces(const glm::vec3 &halfSides);
  void setupBulletShape(const btVector3 halfSides);

  friend class BoxBuilder;
};

//-----------------------------------------------------------------------------
class BoxBuilder : public ObjectBuilder<BoxBuilder> {
public:
  BoxBuilder();

  BoxBuilder &setSides(const btVector3 &sidesLength);

  Box *create();

private:
  btVector3 m_sidesLengths;
};
