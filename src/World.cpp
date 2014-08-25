#include "World.h"

#include <algorithm>

#include "Box.h"
#include "Light.h"
#include "MathUtils.h"
#include "Mesh.h"
#include "Mirror.h"
#include "Object.h"
#include "Plane.h"

#include <LinearMath/btVector3.h>

const float World::DEFAULT_GRAVITY_X = 0.0f;
const float World::DEFAULT_GRAVITY_Y = -10.0f;
const float World::DEFAULT_GRAVITY_Z = 0.0f;

// -----------------------------------------------------------------------------
World::World() : mirror(nullptr) {
  setGravity();
  initObjects();
  initLights();
  initMirror();
}

// -----------------------------------------------------------------------------
World::~World() {
  // Here I delete objects but not their rigid body.
  for (auto object : objects)
    delete object;

  for (auto light : lights)
    delete light;

  // Rigid bodies are deleted by engine.
  // In case there are troubles, convert the engine to a pointer and move its
  // desctruction before the destruction of the objects.
}

// -----------------------------------------------------------------------------
void World::addObject(Object *object) {
  objects.push_back(object);
  engine.addRigidBody(object->getRigidBody());
}

// -----------------------------------------------------------------------------
void World::stepSimulation() {
  int steps = engine.getDynamicsWorld()->stepSimulation(
      1 / World::STEPS_PER_SECOND, World::MAX_STEPS);

  if (steps == 0)
    return;

  std::for_each(begin(objects), end(objects), [](Object *object) {
    btTransform transform;
    object->getRigidBody()->getMotionState()->getWorldTransform(transform);
    object->setTransform(transform);
  });
}

// -----------------------------------------------------------------------------
const btVector3 &World::getGravity() const { return engine.getGravity(); }
void World::setGravity(const btVector3 &gravity) { engine.setGravity(gravity); }
void World::setGravity() {
  engine.setGravity({ World::DEFAULT_GRAVITY_X, World::DEFAULT_GRAVITY_Y,
                      World::DEFAULT_GRAVITY_Z });
}

// -----------------------------------------------------------------------------
const glm::vec4 &World::getAmbientColor() const { return ambientColor; }
void World::setAmbientColor(const glm::vec4 &color) { ambientColor = color; }

// -----------------------------------------------------------------------------
int World::getLightsNumber() const { return lights.size(); }

// -----------------------------------------------------------------------------
Mirror *World::getMirror() { return mirror; }

// -----------------------------------------------------------------------------
const Mirror *World::getMirror() const { return mirror; }

// -----------------------------------------------------------------------------
void traceDominoLine(const btVector3 &origin, const btVector3 &destination,
                     World *world, int full, bool tilt = false) {
  // Assume the y component of the positions is not relevant.
  // Check for vertical lines.
  btScalar zDiff = destination.z() - origin.z();
  btScalar xDiff = destination.x() - origin.x();
  btScalar slope = zDiff / xDiff;
  if (slope < 0)
    slope = -slope;
  btScalar angle = atan2(zDiff, xDiff);
  const int DOMINO_NUMBER = 7;

  btScalar length = destination.distance(origin);
  btScalar dominoDistance = length / (DOMINO_NUMBER - 1);

  BoxBuilder boxBuilder;
  if (xDiff == 0)
    slope = 1;
  btVector3 direction =
      btVector3(signum(xDiff), 0.0, signum(zDiff) * slope).normalize();
  int dominoNumber = DOMINO_NUMBER;
  int index = 1;
  if (full == 2)
    dominoNumber = dominoNumber / 2 + 1;
  if (full == 3) {
    dominoNumber = dominoNumber / 2;
  }

  for (; index < dominoNumber; ++index) {
    btScalar zAngle = 0;
    //    if(tilt && index == 1)
    //      zAngle = -M_PI_2 / 5.0;
    btQuaternion yRotation(-angle, 0.0, zAngle);

    btVector3 currentPosition = origin + dominoDistance * index * direction;
    currentPosition += btVector3(0.0, 1.0, 0.0);
    world->addObject(
        boxBuilder.setTransform(btTransform(yRotation, currentPosition))
            .setMass(btScalar(20.0))
            .setAmbientColor(glm::vec4(0.1745, 0.01175, 0.01175, 1.0))
            .setDiffuseColor(glm::vec4(0.61424, 0.04136, 0.04136, 1.0))
            .setSpecularColor(glm::vec4(0.727811, 0.626959, 0.626959, 0.6))
            .setShininess(51.2f)
            .setSides(btVector3(0.25, 2.0, 0.5))
            .create());
  }
}

