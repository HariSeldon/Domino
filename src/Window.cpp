#include "Window.h"

#include <algorithm>
#include <memory>

#include <glm/vec4.hpp>
#include <glm/ext.hpp>

#include "Light.h"
#include "Mirror.h"
#include "Object.h"
#include "SysDefines.h"
#include "SysUtils.h"

// Support functions.
// -----------------------------------------------------------------------------
Uint32 eventHandler(Uint32 interval, void *);
Uint32 fpsHandler(Uint32 interval, void *);
void checkSDLError(const std::string &message);
void abort(const std::string &message);

const glm::vec4 Window::CLEAR_COLOR = {0.2f, 0.4f, 0.6f, 1.f};
const float Window::VIEW_ANGLE = 50.0f;
const float Window::Z_NEAR = 0.1f;
const float Window::Z_FAR = 2000.0f;
const std::string Window::FONT_FILE = "VeraMono.ttf";

// -----------------------------------------------------------------------------
Window::Window()
    : sdlWindow(nullptr), context(nullptr), eventTimerId(0), 
      keyboardManager(this), running(true), height(0), width(0),
      currentYRotation(0), currentXRotation(0), frameCounter(0), fps(0) {

  int initResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
  assert(initResult == 0 && "Unable to initialize SDL");

  initSDL();
  initSDLWindow();
  initGL();

  setupProjection();
  world = new World();
  lightShaderProgram = new ShaderProgram("phong.vert", "phong.frag");
  drawer = new Drawer();

  drawer->initGPUObjects(*lightShaderProgram, *world);
  if(Mirror *mirror = world->getMirror()) {
    ShaderProgram *mirrorShader = mirror->getShaderProgram();
    drawer->initMirror(*mirrorShader, mirror);
  }

  keyboardManager.setLightMask((2 << (world->getLightsNumber() - 1)) - 1);
  textManager = new TextManager(FONT_PATH + FONT_FILE, FONT_HEIGHT, width, height);
}

// -----------------------------------------------------------------------------
Window::~Window() {
  glUseProgram(0);
  delete lightShaderProgram;
  delete drawer;
  delete textManager;
  delete world;
  SDL_RemoveTimer(eventTimerId);
  SDL_RemoveTimer(fpsTimerId);
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(sdlWindow);
  SDL_Quit();
}

// -----------------------------------------------------------------------------
void Window::initSDLWindow() {
  sdlWindow =
      SDL_CreateWindow("Domino", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       0, 0,
                       SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN |
                       SDL_WINDOW_FULLSCREEN | SDL_WINDOW_INPUT_GRABBED);
  checkSDLError("Error creating sdlWindow");
  assert(sdlWindow != nullptr && "Error creating sdlWindow");

  context = SDL_GL_CreateContext(sdlWindow);
  checkSDLError("Error creating GL context");
  assert(context != nullptr && "Error GL creating context"); 

  SDL_GL_MakeCurrent(sdlWindow, context);
  SDL_SetRelativeMouseMode(SDL_TRUE);

  eventTimerId = SDL_AddTimer(Window::EVENT_FREQUENCY, eventHandler,
                              static_cast<void *>(this));
  assert(eventTimerId != 0 && "Failure creating event timer");
  fpsTimerId = SDL_AddTimer(Window::FPS_FREQUENCY, fpsHandler,
                            static_cast<void *>(this));
  assert(fpsTimerId != 0 && "Failure creating fps timer");
}

// -----------------------------------------------------------------------------
void Window::stopRendering() { running = false; }

// -----------------------------------------------------------------------------
void Window::startRendering() {
  while (1) {
    if (!running)
      break;

    drawScene();

    ++frameCounter;
    SDL_GL_SwapWindow(sdlWindow);
    SDL_Delay(Window::DISPLAY_DELAY);
  }
}

// -----------------------------------------------------------------------------
void Window::setupProjection() {
  glViewport(0, 0, width, height);
  float aspectRatio = (float) width / height;
  projection = glm::perspective(Window::VIEW_ANGLE, aspectRatio, Window::Z_NEAR,
                                Window::Z_FAR);
}

// -----------------------------------------------------------------------------
void Window::drawScene() {
  lightShaderProgram->useProgram();
  lightShaderProgram->setUniform("projectionMatrix", projection);

  if(Mirror *mirror = world->getMirror()) {
    // Draw the scene on the mirror texture from the point of view of the
    // mirror.
    mirror->enableMirror();
    // Setup a camera for the mirror.
    Camera mirrorCamera(glm::vec4(0.f, 1.0f, 15.f, 1.0f), 0.f, 180.f);
    modelView = mirrorCamera.applyView(); 

    drawWorld();   
    mirror->disableMirror();
  }

  updateCameraPosition();

  drawWorld();

  if (Mirror *mirror = world->getMirror()) {
    // Enable mirror shader.
    ShaderProgram *program = mirror->getShaderProgram();
    program->useProgram();
    program->setUniform("projectionMatrix", projection);
    drawMirror(mirror);
  }

  // Draw text on top of the scene.
  drawText();
}

