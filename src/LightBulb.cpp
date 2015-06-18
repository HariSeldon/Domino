#include "LightBulb.h"

#include "Light.h"
#include "MathUtils.h"
#include "ObjParser.h"
#include "SysDefines.h"

#include <LinearMath/btVector3.h>

#include <algorithm>
#include <map>

template class ObjectBuilder<LightBulbBuilder>;

LightBulb::LightBulb(const btTransform &transform, const btScalar mass,
                     btVector3 &inertia, float radius)
    : Object(transform, mass, inertia), radius(radius) {
  setupPoints();
  setupBulletShape();
}

//------------------------------------------------------------------------------
void LightBulb::setupPoints() {
  points.reserve(POINTS_NUMBER + 1);

  // Generate 2D data for a unit circle.
  points.emplace_back(0, 0, 0);

  for (int index = 0; index < POINTS_NUMBER + 1; ++index) {
    auto angle = index * ANGLE;
    auto x = radius * glm::cos(angle);
    auto y = radius * glm::sin(angle);
    points.emplace_back(x, y, 0);
  }

  for (auto index = 0u; index < POINTS_NUMBER; ++index) {
    indices.insert(indices.end(), {0, index + 1, index + 2});
  }
}

//------------------------------------------------------------------------------
void LightBulb::setupBulletShape() {
  collisionShape = new btSphereShape(radius);
  collisionShape->calculateLocalInertia(mass, inertia);
  motionState = new btDefaultMotionState(transform);
  constructionInfo = new btRigidBody::btRigidBodyConstructionInfo(
      mass, motionState, collisionShape, inertia);
  rigidBody = new btRigidBody(*constructionInfo);
}

//------------------------------------------------------------------------------
void LightBulb::setLight(PositionalLight *light) { this->light = light; }

PositionalLight *LightBulb::getLight() const { return light; }

//------------------------------------------------------------------------------
LightBulbBuilder::LightBulbBuilder() : ObjectBuilder() {}

LightBulbBuilder &LightBulbBuilder::setLight(PositionalLight *light) {
  this->light = light;
  return *this;
}

LightBulbBuilder &LightBulbBuilder::setRadius(float radius) {
  this->radius = radius;
  return *this;
}

LightBulb *LightBulbBuilder::create() {
  LightBulb *bulb = new LightBulb(transform, mass, inertia, radius);
  bulb->setLight(light);
  bulb->textureFile = textureFile;
  return bulb;
}
