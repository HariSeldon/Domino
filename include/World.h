#pragma once

#include "Engine.h"

#include <LinearMath/btVector3.h>

#include <glm/vec4.hpp>

#include <iterator>
#include <vector>

class Light;
class LightBulb;
class PositionalLight;
class DirectionalLight;
class Mirror;
class Object;

class World {
public:
  World();
  ~World();

private:
  glm::vec4 m_ambientColor {0.f, 0.f, 0.f, 1.f};
  std::vector<Object*> m_objects;
  std::vector<Light*> m_lights;
  std::vector<LightBulb*> m_bulbs;
  Mirror* m_mirror = nullptr;
  Engine m_engine;
  static const float STEPS_PER_SECOND;
  static const int MAX_STEPS = 8;

public:
  void addObject(Object *object);
  void addLightBulb(LightBulb *lightBulb);
  void addDirectionalLight(DirectionalLight *light);
  void stepSimulation();

  const btVector3& getGravity() const;
  void setGravity(const btVector3& gravity);
  void setGravity();

  const glm::vec4 &getAmbientColor() const;
  void setAmbientColor(const glm::vec4 &color);

  inline int getLightsNumber() const {
    return m_lights.size();
  }

  inline void setMirror(Mirror *mirror) {
    m_mirror = mirror;
  } 
  inline Mirror *getMirror() {
    return m_mirror;
  }
  inline const Mirror *getMirror() const {
    return m_mirror;
  }

private:
  void initWorld();

//-----------------------------------------------------------------------------
public:
  class object_iterator :
    public std::iterator<std::forward_iterator_tag, Object*> {
  public:
    object_iterator();
    object_iterator(const World& world);
    object_iterator(const object_iterator& original);

  private:
    std::vector<Object*> m_objects;
    size_t m_currentObject;

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
    std::vector<Object*> m_objects;
    size_t m_currentObject;

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
    std::vector<Light*> m_lights;
    size_t m_currentLight;

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
    std::vector<Light*> m_lights;
    size_t m_currentLight;

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

World::const_light_iterator constBeginLights(const World &world);
World::const_light_iterator constBeginLights(const World *world);
World::const_light_iterator constEndLights(const World &world);
World::const_light_iterator constEndLights(const World *world);
