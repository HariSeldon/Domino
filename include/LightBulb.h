#pragma once

#include "Object.h"

#include <LinearMath/btScalar.h>
#include <LinearMath/btVector3.h>

#include <glm/fwd.hpp>

class LightBuilder;
class PositionalLight;
class ObjParser;

class LightBulb : public Object {
private:
  LightBulb(const btTransform &transform, const btScalar mass,
            btVector3 &inertia);

public:
  PositionalLight* getLight() const;

private:
  void setLight(PositionalLight *light);
  void fillMesh(const ObjParser &objParser);
 
private:
  PositionalLight *light;
  static constexpr auto POINTS_NUMBER = 30;
  static constexpr auto ANGLE = 2 * M_PI / POINTS_NUMBER;

  friend class LightBulbBuilder;
};

//-----------------------------------------------------------------------------
class LightBulbBuilder : public ObjectBuilder<LightBulbBuilder> {
public:
  LightBulbBuilder();

  LightBulbBuilder &setLight(PositionalLight *light);
  LightBulb *create();

private:
  PositionalLight *light;
};
