#include "LightBulb.h"

#include "Light.h"
#include "MathUtils.h"
#include "ObjParser.h"
#include "SysDefines.h"

#include <LinearMath/btVector3.h>

#include <algorithm>
#include <map>

template class ObjectBuilder<LightBulbBuilder>;

const float LightBulb::m_ANGLE = 2 * M_PI / m_POINTS_NUMBER;

LightBulb::LightBulb(const btTransform &transform, const btScalar mass,
                     btVector3 &inertia, float radius)
    : Object(transform, mass, inertia), m_radius(radius) {
  setupPoints();
  setupBulletShape();
}

//------------------------------------------------------------------------------
void LightBulb::setupPoints() {
  m_points.reserve(m_POINTS_NUMBER + 1);

  // Generate 2D data for a unit circle.
  m_points.emplace_back(0, 0, 0);

  for (int index = 0; index < m_POINTS_NUMBER + 1; ++index) {
    auto angle = index * m_ANGLE;
    auto x = m_radius * glm::cos(angle);
    auto y = m_radius * glm::sin(angle);
    m_points.emplace_back(x, y, 0);
  }

  for (auto index = 0u; index < m_POINTS_NUMBER; ++index) {
    m_indices.insert(m_indices.end(), {0, index + 1, index + 2});
  }
}

//------------------------------------------------------------------------------
void LightBulb::setupBulletShape() {
  m_collisionShape = new btSphereShape(m_radius);
  m_collisionShape->calculateLocalInertia(m_mass, m_inertia);
  m_motionState = new btDefaultMotionState(m_transform);
  m_constructionInfo = new btRigidBody::btRigidBodyConstructionInfo(
      m_mass, m_motionState, m_collisionShape, m_inertia);
  m_rigidBody = new btRigidBody(*m_constructionInfo);
}

//------------------------------------------------------------------------------
void LightBulb::setLight(PositionalLight *light) { m_light = light; }

PositionalLight *LightBulb::getLight() const { return m_light; }

//------------------------------------------------------------------------------
LightBulbBuilder::LightBulbBuilder() : ObjectBuilder() {}

LightBulbBuilder &LightBulbBuilder::setLight(PositionalLight *light) {
  m_light = light;
  return *this;
}

LightBulbBuilder &LightBulbBuilder::setRadius(float radius) {
  m_radius = radius;
  return *this;
}

LightBulb *LightBulbBuilder::create() {
  LightBulb *bulb = new LightBulb(m_transform, m_mass, m_inertia, m_radius);
  bulb->setLight(m_light);
  bulb->m_textureFile = m_textureFile;
  return bulb;
}
