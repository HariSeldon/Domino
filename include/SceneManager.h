#pragma once

#include "Camera.h"
#include "TextManager.h"

#include <glm/fwd.hpp>
#include <glm/mat4x4.hpp>

#include <atomic>
#include <mutex>
#include <string>

class Drawer;
class Mirror;
class ShaderProgram;
class World;

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

private:
  void setupProjection(const glm::ivec2 &screenSize);
  void drawWorld(const glm::mat4 &modelView);
  void drawLights(const glm::mat4 &modelView);
  void drawObjects(const glm::mat4 &modelView);
  void drawMirror(Mirror *mirror, const glm::mat4 &modelView);
  void drawText();

private:
  World* world;
  Drawer *drawer;
  ShaderProgram* shader;
  TextManager* textManager;

  glm::mat4 projection;

  Camera camera;

  std::atomic_int currentYRotation;
  std::atomic_int currentXRotation;
  std::mutex cameraMutex;

  int fps;
  int lightMask;
};
