#pragma once

#include "Camera.h"
#include "Drawer.h"
#include "KeyboardManager.h"
#include "SceneManager.h"
#include "ShaderProgram.h"
#include "TextManager.h"
#include "World.h"

#include <atomic>
#include <memory>
#include <mutex>

#include <GL/glew.h>

#include "SDL2/SDL.h"

class Window {
public:
  static constexpr int EVENT_FREQUENCY = 20;
  static constexpr int FPS_FREQUENCY = 1000;
  static constexpr int DISPLAY_DELAY = 15;

public:
  Window();
  ~Window();

public:
  inline void setScene(std::unique_ptr<SceneManager> sceneManager) {
    scene = std::move(sceneManager);
  }
  void startRendering();
  void stopRendering();

private:
  void initSDLWindow();
  void attachTimers();
  void renderingLoop();
  void updateCurrentCameraPosition();
  void setupProjection(); 
  inline SceneManager* getScene() {
    return scene.get();
  }

private:
  SDL_Window *sdlWindow;
  SDL_GLContext context;

  std::unique_ptr<SceneManager> scene;

  SDL_TimerID eventTimerId;
  SDL_TimerID fpsTimerId;

  KeyboardManager keyboardManager; 

  bool running; 

  std::mutex cameraMutex;

  std::atomic_int frameCounter;

friend Uint32 eventHandler(Uint32 interval, void *);
friend Uint32 fpsHandler(Uint32 interval, void *);

};
