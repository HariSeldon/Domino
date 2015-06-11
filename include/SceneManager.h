#pragma once

#include "Camera.h"
#include "Drawer.h"
#include "SceneContainer.h"
#include "ShaderProgram.h"
#include "ShadowManager.h"
#include "TextManager.h"
#include "World.h"

#include <glm/fwd.hpp>
#include <glm/mat4x4.hpp>

#include <atomic>
#include <memory>
#include <string>

#include "SDL2/SDL_thread.h"

class Mirror;
class ShadowManager;

class SceneManager {
public:
  static constexpr int FONT_HEIGHT = 20;
  static constexpr int MAX_LIGHTS_NUMBER = 4;
  static const std::string FONT_FILE;

public:
  SceneManager(const glm::ivec2 &screenSize, SceneContainer *container);
  ~SceneManager();

public:
  void drawScene();

  void updateCameraPosition();
  void updateCurrentCameraPosition(unsigned char keyMask);
  void updateCurrentCameraRotation(glm::vec2 diff);

  void setFps(int fps);

  int getLightMask() const;
  void updateLightMask(int lightMask);

  void stepSimulation();

private:
  void initGPU(SceneContainer *container);
  void initTextures();
  void setupProjection(const glm::ivec2 &screenSize);
  void drawWorld(const glm::mat4 &modelView);
  void drawShadowWorld(const glm::mat4 &modelView, const glm::mat4 &projection,
                       ShaderProgram &shader);
  void drawMirror(const glm::mat4 &modelView);
  void drawText();
  void mirrorRenderingPass();
  void shadowRenderingPass();
  void screenRenderingPass();

private:
  World* world;
  Camera* camera;

  TextManager textManager;
  Drawer drawer;
  ShadowManager shadowManager;

  glm::mat4 projection;

  const glm::vec4 BACKGROUND_COLOR;
  float currentYRotation;
  float currentXRotation;
  float currentOffset;
  SDL_mutex *positionMutex;

  int fps;
  int lightMask;
};
