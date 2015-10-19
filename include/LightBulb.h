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
  PositionalLight *m_light;
  float m_radius;
  static constexpr auto m_POINTS_NUMBER = 30;
  static constexpr auto m_ANGLE = 2 * M_PI / m_POINTS_NUMBER;

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
  PositionalLight *m_light;
  float m_radius;
};
