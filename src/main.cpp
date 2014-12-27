#include "Window.h"

#include "GLInitializer.h"
#include "SceneManager.h"
#include "ScriptEngine.h"

#include <memory>

extern Camera *tmpCamera;
extern World *tmpWorld;

int main(int, char **) {
  World *world = new World();
  Camera *camera = new Camera();

  tmpCamera = camera;
  tmpWorld = world;

  runScript("hello.lua");

  // Init GL.
  glm::ivec2 screenSize = GLInitializer::initSDL();
  Window window;
  GLInitializer::initGL();

  auto sceneManager = std::unique_ptr<SceneManager>(
      new SceneManager(screenSize, world, camera));

  window.setScene(std::move(sceneManager));
  window.startRendering();
  return 0;
}
