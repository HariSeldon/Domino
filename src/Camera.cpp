#include "Camera.h"

#include "MathUtils.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

#include <iostream>

const float Camera::ROTATION_FACTOR = 0.0004f;
const float Camera::STEP = 0.5f;
const float Camera::FIXED_ROTATION_ANGLE = 0.02f;

Camera::Camera() {}

Camera::Camera(const glm::vec4 position, const glm::vec2 orientation,
               float viewAngle, float zNear, float zFar)
    : m_position(position), m_orientation(orientation), m_viewAngle(viewAngle),
      m_zNear(zNear), m_zFar(zFar), currentTransform(buildTransform()) {}

// -----------------------------------------------------------------------------
void Camera::assign(const glm::vec4 position, const glm::vec2 orientation,
                    float viewAngle, float zNear, float zFar) {
  m_position = position;
  m_orientation = orientation;
  m_viewAngle = viewAngle;
  m_zNear = zNear;
  m_zFar = zFar;
}

// -----------------------------------------------------------------------------
void Camera::moveForward() { move(Camera::STEP); }

void Camera::moveBackward() { move(-Camera::STEP); }

// -----------------------------------------------------------------------------
void Camera::move(float step) {
  currentTransform = buildTransform();
  // Translate in the new coordinate system.
  currentTransform = glm::translate(currentTransform, glm::vec3(0, 0, step));
  m_position = currentTransform[3];
}

// -----------------------------------------------------------------------------
// Change the current orientation angles.
void Camera::rotate(const glm::vec2 offset) {
  m_orientation +=
      glm::vec2(Camera::ROTATION_FACTOR, Camera::ROTATION_FACTOR) * offset;
}

// -----------------------------------------------------------------------------
void Camera::rotateLeft() { m_orientation.y += Camera::FIXED_ROTATION_ANGLE; }

// -----------------------------------------------------------------------------
void Camera::rotateRight() { m_orientation.y -= Camera::FIXED_ROTATION_ANGLE; }

// -----------------------------------------------------------------------------
glm::mat4 Camera::applyView() {
  // Compute center.
  glm::mat4 centerMatrix =
      glm::translate(currentTransform, glm::vec3(0.f, 0.f, 1.f));
  // The fourth column of the center matrix contains the position of the center.
  glm::vec4 center = centerMatrix[3];
  // The second column of the center matrix contains the up vector.
  glm::vec4 up = centerMatrix[1];
  return glm::lookAt(glm::vec3(m_position), glm::vec3(center), glm::vec3(up));
}

// -----------------------------------------------------------------------------
void Camera::dump() {
  std::cout << glm::to_string(m_position) << " " << glm::to_string(m_orientation)
            << std::endl;
}

// -----------------------------------------------------------------------------
glm::mat4 Camera::buildTransform() {
  glm::vec2 sine = glm::sin(m_orientation);
  glm::vec2 cosine = glm::cos(m_orientation);

  float cy = cosine.y;
  float sy = sine.y;
  float cx = cosine.x;
  float sx = sine.x;

  return {cy,      0,   -sy,     0, sx * sy,    cx,         sx * cy,    0,
          cx * sy, -sx, cx * cy, 0, m_position.x, m_position.y, m_position.z, 1};
}
