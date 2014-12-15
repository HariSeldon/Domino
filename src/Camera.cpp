#include "Camera.h"

#include "MathUtils.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

#include <iostream>

const glm::vec4 Camera::DEFAULT_POSITION = { 1.f, 1.f, 0.f, 1.f };

Camera::Camera()
    : Camera(Camera::DEFAULT_POSITION, Camera::DEFAULT_X_ROTATION, -M_PI_2) {}

Camera::Camera(const glm::vec4 &position, float xRotation, float yRotation)
    : position(position), xRotation(xRotation), yRotation(yRotation) {}

// ----------------------------------------------------------------------------- 
void Camera::assign(const glm::vec4 position, float xRotation, float yRotation) {
  this->position = position;
  this->xRotation = xRotation;
  this->yRotation = yRotation;
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
void Camera::rotate(float xOffset, float yOffset) {
  xRotation += Camera::ROTATION_FACTOR * xOffset;
  yRotation -= Camera::ROTATION_FACTOR * yOffset;
}

// ----------------------------------------------------------------------------- 
void Camera::rotateLeft() {
  yRotation += Camera::FIXED_ROTATION_ANGLE;
}

// ----------------------------------------------------------------------------- 
void Camera::rotateRight() {
  yRotation -= Camera::FIXED_ROTATION_ANGLE;
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
  std::cout << xRotation << " " << yRotation << " " << glm::to_string(position)
            << std::endl;
}

// ----------------------------------------------------------------------------- 
glm::mat4 Camera::buildTransform() {
  float cy = cos(yRotation);
  float sy = sin(yRotation);
  float cx = cos(xRotation);
  float sx = sin(xRotation);

  return { cy,      0,   -sy,     0, sx * sy,    cx,         sx * cy,    0,
           cx * sy, -sx, cx * cy, 0, position.x, position.y, position.z, 1 };
}
