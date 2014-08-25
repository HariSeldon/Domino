#pragma once

#include "Camera.h"
#include "Drawer.h"
#include "ShaderProgram.h"
#include "ShadowManager.h"
#include "TextManager.h"
#include "World.h"

#include <glm/fwd.hpp>
#include <glm/mat4x4.hpp>

#include <atomic>
#include <mutex>
#include <string>

class Mirror;
class ShadowManager;

class SceneManager {
public:
  static const float VIEW_ANGLE;
  static const float Z_NEAR;
  static const float Z_FAR;

  static const glm::vec4 CLEAR_COLOR;

  static constexpr int FONT_HEIGHT = 20;
  static const std::string FONT_FILE;
  static const std::string MAIN_VERTEX_SHADER;
  static const std::string MAIN_FRAGMENT_SHADER;

public:
  SceneManager(const glm::ivec2 &screenSize);
  ~SceneManager();

public:
  void drawScene();

  void updateCameraPosition(unsigned char keyMask);
  void updateCameraRotation(glm::ivec2 diff);

  void setFps(int fps);

  int getLightMask() const;
  void updateLightMask(int lightMask);

  void stepSimulation();

private:
  void initGPU();
  void setupProjection(const glm::ivec2 &screenSize);
  void drawWorld(const glm::mat4 &modelView, ShaderProgram &shader);
  void drawShadowWorld(const glm::mat4 &modelView, const glm::mat4 &projection,
                       ShaderProgram &shader);
  void drawLights(const glm::mat4 &modelView, ShaderProgram &shader);
  void drawObjects(const glm::mat4 &modelView, ShaderProgram &shader);
  void drawMirror(const glm::mat4 &modelView);
  void drawText();
  void mirrorRenderingPass();
  void shadowRenderingPass();
  void screenRenderingPass();

private:
  World world;
  Drawer drawer;
  ShaderProgram worldShader;
  TextManager textManager;
  ShadowManager shadowManager;

  glm::mat4 projection;

  Camera camera;

  std::atomic_int currentYRotation;
  std::atomic_int currentXRotation;
  std::mutex cameraMutex;

  int fps;
  int lightMask;
};
