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
  void setupBulletShape();

private:
  int m_textureRepetitions = 0; 

  friend class PlaneBuilder;
};

//-----------------------------------------------------------------------------
class PlaneBuilder : public ObjectBuilder<PlaneBuilder> {
public:
  PlaneBuilder();

  inline PlaneBuilder &setSide(const btScalar side) {
    m_side = side;
    return *this;
  }
  inline PlaneBuilder &setTextureRepetitions(const int repetitions) {
    m_textureRepetitions = repetitions;
    return *this;
  }

  Plane *create();

private:
  btScalar m_side = 0.0f;
  int m_textureRepetitions = 1;
};
