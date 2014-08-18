#include "Window.h"

#include "GLInitializer.h"
#include "SceneManager.h"

int main(int, char**) {
  glm::ivec2 screenSize = GLInitializer::initSDL();
  Window window;
  GLInitializer::initGL();

  SceneManager *sceneManager = new SceneManager(screenSize);
  // The ownership of the pointer will be taken by window.
  // FIXME: replace this with a smart pointer.
  window.setScene(sceneManager);
  window.startRendering();
  return 0;
}