// -----------------------------------------------------------------------------
void World::initObjects() {
  PlaneBuilder planeBuilder;
  // SphereBuilder sphereBuilder;
  BoxBuilder boxBuilder;
  MeshBuilder meshBuilder;
  // WedgeBuilder wedgeBuilder;

  // Plane.
  addObject(planeBuilder.setTransform(btTransform::getIdentity())
                .setMass(btScalar(0.0))
                .setAmbientColor(glm::vec4(0.0215, 0.1745, 0.0215, 1.0))
                .setDiffuseColor(glm::vec4(0.07568, 0.61424, 0.07568, 1.0))
                .setSide(btScalar(8.0))
                .create());

  btQuaternion rotation(0.0, 0.0, M_PI_2);
  addObject(
      boxBuilder.setTransform(btTransform(rotation, btVector3(0, 1.0, 0.0)))
          .setMass(btScalar(20.0))
          .setAmbientColor(glm::vec4(0.1745, 0.01175, 0.01175, 1.0))
          .setDiffuseColor(glm::vec4(0.61424, 0.04136, 0.04136, 1.0))
          .setSpecularColor(glm::vec4(0.727811, 0.626959, 0.626959, 0.6))
          .setShininess(51.2f)
          .setSides(btVector3(0.25, 2.0, 0.5))
          .create());

  addObject(
      boxBuilder.setTransform(btTransform(rotation, btVector3(0, 1.0, -3.0)))
          .setMass(btScalar(20.0))
          .setAmbientColor(glm::vec4(0.1745, 0.01175, 0.01175, 1.0))
          .setDiffuseColor(glm::vec4(0.61424, 0.04136, 0.04136, 1.0))
          .setSpecularColor(glm::vec4(0.727811, 0.626959, 0.626959, 0.6))
          .setShininess(51.2f)
          .setSides(btVector3(0.25, 2.0, 0.5))
          .create());
  //
  //  addObject(boxBuilder.setTransform(btTransform(yRotation,
  //                                                btVector3(-2, 1.0, -3.75)))
  //                .setMass(btScalar(20.0))
  //                .setAmbientColor(glm::vec4(0.1745, 0.01175, 0.01175, 1.0))
  //                .setDiffuseColor(glm::vec4(0.61424, 0.04136, 0.04136, 1.0))
  //                .setSpecularColor(glm::vec4(0.727811, 0.626959, 0.626959,
  // 0.6))
  //                .setShininess(51.2f)
  //                .setSides(btVector3(0.25, 2.0, 0.5))
  //                .create());
  //
  //  addObject(boxBuilder.setTransform(btTransform(yRotation,
  //                                                btVector3(-2, 1.0, 3.75)))
  //                .setMass(btScalar(20.0))
  //                .setAmbientColor(glm::vec4(0.1745, 0.01175, 0.01175, 1.0))
  //                .setDiffuseColor(glm::vec4(0.61424, 0.04136, 0.04136, 1.0))
  //                .setSpecularColor(glm::vec4(0.727811, 0.626959, 0.626959,
  // 0.6))
  //                .setShininess(51.2f)
  //                .setSides(btVector3(0.25, 2.0, 0.5))
  //                .create());

  //  addObject(meshBuilder.setTransform(btTransform(btQuaternion::getIdentity(),
  //                                                btVector3(20.0, 10.0, 0.0)))
  //                       .setMass(btScalar(0.0))
  //                       .setAmbientColor(glm::vec4(0.1745, 0.01175, 0.01175,
  // 1.0))
  //                       .setDiffuseColor(glm::vec4(0.61424, 0.04136, 0.04136,
  // 1.0))
  //                       .setSpecularColor(glm::vec4(0.727811, 0.626959,
  // 0.626959, 0.6))
  //                       .setShininess(51.2f)
  //                       .setObjFile("/home/hari/src/domino/obj/fighter.obj")
  //                       .create());

  //  addObject(planeBuilder.setTransform(btTransform::getIdentity())
  //                        .setMass(btScalar(0.0))
  //                        .setAmbientColor((0.0215, 0.1745, 0.0215, 1.0))
  //                        .setDiffuseColor((0.07568, 0.61424, 0.07568, 1.0))
  //                        .setSide(btScalar(500.0)).create());

  //  addObject(boxBuilder.setTransform(btTransform(btQuaternion::getIdentity(),
  //                                                btVector3(0.0, 20.0, 0.0)))
  //                .setMass(btScalar(20.0))
  //                .setAmbientColor(glm::vec4(0.19225, 0.19225, 0.19225, 1.0))
  //                .setDiffuseColor(glm::vec4(0.50754, 0.50754, 0.50754, 1.0))
  //                .setSpecularColor(glm::vec4(0.508273, 0.508273, 0.508273,
  // 1.0))
  //                .setShininess(51.2f)
  //                .setSides(btVector3(5, 40.0, 10.0))
  //                .create());

  //  // WORKING CONFIG.
  //  int SIDES = 13;
  //  btScalar RADIUS = 9;
  //  btScalar angle = 2 * M_PI / SIDES;
  //  btScalar currentAngle = 0;
  //  int halfSides = ceil(SIDES / 2.0);
  //
  //  for (int index = 0; index < halfSides; ++index) {
  //    btScalar currentStep = angle;
  //
  //    btScalar x1 = RADIUS * cos(currentAngle);
  //    btScalar z1 = RADIUS * sin(currentAngle);
  //    currentAngle += currentStep;
  //
  //    btScalar x2 = RADIUS * cos(currentAngle);
  //    btScalar z2 = RADIUS * sin(currentAngle);
  //
  //    btVector3 destination(x2, 0, z2);
  //    btVector3 origin(x1, 0, z1);
  //
  //    int full = 1;
  //    if (index == (halfSides - 1)) {
  //      full = 2;
  //    }
  //
  //    traceDominoLine(origin, destination, this, full, index == 0);
  //  }
  //
  //  currentAngle = 0;
  //  for (int index = 0; index < halfSides; ++index) {
  //    btScalar currentStep = angle;
  //
  //    btScalar x1 = RADIUS * cos(currentAngle);
  //    btScalar z1 = RADIUS * sin(currentAngle);
  //    currentAngle -= currentStep;
  //
  //    btScalar x2 = RADIUS * cos(currentAngle);
  //    btScalar z2 = RADIUS * sin(currentAngle);
  //
  //    btVector3 destination(x2, 0, z2);
  //    btVector3 origin(x1, 0, z1);
  //
  //    int full = 1;
  //    if (index == (halfSides - 1)) {
  //      full = 3;
  //    }
  //    traceDominoLine(origin, destination, this, full, index == 0);
  //
  //  }

  //  // Wedge.
  //  btQuaternion yRotation(0.0, 0.0, 0.0);
  //  addObject(wedgeBuilder.setTransform(
  //      btTransform(yRotation, btVector3(0.0, 0.0,
  // 0.0))).setMass(btScalar(0.0))
  //                .setAmbientColor(btVector4(0.1745, 0.01175, 0.01175, 1.0))
  //                .setDiffuseColor(btVector4(0.61424, 0.04136, 0.04136, 1.0))
  //                .setSpecularColor(btVector4(0.727811, 0.626959, 0.626959,
  // 0.6))
  //                .setShininess(76.8f).setSides(btVector3(20.0, 20.0, 20.0))
  //                .create());
  //
  //  // Sphere.
  //  addObject(sphereBuilder.setTransform(
  //      btTransform(btQuaternion::getIdentity(), btVector3(0.0, 30, 0.0)))
  //                .setMass(btScalar(20.0))
  //                .setAmbientColor(btVector4(0.1745, 0.01175, 0.01175, 1.0))
  //                .setDiffuseColor(btVector4(0.61424, 0.04136, 0.04136, 1.0))
  //                .setSpecularColor(btVector4(0.727811, 0.626959, 0.626959,
  // 0.6))
  //                .setShininess(76.8f).setRadius(btScalar(15.0)).create());
}

