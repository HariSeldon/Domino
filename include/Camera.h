#pragma once

#include <glm/fwd.hpp>
#include <glm/vec4.hpp>

class Camera {

public:
  Camera();
  Camera(const glm::vec4& position, float xRotation, float yRotation);

public:
  void moveForward();
  void moveBackward();
  void rotate(float xOffset, float yOffset);
  void rotateLeft();
  void rotateRight();
  glm::mat4 applyView();

  void dump();

private:
  glm::mat4 buildTransform();
  void move(float step);

private:
  glm::vec4 position;
  float xRotation;
  float yRotation;

  static constexpr float ROTATION_FACTOR = 0.02f;
  static constexpr float FIXED_ROTATION_ANGLE = 30.f;
  static constexpr float STEP = 0.3f;
  static constexpr float DEFAULT_X_ROTATION = 0.f;
  static constexpr float DEFAULT_Y_ROTATION = 0.f;
  static const glm::vec4 DEFAULT_POSITION;
};
