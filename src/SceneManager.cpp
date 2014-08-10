#include "SceneManager.h"

#include "Window.h"

SceneManager::SceneManager() {
  window = new Window();
}

SceneManager::~SceneManager() {
  delete window;
}

void SceneManager::start() {
  window->startRendering();
}
