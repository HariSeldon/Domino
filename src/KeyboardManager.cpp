#include "KeyboardManager.h"

#include "SDL2/SDL.h"

#include "Window.h"

KeyboardManager::KeyboardManager(Window *window)
    : window(window), lightMask(0), leftDown(false), rightDown(false), forwardDown(false),
      backwardDown(false) {}

bool KeyboardManager::keyPressed(long key) {
  switch (key) {
  case SDLK_ESCAPE: {
    window->stopRendering();
    return true;
  }
  case SDLK_d:
  case SDLK_RIGHT: {
    rightDown = true;
    break;
  }
  case SDLK_a:
  case SDLK_LEFT: {
    leftDown = true;
    break;
  }
  case SDLK_w:
  case SDLK_UP: {
    forwardDown = true;
    break;
  }
  case SDLK_s:
  case SDLK_DOWN: {
    backwardDown = true;
    break;
  }
  case SDLK_1: {
    lightMask ^= 1;
    break;
  }
  case SDLK_2: {
    lightMask ^= 2;
    break;
  }
  case SDLK_3: {
    lightMask ^= 4;
    break;
  }
  case SDLK_4: {
    lightMask ^= 8;
    break;
  }
  }
  return false;
}

void KeyboardManager::keyReleased(long key) {
  switch (key) {
  case SDLK_d:
  case SDLK_RIGHT: {
    rightDown = false;
    break;
  }
  case SDLK_a:
  case SDLK_LEFT: {
    leftDown = false;
    break;
  }
  case SDLK_w:
  case SDLK_UP: {
    forwardDown = false;
    break;
  }
  case SDLK_s:
  case SDLK_DOWN: {
    backwardDown = false;
    break;
  }
  }
}

bool KeyboardManager::isLeftDown() const { return leftDown; }

bool KeyboardManager::isRightDown() const { return rightDown; }

bool KeyboardManager::isForwardDown() const { return forwardDown; }

bool KeyboardManager::isBackwardDown() const { return backwardDown; }

int KeyboardManager::getLightMask() const { return lightMask; }

void KeyboardManager::setLightMask(int lightMask) { this->lightMask = lightMask; }
