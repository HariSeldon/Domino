#pragma once

class Window;

class KeyboardManager {
public:
  KeyboardManager(Window *window);

public:
  bool keyPressed(long key);
  void keyReleased(long key);

  bool isLeftDown() const;
  bool isRightDown() const;
  bool isForwardDown() const;
  bool isBackwardDown() const;

  int getLightMask() const;
  void setLightMask(int lightMask);

private:
  Window *m_window = nullptr;

  int m_lightMask = 0;

  bool m_leftDown = 0;
  bool m_rightDown = 0;
  bool m_forwardDown = 0; 
  bool m_backwardDown = 0;
};
