#pragma once

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>


class Camera {

public:
  static constexpr float STEP = 0.5f;
  static constexpr float FIXED_ROTATION_ANGLE = 0.02f;
  Camera();
  Camera(const glm::vec4 position, 
         const glm::vec2 rotation,
         float viewAngle, float zNear, float zFar);

public:
  void assign(const glm::vec4 position, const glm::vec2 rotation,
              float viewAngle, float zNear, float zFar);

  void move(float step);
  void moveForward();
  void moveBackward();
  void rotate(const glm::vec2 offset);
  void rotateLeft();
  void rotateRight();
  glm::mat4 applyView();

  inline const glm::vec4 &getPosition() const {
    return position;
  }

  inline float getXOrientation() const {
    return orientation.x;
  }

  inline float getYOrientation() const {
    return orientation.y;
  }
  
  inline float getViewAngle() const {
    return viewAngle;
  }

  inline float getZNear() const {
    return zNear;
  }

  inline float getZFar() const {
    return zFar;
  }

  void dump();

private:
  glm::mat4 buildTransform();

private:
  glm::vec4 position;
  // In radians.
  glm::vec2 orientation;
  // In degrees.
  float viewAngle;
  float zNear;
  float zFar;
  glm::mat4 currentTransform;

  static const float ROTATION_FACTOR;
};
