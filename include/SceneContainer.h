#pragma once

#include "Camera.h"
#include "SysDefines.h"
#include "World.h"

#include <LinearMath/btVector3.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <map>
#include <string>

class Light;
class Object;

class SceneContainer {
public:
  SceneContainer() : camera(new Camera()), world(new World()) {}

public:
  // World setup.
//  inline void addPositionalLight(PositionalLight *light) {
//    world->addPositionalLight(light);
//  }
  inline void addDirectionalLight(DirectionalLight *light) {
    world->addDirectionalLight(light);
  }
//  inline void addSpotLight(SpotLight *light) {
//    world->addSpotLight(light);
//  }
  inline void addObject(Object *object) { world->addObject(object); }
  inline void addLightBulb(LightBulb *bulb) { world->addLightBulb(bulb); }
  inline void setGravity(const btVector3 &gravity) {
    world->setGravity(gravity);
  }

  // Camera setup.
  inline void setCamera(const glm::vec4 position, const glm::vec2 orientation,
                        float viewAngle, float zNear, float zFar) {
    camera->assign(position, orientation, viewAngle, zNear, zFar);
  }

  inline void setBackgroundColor(const glm::vec4 backgroundColor) {
    this->backgroundColor = backgroundColor;
  }

  inline void addShader(const Object *object, const std::string shaderFile) {
//    shaderFileMap.insert(std::make_pair(object, shaderFile));
    shaderFileMap[shaderFile].push_back(object);
  }

  // Getters.
  // FIXME use smart pointers.
  inline World *getWorld() const { return world; }
  inline Camera *getCamera() const { return camera; }
  inline glm::vec4 getBackgroundColor() const { return backgroundColor; }
  //inline const std::map<const Object *, const std::string> &
  inline const std::map<const std::string, std::vector<const Object*>> &
  getShaderMap() const {
    return shaderFileMap;
  }

private:
  World *world;
  Camera *camera;
//  std::map<const Object *, const std::string> shaderFileMap;
  std::map<const std::string, std::vector<const Object*>> shaderFileMap;
  glm::vec4 backgroundColor;
};
