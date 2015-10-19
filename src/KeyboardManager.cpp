#include "KeyboardManager.h"

#include "SDL2/SDL.h"

#include "Window.h"

KeyboardManager::KeyboardManager(Window *window) : m_window(window) {}

bool KeyboardManager::keyPressed(long key) {
  switch (key) {
  case SDLK_ESCAPE: {
    m_window->stopRendering();
    return true;
  }
  case SDLK_d:
  case SDLK_RIGHT: {
    m_rightDown = true;
    break;
  }
  case SDLK_a:
  case SDLK_LEFT: {
    m_leftDown = true;
    break;
  }
  case SDLK_w:
  case SDLK_UP: {
    m_forwardDown = true;
    break;
  }
  case SDLK_s:
  case SDLK_DOWN: {
    m_backwardDown = true;
    break;
  }
  case SDLK_1: {
    m_lightMask ^= 1;
    break;
  }
  case SDLK_2: {
    m_lightMask ^= 2;
    break;
  }
  case SDLK_3: {
    m_lightMask ^= 4;
    break;
  }
  case SDLK_4: {
    m_lightMask ^= 8;
    break;
  }
  }
  return false;
}

void KeyboardManager::keyReleased(long key) {
  switch (key) {
  case SDLK_d:
  case SDLK_RIGHT: {
    m_rightDown = false;
    break;
  }
  case SDLK_a:
  case SDLK_LEFT: {
    m_leftDown = false;
    break;
  }
  case SDLK_w:
  case SDLK_UP: {
    m_forwardDown = false;
    break;
  }
  case SDLK_s:
  case SDLK_DOWN: {
    m_backwardDown = false;
    break;
  }
  }
}

bool KeyboardManager::isLeftDown() const { return m_leftDown; }

bool KeyboardManager::isRightDown() const { return m_rightDown; }

bool KeyboardManager::isForwardDown() const { return m_forwardDown; }

bool KeyboardManager::isBackwardDown() const { return m_backwardDown; }

int KeyboardManager::getLightMask() const { return m_lightMask; }

void KeyboardManager::setLightMask(int lightMask) { m_lightMask = lightMask; }
