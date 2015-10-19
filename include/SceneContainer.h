#pragma once

#include "Camera.h"
#include "SysDefines.h"
#include "World.h"
#include "Mirror.h"
#include "Object.h"

#include <LinearMath/btVector3.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <map>
#include <string>

class Light;
class Object;

class SceneContainer {
public:
  SceneContainer() : m_camera(new Camera()), m_world(new World()) {}

public:
  // World setup.
  inline void addDirectionalLight(DirectionalLight *light) {
    m_world->addDirectionalLight(light);
  }
//  inline void addSpotLight(SpotLight *light) {
//    world->addSpotLight(light);
//  }
  inline void addObject(Object *object) { m_world->addObject(object); }
  inline void addLightBulb(LightBulb *bulb) { m_world->addLightBulb(bulb); }
  inline void setMirror(Mirror *mirror) { 
    m_world->setMirror(mirror); 
    addObject(dynamic_cast<Object*>(mirror));
  } 
  inline void setGravity(const btVector3 &gravity) {
    m_world->setGravity(gravity);
  }

  // Camera setup.
  inline void setCamera(const glm::vec4 position, const glm::vec2 orientation,
                        float viewAngle, float zNear, float zFar) {
    m_camera->assign(position, orientation, viewAngle, zNear, zFar);
  }

  inline void setBackgroundColor(const glm::vec4 backgroundColor) {
    m_backgroundColor = backgroundColor;
  }

  inline void addShader(const Object *object, const std::string shaderFile) {
    m_shaderFileMap[shaderFile].push_back(object);
  }

  // Getters.
  // FIXME use smart pointers.
  inline World *getWorld() const { return m_world; }
  inline Camera *getCamera() const { return m_camera; }
  inline glm::vec4 getBackgroundColor() const { return m_backgroundColor; }
  inline auto getShaderMap() const { return m_shaderFileMap; }

private:
  Camera *m_camera;
  World *m_world;
  std::map<const std::string, std::vector<const Object*>> m_shaderFileMap;
  glm::vec4 m_backgroundColor;
};
