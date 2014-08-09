#include "Engine.h"

#include <LinearMath/btVector3.h>

#include <iostream>

Engine::Engine() {
  broadphase = new btDbvtBroadphase();
  collisionConfiguration = new btDefaultCollisionConfiguration();
  collisionDispatcher = new btCollisionDispatcher(collisionConfiguration);
  constraintSolver = new btSequentialImpulseConstraintSolver();
  dynamicsWorld =
      new btDiscreteDynamicsWorld(collisionDispatcher, broadphase,
                                  constraintSolver, collisionConfiguration);
}

Engine::~Engine() {
  btCollisionObjectArray objectsArray = dynamicsWorld->getCollisionObjectArray();
  int objNumber = dynamicsWorld->getNumCollisionObjects();

  // Delete all rigid bodies removing them from the world.
  for (int index = 0; index < objNumber; ++index) {
    btCollisionObject* obj = objectsArray[index];
    dynamicsWorld->removeCollisionObject(obj);
    delete obj;
  }

  delete dynamicsWorld;
  delete constraintSolver;
  delete collisionConfiguration;
  delete collisionDispatcher;
  delete broadphase;
}

void Engine::setGravity(const btVector3& gravity) {
  this->gravity = gravity;
  dynamicsWorld->setGravity(gravity);
}
const btVector3& Engine::getGravity() const { return gravity; }

btDiscreteDynamicsWorld* Engine::getDynamicsWorld() const {
  return dynamicsWorld;
}

void Engine::addRigidBody(btRigidBody* rigidBody) {
  dynamicsWorld->addRigidBody(rigidBody);
}