// -----------------------------------------------------------------------------
void World::initLights() {
  ambientColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
  LightBuilder lightBuilder;
  //  Light* light = lightBuilder.setPosition(glm::vec3(0.0f, 10.0f, 0.0f))
  //              .setDirection(glm::vec3(1.0f, 0.0f, 0.0f))
  //              .setAmbientColor(glm::vec4(0.9f, 0.9f, 0.9f, 1.0f))
  //              .setDiffuseColor(glm::vec4(1.f, 1.f, 1.f, 1.f))
  //              .setSpecularColor(glm::vec4(1.f, 1.f, 1.f, 1.f))
  //              .setCutOff(45.0f)
  //              .setSpotExponent(3.0f)
  ////              .setLinearAttenuation(2.0f)
  //              .createSpot();
  //  lights.push_back(light);
  //
  //
  //  Light *light1 = lightBuilder.setPosition(glm::vec3(30.0f, 10.0f, 0.0f))
  //                      .setAmbientColor(glm::vec4(0.9f, 0.9f, 0.9f, 1.0f))
  //                      .setDiffuseColor(glm::vec4(1.f, 1.f, 1.f, 1.f))
  //                      .setSpecularColor(glm::vec4(1.f, 1.f, 1.f, 1.f))
  //                      .createPositional();
  //  lights.push_back(light1);

  //  Light *light2 = lightBuilder.setPosition(glm::vec3(0.0f, 2.0f, 0.0f))
  //              .setAmbientColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f))
  //              .setDiffuseColor(glm::vec4(1.f, 1.f, 1.f, 1.f))
  //              .setSpecularColor(glm::vec4(1.f, 1.f, 1.f, 1.f))
  //              .setLinearAttenuation(0.05f)
  //              .createPositional();
  //  lights.push_back(light2);

  //  Light *light3 = lightBuilder.setPosition(glm::vec3(0.0f, 10.0f, 30.0f))
  //              .setAmbientColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f))
  //              .setDiffuseColor(glm::vec4(1.f, 1.f, 1.f, 1.f))
  //              .setSpecularColor(glm::vec4(1.f, 1.f, 1.f, 1.f))
  //              .setLinearAttenuation(1.f)
  //              .createPositional();
  //  lights.push_back(light3);
  //
  //  Light *light4 = lightBuilder.setPosition(glm::vec3(0.0f, 10.0f, -30.0f))
  //              .setAmbientColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f))
  //              .setDiffuseColor(glm::vec4(1.f, 1.f, 1.f, 1.f))
  //              .setSpecularColor(glm::vec4(1.f, 1.f, 1.f, 1.f))
  //              .setLinearAttenuation(1.f)
  //              .createPositional();
  //  lights.push_back(light4);

  Light *light = lightBuilder.setAmbientColor(glm::vec4(0.7f, 0.7f, 0.7f, 1.0f))
                     .setDiffuseColor(glm::vec4(1.f, 1.f, 1.f, 1.f))
                     .setSpecularColor(glm::vec4(1.f, 1.f, 1.f, 1.f))
                     .setDirection(glm::vec3(0.0f, -1.f, 0.0f))
                     .createDirectional();
  lights.push_back(light);
}

