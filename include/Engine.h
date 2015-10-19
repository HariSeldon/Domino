#pragma once

#include <btBulletDynamicsCommon.h>

class Engine {
public:
  Engine();
  ~Engine();

private:
  btBroadphaseInterface *m_broadphase = nullptr;
  btDefaultCollisionConfiguration *m_collisionConfiguration = nullptr;
  btCollisionDispatcher *m_collisionDispatcher = nullptr;
  btSequentialImpulseConstraintSolver *m_constraintSolver = nullptr;
  btDiscreteDynamicsWorld *m_dynamicsWorld = nullptr;
  btVector3 m_gravity;

public:
  btDiscreteDynamicsWorld *getDynamicsWorld() const;

  const btVector3 &getGravity() const;
  void setGravity(const btVector3 &gravity);

  void addRigidBody(btRigidBody *rigidBody);
};
