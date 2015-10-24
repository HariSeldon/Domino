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
#include <functional>
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
  SceneManager(const glm::ivec2 screenSize, SceneContainer *container);
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
  void drawWorld(const glm::mat4 &modelView, const glm::vec3 &cameraPosition);
  void drawWorldForMirror(const glm::mat4 &modelView);
  void drawShadowWorld(const glm::mat4 &modelView, const glm::mat4 &projection,
                       ShaderProgram &shader);
  void drawText();
  void mirrorRenderingPass();
  void noMirrorRenderingPass();
  void shadowRenderingPass();
  void screenRenderingPass();

private:
  World* m_world = nullptr;
  Camera* m_camera = nullptr;

  #ifndef WINDOWS
  TextManager m_textManager;
  #endif
  Drawer m_drawer;
  ShadowManager m_shadowManager;

  std::function<void(SceneManager*)> m_mirrorPass;

  glm::mat4 m_projection;

  const glm::vec4 m_BACKGROUND_COLOR;
  float m_currentYRotation = 0.f;
  float m_currentXRotation = 0.f;
  float m_currentOffset = 0.f;
  SDL_mutex *m_positionMutex = nullptr;

  int m_fps = 0;
  int m_lightMask = 0;
};
