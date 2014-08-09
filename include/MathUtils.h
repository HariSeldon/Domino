#pragma once

#include <LinearMath/btVector3.h>

#include <string>

#include <glm/vec3.hpp>
#include <glm/glm.hpp>

class btMatrix3x3;
class btQuaternion;
class btTransform;
class btVector3;

inline glm::vec3 computeNormal(const glm::vec3 &first, const glm::vec3 &second,
                               const glm::vec3 &third) {
  return glm::normalize(glm::cross(first - second, second - third));
}

btScalar clampToDegrees(btScalar value);
btScalar degreesToRadiants(btScalar degrees);
int signum(btScalar x);

// Dump functions.
void dumpVector(const btVector3& vector, const std::string& name = "");
void dumpVector(const btVector4& vector, const std::string& name = "");
void dumpMatrix(const btMatrix3x3& matrix, const std::string& name = "");
void dumpOpenGLMatrix(const float* matrix, const std::string& name = "");
void dumpQuaternion(const btQuaternion& quaternion, const std::string& name = "");
