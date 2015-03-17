#pragma once

#include "Object.h"

#include <LinearMath/btScalar.h>
#include <LinearMath/btVector3.h>

#include <glm/fwd.hpp>

class LightBuilder;
class PositionalLight;

class LightBulb : public Object {
private:
  LightBulb(const btTransform &transform, const btScalar mass,
            btVector3 &inertia, const btScalar side);

public:
  btScalar getSide() const;
  void setSide(btScalar side);

  PositionalLight* getLight() const;

private:
  void setupFaces(btScalar side);
  void setLight(PositionalLight *light);
 
private:
  PositionalLight *light;

  friend class LightBulbBuilder;
};

//-----------------------------------------------------------------------------
class LightBulbBuilder : public ObjectBuilder<LightBulbBuilder> {
public:
  LightBulbBuilder();

  LightBulbBuilder &setSide(btScalar side);
  LightBulbBuilder &setLight(PositionalLight *light);
  LightBulb *create();

private:
  btScalar side;
  PositionalLight *light;
};
