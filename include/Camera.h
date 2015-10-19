#pragma once

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>


class Camera {

public:
  static const float STEP;
  static const float FIXED_ROTATION_ANGLE;
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
    return m_position;
  }

  inline float getXOrientation() const {
    return m_orientation.x;
  }

  inline float getYOrientation() const {
    return m_orientation.y;
  }
  
  inline float getViewAngle() const {
    return m_viewAngle;
  }

  inline float getZNear() const {
    return m_zNear;
  }

  inline float getZFar() const {
    return m_zFar;
  }

  void dump();

private:
  glm::mat4 buildTransform();

private:
  glm::vec4 m_position;
  // In radians.
  glm::vec2 m_orientation;
  // In degrees.
  float m_viewAngle = 0.f;
  float m_zNear = 0.f;
  float m_zFar = 0.f;
  glm::mat4 currentTransform;

  static const float ROTATION_FACTOR;
};
