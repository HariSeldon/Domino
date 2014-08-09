#pragma once

class Window;
class World;

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
  World *world;
  Window *window;
};
