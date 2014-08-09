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
  Window *window;

  int lightMask;

  bool leftDown;
  bool rightDown;
  bool forwardDown;
  bool backwardDown;
};