//-----------------------------------------------------------------------------
void World::initMirror() {
  //  MirrorBuilder mirrorBuilder;
  //  float SIDE = 4.f;
  //  btQuaternion rotation(M_PI, 0.0, 0.0);
  //  mirror =
  //      mirrorBuilder.setTransform(
  //                        btTransform(rotation, btVector3(0.f, SIDE / 2,
  // 15.f)))
  //          .setAmbientColor(glm::vec4(1.f, 1.f, 1., 1.f))
  //          .setDiffuseColor(glm::vec4(1.f, 1.f, 1., 1.f))
  //          .setSide(btScalar(SIDE))
  //          .create();
}

//-----------------------------------------------------------------------------
World::object_iterator::object_iterator() { currentObject = 0; }
World::object_iterator::object_iterator(const World &world) {
  objects = world.objects;
  currentObject = (objects.size() == 0) ? -1 : 0;
}
World::object_iterator::object_iterator(const object_iterator &original) {
  this->objects = original.objects;
  this->currentObject = original.currentObject;
}

// Pre-increment.
World::object_iterator &World::object_iterator::operator++() {
  toNext();
  return *this;
}
// Post-increment.
World::object_iterator World::object_iterator::operator++(int) {
  object_iterator old(*this);
  ++*this;
  return old;
}

Object *World::object_iterator::operator*() const {
  return objects.at(currentObject);
}
bool World::object_iterator::operator!=(const object_iterator &iter) const {
  return iter.currentObject != this->currentObject;
}

void World::object_iterator::toNext() {
  ++currentObject;
  if (currentObject == objects.size())
    currentObject = -1;
}

World::object_iterator World::object_iterator::end() {
  object_iterator endIterator;
  endIterator.currentObject = -1;
  return endIterator;
}

//-----------------------------------------------------------------------------
World::const_object_iterator::const_object_iterator() { currentObject = 0; }
World::const_object_iterator::const_object_iterator(const World &world) {
  objects = world.objects;
  currentObject = (objects.size() == 0) ? -1 : 0;
}
World::const_object_iterator::const_object_iterator(
    const const_object_iterator &original) {
  this->objects = original.objects;
  this->currentObject = original.currentObject;
}

