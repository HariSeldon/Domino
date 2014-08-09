#pragma once

#include "Engine.h"

#include <LinearMath/btVector3.h>

#include <glm/vec4.hpp>

#include <iterator>
#include <vector>

class Light;
class Object;

class World {
public:
  World();
  ~World();

private:
  glm::vec4 ambientColor;
  std::vector<Object*> objects;
  std::vector<Light*> lights;
  Engine engine;
  static const float DEFAULT_GRAVITY_X;
  static const float DEFAULT_GRAVITY_Y;
  static const float DEFAULT_GRAVITY_Z;
  static constexpr float STEPS_PER_SECOND = 70.0f;
  static constexpr int MAX_STEPS = 8;

public:
  void addObject(Object* object);
  void stepSimulation();

  const btVector3& getGravity() const;
  void setGravity(const btVector3& gravity);
  void setGravity();

  const glm::vec4 &getAmbientColor() const;
  void setAmbientColor(const glm::vec4 &color);

  int getLightsNumber() const;

private:
  void initWorld();
  void initLights();
  void initObjects();

//-----------------------------------------------------------------------------
public:
  class object_iterator :
    public std::iterator<std::forward_iterator_tag, Object*> {
  public:
    object_iterator();
    object_iterator(const World& world);
    object_iterator(const object_iterator& original);

  private:
    std::vector<Object*> objects;
    size_t currentObject;

  public:
    // Pre-increment.
    object_iterator& operator++();
    // Post-increment.
    object_iterator operator++(int);
    Object* operator*() const;
    bool operator!=(const object_iterator& iter) const;
    bool operator==(const object_iterator& iter) const;

    static object_iterator end();

  private:
    void toNext();
  };

//-----------------------------------------------------------------------------
public:
  class const_object_iterator :
    public std::iterator<std::forward_iterator_tag, const Object*> {
  public:
    const_object_iterator();
    const_object_iterator(const World& world);
    const_object_iterator(const const_object_iterator& original);

  private:
    std::vector<Object*> objects;
    size_t currentObject;

  public:
    // Pre-increment.
    const_object_iterator& operator++();
    // Post-increment.
    const_object_iterator operator++(int);
    const Object* operator*() const;
    bool operator!=(const const_object_iterator& iter) const;
    bool operator==(const const_object_iterator& iter) const;

    static const_object_iterator end();

  private:
    void toNext();
  };

//-----------------------------------------------------------------------------
public:
  class light_iterator : 
    public std::iterator<std::forward_iterator_tag, Light*> {
  public:
    light_iterator();
    light_iterator(const World& world);
    light_iterator(const light_iterator& original);

  private:
    std::vector<Light*> lights;
    size_t currentLight;

  public:
    // Pre-increment.
    light_iterator& operator++();
    // Post-increment.
    light_iterator operator++(int);
    Light* operator*() const;
    bool operator!=(const light_iterator& iter) const;
    bool operator==(const light_iterator& iter) const;

    static light_iterator end();

  private:
    void toNext();
  };

//-----------------------------------------------------------------------------
public:
  class const_light_iterator : 
    public std::iterator<std::forward_iterator_tag, const Light*> {
  public:
    const_light_iterator();
    const_light_iterator(const World& world);
    const_light_iterator(const const_light_iterator& original);

  private:
    std::vector<Light*> lights;
    size_t currentLight;

  public:
    // Pre-increment.
    const_light_iterator& operator++();
    // Post-increment.
    const_light_iterator operator++(int);
    const Light* operator*() const;
    bool operator!=(const const_light_iterator& iter) const;
    bool operator==(const const_light_iterator& iter) const;

    static const_light_iterator end();

  private:
    void toNext();
  };

};

// Non-member iterator functions.
World::object_iterator beginObjects(const World& world); 
World::object_iterator beginObjects(const World* world); 
World::object_iterator endObjects(const World& world);
World::object_iterator endObjects(const World* world);

World::const_object_iterator constBeginObjects(const World& world);
World::const_object_iterator constBeginObjects(const World* world);
World::const_object_iterator constEndObjects(const World &world);
World::const_object_iterator constEndObjects(const World* world);

World::light_iterator beginLights(const World& world); 
World::light_iterator beginLights(const World* world); 
World::light_iterator endLights(const World& world);
World::light_iterator endLights(const World* world);

World::const_light_iterator constEndLights(const World &world);
World::const_light_iterator constBeginLights(const World *world);
World::const_light_iterator constEndLights(const World &world);
World::const_light_iterator constEndLights(const World *world);