// -----------------------------------------------------------------------------
void Window::drawWorld() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  drawObjects();
  drawLights();
}

// -----------------------------------------------------------------------------
void Window::drawObjects() {
  std::for_each(constBeginObjects(world), constEndObjects(world),
                [&](const Object *object) {
    drawer->drawObject(object, *lightShaderProgram, modelView);
  });
}

// -----------------------------------------------------------------------------
void Window::drawLights() {
  lightShaderProgram->setUniform("ambientColor", world->getAmbientColor());
  lightShaderProgram->setUniform("lightsNumber", world->getLightsNumber());
  lightShaderProgram->setUniform("lightMask", keyboardManager.getLightMask());
  std::for_each(
      constBeginLights(world), constEndLights(world),
      [&](const Light *light) { light->draw(*lightShaderProgram, modelView); });
}

// -----------------------------------------------------------------------------
void Window::drawMirror(Mirror *mirror) {
  ShaderProgram *mirrorShader = mirror->getShaderProgram();
  drawer->drawObject(mirror, *mirrorShader, modelView);
}

// -----------------------------------------------------------------------------
void Window::drawText() {
  glDisable(GL_DEPTH_TEST);
  textManager->addText("Frames per second: " + std::to_string(fps), {0, 780});
  textManager->renderText();
  glEnable(GL_DEPTH_TEST);
}

// -----------------------------------------------------------------------------
void Window::updateCameraPosition() {

  if (keyboardManager.isForwardDown())
    camera.moveForward();
  if (keyboardManager.isBackwardDown())
    camera.moveBackward();
  if (keyboardManager.isLeftDown())
    camera.rotateLeft();
  if (keyboardManager.isRightDown())
    camera.rotateRight();

  // When rotating the camera the lights change intensity.
  camera.rotate(currentYRotation, currentXRotation);
  currentYRotation = 0;
  currentXRotation = 0;

  modelView = camera.applyView();
}

// Support functions.
// -----------------------------------------------------------------------------
void Window::initSDL() {
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,
                      Window::OPENGL_MAJOR_VERSION);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
                      Window::OPENGL_MINOR_VERSION);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, Window::OPENGL_DOUBLE_BUFFER);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, Window::OPENGL_DEPTH_SIZE);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, Window::OPENGL_RED_SIZE);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, Window::OPENGL_GREEN_SIZE);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, Window::OPENGL_BLUE_SIZE);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, Window::OPENGL_ALPHA_SIZE);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,
                      Window::OPENGL_MULTISAMPLE_BUFFERS);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,
                      Window::OPENGL_MULTISAMPLE_SAMPLES);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, Window::OPENGL_BUFFER_SIZE);

  SDL_DisplayMode displayMode;
  SDL_GetCurrentDisplayMode(0, &displayMode);
  width = displayMode.w;
  height = displayMode.h;

  assert(width != 0 && height != 0 && "Error setting width and height");
}

// -----------------------------------------------------------------------------
void Window::initGL() {
  glewExperimental = GL_TRUE;
  int glewStatus = glewInit();
  assert(glewStatus == GLEW_OK && "Error initializing glew");

  glClearColor(Window::CLEAR_COLOR.x, Window::CLEAR_COLOR.y,
               Window::CLEAR_COLOR.z, Window::CLEAR_COLOR.w);
  checkOpenGLError("Window: glClearColor");
  glEnable(GL_DEPTH_TEST);
  checkOpenGLError("Window: glEnable-GL_DEPTH_TEST");
  glEnable(GL_BLEND);
  checkOpenGLError("Window: glEnable-GL_BLEND");
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  checkOpenGLError("Window: glBlendFunc-GL_SRC_ALPHA");
}

// -----------------------------------------------------------------------------
Uint32 eventHandler(Uint32 interval, void *windowPtr) {
  Window *window = static_cast<Window *>(windowPtr);

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
      break;
    }

    case SDL_KEYUP: {
      window->keyboardManager.keyReleased(event.key.keysym.sym);
      break;
    }

    case SDL_MOUSEMOTION: {
      int xDiff = event.motion.xrel;
      int yDiff = event.motion.yrel;
      window->currentXRotation += xDiff;
      window->currentYRotation += yDiff;
      break;
    }

    case SDL_WINDOWEVENT: {
      switch (event.window.event) { 
        case SDL_WINDOWEVENT_RESIZED: {
          break;
        }
      }
      break;
    }
    }
  }
  return interval;
}

// -----------------------------------------------------------------------------
Uint32 fpsHandler(Uint32 interval, void *windowPtr) {
  Window *window = static_cast<Window *>(windowPtr);
  window->fps = window->frameCounter;
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

// FIXME: I have to add the stepping of the simulation.
