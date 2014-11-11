#include "Window.h"

#include "GLInitializer.h"
#include "SceneManager.h"

#include <memory>

int main(int, char**) {
  glm::ivec2 screenSize = GLInitializer::initSDL();
  Window window;
  GLInitializer::initGL();

  auto sceneManager =
      std::unique_ptr<SceneManager>(new SceneManager(screenSize));

  window.setScene(std::move(sceneManager));
  window.startRendering();
  return 0;
}
