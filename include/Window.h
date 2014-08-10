#pragma once

#include <atomic>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>

#include <GL/glew.h>

#include "SDL2/SDL.h"

#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include "Camera.h"
#include "Drawer.h"
#include "KeyboardManager.h"
#include "ShaderProgram.h"
#include "TextManager.h"
#include "World.h"

class Window {
public:
  static constexpr int OPENGL_DOUBLE_BUFFER = 1;
  static constexpr int OPENGL_DEPTH_SIZE = 24;
  static constexpr int OPENGL_MAJOR_VERSION = 3;
  static constexpr int OPENGL_MINOR_VERSION = 3;

  static constexpr int OPENGL_RED_SIZE = 4;
  static constexpr int OPENGL_GREEN_SIZE = 4;
  static constexpr int OPENGL_BLUE_SIZE = 4;
  static constexpr int OPENGL_ALPHA_SIZE = 4;

  static constexpr int OPENGL_MULTISAMPLE_BUFFERS = 1;
  static constexpr int OPENGL_MULTISAMPLE_SAMPLES = 4;
  static constexpr int OPENGL_BUFFER_SIZE = 32;

  static constexpr int EVENT_FREQUENCY = 20;
  static constexpr int FPS_FREQUENCY = 1000;
  static constexpr int DISPLAY_DELAY = 15;

  static constexpr int FONT_HEIGHT = 20;
  static const std::string FONT_FILE;

  static const float VIEW_ANGLE;
  static const float Z_NEAR;
  static const float Z_FAR;

  static const glm::vec4 CLEAR_COLOR;

public:
  Window();
  ~Window();

public:
  void startRendering();
  void stopRendering();

private:
  void initSDLWindow();
  void initSDL();
  void initGL();
  void drawScene();
  void drawWorld();
  void drawLights();
  void drawObjects();
  void drawMirror(Mirror *mirror);
  void drawText();
  void updateCameraPosition();
  void setupProjection(); 

private:
  SDL_Window *sdlWindow;
  SDL_GLContext context;
  SDL_TimerID eventTimerId;
  SDL_TimerID fpsTimerId;

  KeyboardManager keyboardManager; 
  Camera camera;

  World* world;
  bool running; 
  glm::mat4 projection;
  glm::mat4 modelView;
  ShaderProgram *lightShaderProgram;
  Drawer *drawer;
  TextManager *textManager;

  int height;
  int width;
  std::atomic_int currentYRotation;
  std::atomic_int currentXRotation;

  std::atomic_int frameCounter;
  int fps;

friend Uint32 eventHandler(Uint32 interval, void *);
friend Uint32 fpsHandler(Uint32 interval, void *);

};
