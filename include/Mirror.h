#pragma once

#include "Object.h"

#include <LinearMath/btScalar.h>

class btVector3;

class Mirror : public Object {
public:
  Mirror(btTransform &transform, btScalar mass, btVector3 &inertia,
         const btScalar &side);

private:
  void computePoints(const btScalar &side);

  friend class MirrorBuilder;
};

//-----------------------------------------------------------------------------
class MirrorBuilder : public ObjectBuilder<MirrorBuilder> {
public:
  MirrorBuilder();

  MirrorBuilder &setSide(btScalar side);

  Mirror *create();

private:
  btScalar side;
};
