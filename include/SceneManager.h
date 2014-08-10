#pragma once

class Window;

class SceneManager {
public:
  SceneManager();
  ~SceneManager();

public:
  void start();

private:
  void initWorld();
  void initWindow();

private:
  Window *window;
};
