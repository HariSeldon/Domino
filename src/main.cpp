#include "GLInitializer.h"
#include "SceneContainer.h"
#include "SceneManager.h"
#include "ScriptEngine.h"
#include "Window.h"

#include <memory>

#include <glm/ext.hpp>

extern SceneContainer *tmpContainer;

int main(int, char **) {
  auto container = new SceneContainer();
  tmpContainer = container;

  // Fill the container with the script.
  runScript("hello.lua");

  // Init GL.
  glm::ivec2 screenSize = GLInitializer::initSDL();
  
  std::cout << "Size: " << glm::to_string(screenSize) << "\n";

  Window window;
  GLInitializer::initGL();

  auto sceneManager = std::unique_ptr<SceneManager>(
      new SceneManager(screenSize, container));

  window.setScene(std::move(sceneManager));
  window.startRendering();
  return 0;
}
