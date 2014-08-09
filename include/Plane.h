#pragma once

#include "Object.h"

#include <LinearMath/btScalar.h>

class btVector3;

class Plane : public Object {
private:
  Plane(btTransform &transform, btScalar mass, btVector3 &inertia,
        const btScalar &side);

private:
  void computePoints(const btScalar &side);

  friend class PlaneBuilder;
};

//-----------------------------------------------------------------------------
class PlaneBuilder : public ObjectBuilder<PlaneBuilder> {
public:
  PlaneBuilder();

  PlaneBuilder &setSide(btScalar side);

  Plane *create();

private:
  btScalar side;
};
