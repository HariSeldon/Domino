#include "Box.h"

#include "MathUtils.h"

#include <LinearMath/btVector3.h>

#include <glm/mat4x4.hpp>
#include <glm/ext.hpp>
#include <iostream>

#include <algorithm>

template class ObjectBuilder<BoxBuilder>;

Box::Box(const btTransform &transform, const btScalar mass, btVector3 &inertia,
         const btVector3 &sides)
    : Object(transform, mass, inertia) {
  const btVector3 halfSides = sides / 2;
  setupFaces({ halfSides.x(), halfSides.y(), halfSides.z() });
  setupBulletShape(halfSides);
}

//------------------------------------------------------------------------------
void Box::setupFaces(const glm::vec3 &halfSides) {
  glm::vec3 frontTopLeft = glm::vec3(-1, 1, -1) * halfSides;
  glm::vec3 frontTopRight = glm::vec3(1, 1, -1) * halfSides;
  glm::vec3 frontBottomLeft = glm::vec3(-1, -1, -1) * halfSides;
  glm::vec3 frontBottomRight = glm::vec3(1, -1, -1) * halfSides;
  glm::vec3 backTopLeft = glm::vec3(-1, 1, 1) * halfSides;
  glm::vec3 backTopRight = glm::vec3(1, 1, 1) * halfSides;
  glm::vec3 backBottomLeft = glm::vec3(-1, -1, 1) * halfSides;
  glm::vec3 backBottomRight = glm::vec3(1, -1, 1) * halfSides;

  points = { frontTopRight,    frontBottomRight,
             frontBottomLeft,  frontTopLeft, // Front.
             backTopRight,     backBottomRight,
             frontBottomRight, frontTopRight, // Right.
             backTopLeft,      backBottomLeft,
             backBottomRight,  backTopRight, // Back.
             frontTopLeft,     frontBottomLeft,
             backBottomLeft,   backTopLeft, // Left.
             backTopRight,     frontTopRight,
             frontTopLeft,     backTopLeft, // Top.
             backBottomLeft,   frontBottomLeft,
             frontBottomRight, backBottomRight // Bottom.
  };

  textureCoos = { {1, 1}, {1, 0}, {0, 0}, {0, 1},
                  {1, 1}, {1, 0}, {0, 0}, {0, 1},
                  {1, 1}, {1, 0}, {0, 0}, {0, 1},
                  {1, 1}, {1, 0}, {0, 0}, {0, 1},
                  {1, 1}, {1, 0}, {0, 0}, {0, 1},
                  {1, 1}, {1, 0}, {0, 0}, {0, 1}
  };

  indices = { 0,  1,  2,  2,  3,  0,  // Front.
              4,  5,  6,  6,  7,  4,  // Right.
              8,  9,  10, 10, 11, 8,  // Back.
              12, 13, 14, 14, 15, 12, // Left.
              16, 17, 18, 18, 19, 16, // Top.
              20, 21, 22, 22, 23, 20  // Bottom.
  };

  glm::vec3 frontNormal =
      computeNormal(frontTopRight, frontBottomRight, frontBottomLeft);
  glm::vec3 rightNormal =
      computeNormal(backTopRight, backBottomRight, frontBottomRight);
  glm::vec3 backNormal =
      computeNormal(backTopLeft, backBottomLeft, backBottomRight);
  glm::vec3 leftNormal =
      computeNormal(frontTopLeft, frontBottomLeft, backBottomLeft);
  glm::vec3 topNormal =
      computeNormal(backTopRight, frontTopRight, frontTopLeft);
  glm::vec3 bottomNormal =
      computeNormal(backBottomLeft, frontBottomLeft, frontBottomRight);

  normals = { frontNormal,  frontNormal,  frontNormal,  frontNormal,
              rightNormal,  rightNormal,  rightNormal,  rightNormal,
              backNormal,   backNormal,   backNormal,   backNormal,
              leftNormal,   leftNormal,   leftNormal,   leftNormal,
              topNormal,    topNormal,    topNormal,    topNormal,
              bottomNormal, bottomNormal, bottomNormal, bottomNormal };

  glm::vec3 frontTangent {1, 0, 0};
  glm::vec3 rightTangent {0, 0, 1};
  glm::vec3 backTangent {-1, 0, 0};
  glm::vec3 leftTangent {0, 0, -1};
  glm::vec3 topTangent {1, 0, 0};
  glm::vec3 bottomTangent {-1, 0, 0};

//  tangents = { frontTangent, frontTangent, frontTangent, frontTangent,
//               rightTangent, rightTangent, rightTangent, rightTangent,     
//               backTangent, backTangent, backTangent, backTangent,
//               leftTangent, leftTangent, leftTangent, leftTangent,
//               topTangent, topTangent, topTangent, topTangent,
//               bottomTangent, bottomTangent, bottomTangent, bottomTangent
//    };

  // Compute the tangent vector for each triangle.
  for (auto index = 0u; index < indices.size() / 3; ++index) {
    auto point0 = points[indices[3 * index + 0]];
    auto point1 = points[indices[3 * index + 1]]; 
    auto point2 = points[indices[3 * index + 2]];

    auto tex0 = textureCoos[indices[3 * index + 0]];
    auto tex1 = textureCoos[indices[3 * index + 1]];
    auto tex2 = textureCoos[indices[3 * index + 2]];

    auto edge1 = point1 - point0;
    auto edge2 = point2 - point0;
    
    auto deltaTex1 = tex1 - tex0; 
    auto deltaTex2 = tex2 - tex0;

    glm::vec3 tangent = (edge1 * deltaTex2.y - edge2 * deltaTex1.y) /
                        (deltaTex1.x * deltaTex2.y - deltaTex1.y * deltaTex2.x);

    tangents.insert(tangents.end(), {tangent, tangent}); 
  }
}

//------------------------------------------------------------------------------
void Box::setupBulletShape(const btVector3 halfSides) {
  collisionShape = new btBoxShape(halfSides);
  collisionShape->calculateLocalInertia(mass, inertia);
  motionState = new btDefaultMotionState(transform);
  constructionInfo = new btRigidBody::btRigidBodyConstructionInfo(
      mass, motionState, collisionShape, inertia);
  rigidBody = new btRigidBody(*constructionInfo);
}

//------------------------------------------------------------------------------
BoxBuilder::BoxBuilder() : ObjectBuilder() {}

BoxBuilder &BoxBuilder::setSides(const btVector3 &sidesLengths) {
  this->sidesLengths = sidesLengths;
  return *this;
}

Box *BoxBuilder::create() {
  Box *box = new Box(transform, mass, inertia, sidesLengths);
  ObjectBuilder::setColors(box);
  box->textureFile = textureFile;
  box->normalTextureFile = normalTextureFile;
  return box;
}
