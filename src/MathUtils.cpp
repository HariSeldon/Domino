#include "MathUtils.h"

#include <LinearMath/btMatrix3x3.h>
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btScalar.h>
#include <LinearMath/btTransform.h>
#include <LinearMath/btVector3.h>

#include <iostream>

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
std::string getPreamble(const std::string &name) {
  if (name.empty())
    return name;
  return name + ": ";
}

//------------------------------------------------------------------------------
std::string getVectorOnly(const btVector3 &vector) {
  return "(" + std::to_string(vector.x()) + ", " + std::to_string(vector.y()) +
         ", " + std::to_string(vector.z()) + ", " + "\n";
}

//------------------------------------------------------------------------------
std::string getVectorOnly(const btVector4 &vector) {
  return "(" + std::to_string(vector.x()) + ", " + std::to_string(vector.y()) +
         ", " + std::to_string(vector.z()) + ", " + std::to_string(vector.w()) +
         "\n";
}

//------------------------------------------------------------------------------
int signum(btScalar x) { return (x > btScalar(0.0)) - (x < btScalar(0.0)); }

//------------------------------------------------------------------------------
void dumpVector(const btVector3 &vector, const std::string &name) {
  std::cout << getPreamble(name) + getVectorOnly(vector) + "\n";
}

//------------------------------------------------------------------------------
void dumpVector(const btVector4 &vector, const std::string &name) {
  std::cout << getPreamble(name) + getVectorOnly(vector) + "\n";
}

//------------------------------------------------------------------------------
void dumpMatrix(const btMatrix3x3 &matrix, const std::string &name) {
  std::cout << getPreamble(name) + "\n" + getVectorOnly(matrix.getRow(0)) +
                   "\n" + getVectorOnly(matrix.getRow(1)) + "\n" +
                   getVectorOnly(matrix.getRow(2)) + "\n";
}

//------------------------------------------------------------------------------
void dumpOpenGLMatrix(const float *matrix, const std::string &name) {
  std::string result;
  result += getPreamble(name) + "\n";
  for (int row = 0; row < 4; ++row) {
    for (int column = 0; column < 4; ++column) {
      result += std::to_string(matrix[row + column * 4]) + "\t";
      if (column == 3)
        result += "\n";
    }
  }
  std::cout << result;
}