// Pre-increment.
World::const_object_iterator &World::const_object_iterator::operator++() {
  toNext();
  return *this;
}
// Post-increment.
World::const_object_iterator World::const_object_iterator::operator++(int) {
  const_object_iterator old(*this);
  ++*this;
  return old;
}

const Object *World::const_object_iterator::operator*() const {
  return objects.at(currentObject);
}
bool World::const_object_iterator::
operator!=(const const_object_iterator &iter) const {
  return iter.currentObject != this->currentObject;
}

void World::const_object_iterator::toNext() {
  ++currentObject;
  if (currentObject == objects.size())
    currentObject = -1;
}

World::const_object_iterator World::const_object_iterator::end() {
  const_object_iterator endIterator;
  endIterator.currentObject = -1;
  return endIterator;
}

//-----------------------------------------------------------------------------
World::light_iterator::light_iterator() { currentLight = 0; }
World::light_iterator::light_iterator(const World &world) {
  lights = world.lights;
  currentLight = (lights.size() == 0) ? -1 : 0;
}
World::light_iterator::light_iterator(const light_iterator &original) {
  lights = original.lights;
  currentLight = original.currentLight;
}

// Pre-increment.
World::light_iterator &World::light_iterator::operator++() {
  toNext();
  return *this;
}
// Post-increment.
World::light_iterator World::light_iterator::operator++(int) {
  light_iterator old(*this);
  ++*this;
  return old;
}

Light *World::light_iterator::operator*() const {
  return lights.at(currentLight);
}
bool World::light_iterator::operator!=(const light_iterator &iter) const {
  return iter.currentLight != this->currentLight;
}

void World::light_iterator::toNext() {
  ++currentLight;
  if (currentLight == lights.size())
    currentLight = -1;
}

World::light_iterator World::light_iterator::end() {
  light_iterator endIterator;
  endIterator.currentLight = -1;
  return endIterator;
}

//-----------------------------------------------------------------------------
World::const_light_iterator::const_light_iterator() { currentLight = 0; }
World::const_light_iterator::const_light_iterator(const World &world) {
  lights = world.lights;
  currentLight = (lights.size() == 0) ? -1 : 0;
}
World::const_light_iterator::const_light_iterator(
    const const_light_iterator &original) {
  lights = original.lights;
  currentLight = original.currentLight;
}

// Pre-increment.
World::const_light_iterator &World::const_light_iterator::operator++() {
  toNext();
  return *this;
}
// Post-increment.
World::const_light_iterator World::const_light_iterator::operator++(int) {
  const_light_iterator old(*this);
  ++*this;
  return old;
}

const Light *World::const_light_iterator::operator*() const {
  return lights.at(currentLight);
}
bool World::const_light_iterator::
operator!=(const const_light_iterator &iter) const {
  return iter.currentLight != this->currentLight;
}

void World::const_light_iterator::toNext() {
  ++currentLight;
  if (currentLight == lights.size())
    currentLight = -1;
}

World::const_light_iterator World::const_light_iterator::end() {
  const_light_iterator endIterator;
  endIterator.currentLight = -1;
  return endIterator;
}

// -----------------------------------------------------------------------------
World::object_iterator beginObjects(const World &world) {
  return World::object_iterator(world);
}
World::object_iterator beginObjects(const World *world) {
  return beginObjects(*world);
}

World::object_iterator endObjects(const World &) {
  return World::object_iterator::end();
}
World::object_iterator endObjects(const World *world) {
  return endObjects(*world);
}

World::const_object_iterator constBeginObjects(const World &world) {
  return World::const_object_iterator(world);
}

World::const_object_iterator constBeginObjects(const World *world) {
  return constBeginObjects(*world);
}

World::const_object_iterator constEndObjects(const World &) {
  return World::const_object_iterator::end();
}

World::const_object_iterator constEndObjects(const World *world) {
  return constEndObjects(*world);
}

// -----------------------------------------------------------------------------
World::light_iterator beginLights(const World &world) {
  return World::light_iterator(world);
}
World::light_iterator beginLights(const World *world) {
  return beginLights(*world);
}

World::light_iterator endLights(const World &) {
  return World::light_iterator::end();
}
World::light_iterator endLights(const World *world) {
  return endLights(*world);
}

World::const_light_iterator constBeginLights(const World &world) {
  return World::const_light_iterator(world);
}

World::const_light_iterator constBeginLights(const World *world) {
  return constBeginLights(*world);
}

World::const_light_iterator constEndLights(const World &) {
  return World::const_light_iterator::end();
}

World::const_light_iterator constEndLights(const World *world) {
  return constEndLights(*world);
}
