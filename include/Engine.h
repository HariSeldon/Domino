#pragma once

#include <btBulletDynamicsCommon.h>

class Engine {
public:
  Engine();
  ~Engine();

private:
  btBroadphaseInterface *broadphase;
  btDefaultCollisionConfiguration *collisionConfiguration;
  btCollisionDispatcher *collisionDispatcher;
  btSequentialImpulseConstraintSolver *constraintSolver;
  btDiscreteDynamicsWorld *dynamicsWorld;
  btVector3 gravity;

public:
  btDiscreteDynamicsWorld *getDynamicsWorld() const;

  const btVector3 &getGravity() const;
  void setGravity(const btVector3 &gravity);

  void addRigidBody(btRigidBody *rigidBody);
};
