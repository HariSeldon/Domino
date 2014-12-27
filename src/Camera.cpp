#include "Camera.h"

#include "MathUtils.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

#include <iostream>

const float Camera::ROTATION_FACTOR = 0.0004f;

Camera::Camera() {}

Camera::Camera(const glm::vec4 position, const glm::vec2 orientation,
               float viewAngle, float zNear, float zFar)
    : position(position), orientation(orientation), viewAngle(viewAngle),
      zNear(zNear), zFar(zFar) {}

// ----------------------------------------------------------------------------- 
void Camera::assign(const glm::vec4 position,  const glm::vec2 orientation,
                    float viewAngle, float zNear, float zFar) {
  this->position = position;
  this->orientation = orientation;
  this->viewAngle = viewAngle;
  this->zNear = zNear;
  this->zFar = zFar;
}

// ----------------------------------------------------------------------------- 
void Camera::moveForward() { move(Camera::STEP); }

void Camera::moveBackward() { move(-Camera::STEP); }

// ----------------------------------------------------------------------------- 
void Camera::move(float step) {
  glm::mat4 currentTransform = buildTransform();
  // Translate in the new coordinate system.
  currentTransform = glm::translate(currentTransform, glm::vec3(0, 0, step));
  // Get the new center.
  // The center is the result of applying the transformation to the origin.
  position = currentTransform * glm::vec4(0.f, 0.f, 0.f, 1.f);
}

// ----------------------------------------------------------------------------- 
// Change the current orientation angles.
void Camera::rotate(const glm::vec2 offset) {
  orientation += glm::vec2(Camera::ROTATION_FACTOR, Camera::ROTATION_FACTOR) * offset;
}

// ----------------------------------------------------------------------------- 
void Camera::rotateLeft() {
  orientation.y += Camera::FIXED_ROTATION_ANGLE;
}

// ----------------------------------------------------------------------------- 
void Camera::rotateRight() {
  orientation.y -= Camera::FIXED_ROTATION_ANGLE;
}

// ----------------------------------------------------------------------------- 
glm::mat4 Camera::applyView() {
  glm::mat4 currentTransform = buildTransform();

  // Compute center.
  glm::mat4 centerMatrix = currentTransform;
  centerMatrix = glm::translate(centerMatrix, glm::vec3(0.f, 0.f, 1.f));
  glm::vec4 center = centerMatrix * glm::vec4(0.f, 0.f, 0.f, 1.f);

  // The second column of the center matrix contains the up vector.
  glm::vec4 up = centerMatrix[1];
  glm::mat4 lookAtMatrix =
      glm::lookAt(glm::vec3(position), glm::vec3(center), glm::vec3(up));

  return lookAtMatrix;
}

// ----------------------------------------------------------------------------- 
void Camera::dump() {
  std::cout << glm::to_string(position) << " " << glm::to_string(orientation)
            << std::endl;
}

// ----------------------------------------------------------------------------- 
glm::mat4 Camera::buildTransform() {
  glm::vec2 sine = glm::sin(orientation);
  glm::vec2 cosine = glm::cos(orientation);

  float cy = cosine.y;
  float sy = sine.y;
  float cx = cosine.x;
  float sx = sine.x;

  return { cy,      0,   -sy,     0, sx * sy,    cx,         sx * cy,    0,
           cx * sy, -sx, cx * cy, 0, position.x, position.y, position.z, 1 };
}
