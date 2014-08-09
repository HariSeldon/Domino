#include "SceneManager.h"

#include "Window.h"
#include "World.h"

SceneManager::SceneManager() : world(new World) {
  world->setGravity();
  window = new Window(world);
}

SceneManager::~SceneManager() {
  delete window;
  delete world;
}

void SceneManager::start() {
  window->startRendering();
}
