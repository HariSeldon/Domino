#include "Engine.h"

#include <LinearMath/btVector3.h>

#include <iostream>

Engine::Engine() {
  m_broadphase = new btDbvtBroadphase();
  m_collisionConfiguration = new btDefaultCollisionConfiguration();
  m_collisionDispatcher = new btCollisionDispatcher(m_collisionConfiguration);
  m_constraintSolver = new btSequentialImpulseConstraintSolver();
  m_dynamicsWorld =
      new btDiscreteDynamicsWorld(m_collisionDispatcher, m_broadphase,
                                  m_constraintSolver, m_collisionConfiguration);
}

Engine::~Engine() {
  btCollisionObjectArray objectsArray = m_dynamicsWorld->getCollisionObjectArray();
  int objNumber = m_dynamicsWorld->getNumCollisionObjects();

  // Delete all rigid bodies removing them from the world.
  for (int index = 0; index < objNumber; ++index) {
    btCollisionObject* obj = objectsArray[index];
    m_dynamicsWorld->removeCollisionObject(obj);
    delete obj;
  }

  delete m_dynamicsWorld;
  delete m_constraintSolver;
  delete m_collisionConfiguration;
  delete m_collisionDispatcher;
  delete m_broadphase;
}

void Engine::setGravity(const btVector3& gravity) {
  m_gravity = gravity;
  m_dynamicsWorld->setGravity(gravity);
}
const btVector3& Engine::getGravity() const { return m_gravity; }

btDiscreteDynamicsWorld* Engine::getDynamicsWorld() const {
  return m_dynamicsWorld;
}

void Engine::addRigidBody(btRigidBody* rigidBody) {
  m_dynamicsWorld->addRigidBody(rigidBody);
}
