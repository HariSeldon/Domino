#include "MathUtils.h"

#include <LinearMath/btMatrix3x3.h>
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btScalar.h>
#include <LinearMath/btTransform.h>
#include <LinearMath/btVector3.h>

#include <cmath>
#include <iostream>
#include <sstream>

//------------------------------------------------------------------------------
btScalar degreesToRadiants(btScalar degrees) { return M_PI * degrees / 180.0f; }

//------------------------------------------------------------------------------
btScalar clampToDegrees(btScalar value) {
  if (value >= btScalar(360.0))
    return value - btScalar(360.0);
  else if (value < btScalar(-360))
    return value + btScalar(360.0);
  else
    return value;
}

//------------------------------------------------------------------------------
std::string getPreamble(const std::string& name) {
  if (name.empty()) return name;

  return std::string(name + ": ");
}

//------------------------------------------------------------------------------
std::string getVectorOnly(const btVector3& vector) {
  std::stringstream ss;
  ss << "(" << vector.x() << ", " << vector.y() << ", " << vector.z() << ")";
  return ss.str();
}

//------------------------------------------------------------------------------
std::string getVectorOnly(const btVector4& vector) {
  std::stringstream ss;
  ss << "(" << vector.x() << ", " << vector.y() << ", " << vector.z() << ", "
     << vector.w() << ")";
  return ss.str();
}

//------------------------------------------------------------------------------
int signum(btScalar x) { return (x > btScalar(0.0)) - (x < btScalar(0.0)); }

//------------------------------------------------------------------------------
void dumpVector(const btVector3& vector, const std::string& name) {
  std::string result(getPreamble(name) + getVectorOnly(vector) + "\n");
  std::cout << result;
}

//------------------------------------------------------------------------------
void dumpVector(const btVector4& vector, const std::string& name) {
  std::string result(getPreamble(name) + getVectorOnly(vector) + "\n");
  std::cout << result;
}

//------------------------------------------------------------------------------
void dumpMatrix(const btMatrix3x3& matrix, const std::string& name) {
  std::string result(getPreamble(name) + "\n" +
                     getVectorOnly(matrix.getRow(0)) + "\n" +
                     getVectorOnly(matrix.getRow(1)) + "\n" +
                     getVectorOnly(matrix.getRow(2)) + "\n");
  std::cout << result;
}

//------------------------------------------------------------------------------
void dumpOpenGLMatrix(const float* matrix, const std::string& name) {
  std::stringstream ss;
  ss << getPreamble(name) << "\n";
  for (int row = 0; row < 4; ++row) {
    for (int column = 0; column < 4; ++column) {
      ss << matrix[row + column * 4] << "\t";
      if (column == 3) ss << "\n";
    }
  }
  std::cout << ss.str();
}
