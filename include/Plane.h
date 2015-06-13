#pragma once

#include "Object.h"

#include <LinearMath/btScalar.h>

class btVector3;

//-----------------------------------------------------------------------------
class Plane : public Object {
private:
  Plane(const btTransform &transform, const btScalar mass, btVector3 &inertia,
        const btScalar side, const int textureRepetitions);

private:
  void computePoints(const btScalar side);

  int textureRepetitions; 

  friend class PlaneBuilder;
};

//-----------------------------------------------------------------------------
class PlaneBuilder : public ObjectBuilder<PlaneBuilder> {
public:
  PlaneBuilder();

  inline PlaneBuilder &setSide(const btScalar side) {
    this->side = side;
    return *this;
  }
  inline PlaneBuilder &setTextureRepetitions(const int repetitions) {
    this->textureRepetitions = repetitions;
    return *this;
  }

  Plane *create();

private:
  btScalar side;
  int textureRepetitions;
};
