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
            btVector3 &inertia, float radius);

public:
  PositionalLight* getLight() const;

private:
  void setLight(PositionalLight *light);
  void setupPoints();
  void setupBulletShape();
 
private:
  PositionalLight *m_light = nullptr;
  float m_radius = 0.f;
  static const int m_POINTS_NUMBER = 30;
  static const float m_ANGLE;

  friend class LightBulbBuilder;
};

//-----------------------------------------------------------------------------
class LightBulbBuilder : public ObjectBuilder<LightBulbBuilder> {
public:
  LightBulbBuilder();

  LightBulbBuilder &setRadius(float radius);
  LightBulbBuilder &setLight(PositionalLight *light);
  LightBulb *create();

private:
  PositionalLight *m_light = nullptr;
  float m_radius = 0.f;
};
