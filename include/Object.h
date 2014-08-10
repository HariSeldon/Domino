#pragma once

#include "Entity.h"

#include <btBulletDynamicsCommon.h>
#include <LinearMath/btScalar.h>

#include <glm/fwd.hpp>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

#include <string>
#include <vector>

class btVector3;

class Drawer;
class ShaderProgram;

class Object : public Entity {
protected:
  Object(btTransform &transform, btScalar mass, btVector3 &inertia);

public:
  virtual ~Object();

protected:
  // Physical parameters.
  btScalar mass;
  btVector3 inertia;
  btCollisionShape* collisionShape;
  btRigidBody* rigidBody;
  btDefaultMotionState* motionState;
  btRigidBody::btRigidBodyConstructionInfo* constructionInfo;

  // Shape parameters.
  std::vector<glm::vec3> points;
  std::vector<glm::vec3> normals;
  std::vector<unsigned int> indices;
  std::vector<glm::vec2> textureCoos;

  // Material colors.
  glm::vec4 ambientColor;
  glm::vec4 diffuseColor;
  glm::vec4 specularColor;
  float shininess;

  std::string name;

public:
  const float* getPoints() const;
  int getPointsNumber() const;

  const float* getNormals() const;
  const unsigned int* getIndices() const;

  const float* getTextureCoos() const;
  
  int getTrigsNumber() const;
  int getIndicesNumber() const;

  const glm::vec4 &getAmbientColor() const;
  void setAmbientColor(const glm::vec4 &ambientColor);

  const glm::vec4 &getDiffuseColor() const;
  void setDiffuseColor(const glm::vec4 &color);

  const glm::vec4 &getSpecularColor() const;
  void setSpecularColor(const glm::vec4 &color);

  float getShininess() const;
  void setShininess(float shininess);

  btScalar getMass() const;
  void setMass(btScalar mass);

  const btVector3& getInertia() const;
  void setInertia(const btVector3& inertia);

  btCollisionShape* getCollisionShape() const;
  void setCollisionShape(btCollisionShape* collisionShape);

  btRigidBody* getRigidBody() const;
  void setRigidBody(btRigidBody* rigidBody);

  btDefaultMotionState* getMotionState() const;
  void setMotionState(btDefaultMotionState* motionState);

  btRigidBody::btRigidBodyConstructionInfo* 
               getConstructionInfo() const;
  void setConstructionInfo(btRigidBody::btRigidBodyConstructionInfo* 
                           constructionInfo);

  void getOpenGLMatrix(btScalar* matrix) const;
};

//-----------------------------------------------------------------------------
template <class Subtype>
class ObjectBuilder {
public:
  ObjectBuilder();

public:
  Subtype& setTransform(const btTransform& transform);
  Subtype& setMass(btScalar mass);
  Subtype& setInertia(const btVector3& inertia);

  Subtype& setAmbientColor(const glm::vec4& color);
  Subtype& setDiffuseColor(const glm::vec4& color);
  Subtype& setSpecularColor(const glm::vec4& color);
  Subtype& setShininess(float shininess);

  virtual Object* create() = 0;

protected:
  void setColors(Object* object);

protected:
  btTransform transform;
  btScalar mass;
  btVector3 inertia;
  glm::vec4 ambientColor;
  glm::vec4 diffuseColor;
  glm::vec4 specularColor;
  float shininess;
};
