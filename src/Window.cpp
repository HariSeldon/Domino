#include "Window.h"

#include "Light.h"
#include "Mirror.h"
#include "Object.h"
#include "SysDefines.h"
#include "SysUtils.h"
#include "MathUtils.h"

#include <algorithm>
#include <iostream>
#include <memory>

#include <glm/vec4.hpp>
#include <glm/ext.hpp>

// Support functions.
// -----------------------------------------------------------------------------
Uint32 eventHandler(Uint32 interval, void *);
Uint32 fpsHandler(Uint32 interval, void *);
void checkSDLError(const std::string &message);
void abort(const std::string &message);

// -----------------------------------------------------------------------------
Window::Window()
    : sdlWindow(nullptr), context(nullptr), scene(nullptr),
      eventTimerId(0), fpsTimerId(0), keyboardManager(this), running(true),
      frameCounter(0)  {
  initSDLWindow();
}

// -----------------------------------------------------------------------------
void Window::initSDLWindow() {
  sdlWindow = SDL_CreateWindow(
      "Domino", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0,
      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN |
          SDL_WINDOW_INPUT_GRABBED);
  checkSDLError("Error creating sdlWindow");
  assert(sdlWindow != nullptr && "Error creating sdlWindow");

  context = SDL_GL_CreateContext(sdlWindow);
  checkSDLError("Error creating GL context");
  assert(context != nullptr && "Error GL creating context");

  SDL_GL_MakeCurrent(sdlWindow, context);
  SDL_SetRelativeMouseMode(SDL_TRUE);
}

// -----------------------------------------------------------------------------
Window::~Window() {
  SDL_RemoveTimer(eventTimerId);
  SDL_RemoveTimer(fpsTimerId);
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(sdlWindow);
  SDL_Quit();
}

// -----------------------------------------------------------------------------
void Window::stopRendering() { running = false; }

// -----------------------------------------------------------------------------
void Window::startRendering() {
  assert(scene != nullptr && "Empty scene!");
  keyboardManager.setLightMask(scene->getLightMask());

  attachTimers();
  renderingLoop();
}

// -----------------------------------------------------------------------------
void Window::attachTimers() {
  eventTimerId = SDL_AddTimer(Window::EVENT_FREQUENCY, eventHandler,
                              static_cast<void *>(this));
  assert(eventTimerId != 0 && "Failure creating event timer");
  fpsTimerId = SDL_AddTimer(Window::FPS_FREQUENCY, fpsHandler,
                            static_cast<void *>(this));
  assert(fpsTimerId != 0 && "Failure creating fps timer");
}

// -----------------------------------------------------------------------------
void Window::renderingLoop() {
  while (1) {
    if (!running)
      break;

    scene->updateCameraPosition();
    scene->drawScene();

    ++frameCounter;
    SDL_GL_SwapWindow(sdlWindow);
  }
}

// -----------------------------------------------------------------------------
void Window::updateCurrentCameraPosition() {
  unsigned char forward = keyboardManager.isForwardDown() << 3;
  unsigned char backward = keyboardManager.isBackwardDown() << 2;
  unsigned char left = keyboardManager.isLeftDown() << 1;
  unsigned char right = keyboardManager.isRightDown();

  unsigned char mask = forward | backward | left | right;

  scene->updateCurrentCameraPosition(mask);
}

// -----------------------------------------------------------------------------
Uint32 eventHandler(Uint32 interval, void *windowPtr) {
  Window *window = static_cast<Window *>(windowPtr);
  SceneManager *scene = window->getScene();

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT: {
      window->stopRendering();
      return 0;
    }

    case SDL_KEYDOWN: {
      if (event.key.repeat != 0)
        break;

      bool kill = window->keyboardManager.keyPressed(event.key.keysym.sym);
      if (kill)
        return 0;
      scene->updateLightMask(window->keyboardManager.getLightMask());
      break;
    }

    case SDL_KEYUP: {
      window->keyboardManager.keyReleased(event.key.keysym.sym);
      break;
    }

    case SDL_MOUSEMOTION: {
      int xDiff = event.motion.xrel;
      int yDiff = event.motion.yrel;
      scene->updateCurrentCameraRotation({xDiff, yDiff});
      break;
    }

    case SDL_WINDOWEVENT: {
      switch (event.window.event) {
      case SDL_WINDOWEVENT_RESIZED: { break; }
      }
      break;
    }
    }
  }

  window->updateCurrentCameraPosition();
  // This makes the update of the scene independent of the frame-rate.
  scene->stepSimulation(); 
  return interval;
}

// -----------------------------------------------------------------------------
Uint32 fpsHandler(Uint32 interval, void *windowPtr) {
  Window *window = static_cast<Window *>(windowPtr);
  window->getScene()->setFps(window->frameCounter);
  window->frameCounter = 0;
  return interval;
}

// -----------------------------------------------------------------------------
void checkSDLError(const std::string &message) {
  const char *error = SDL_GetError();
  if (*error != '\0') {
    std::cout << message << " - " << error << "\n";
    SDL_ClearError();
  }
}
