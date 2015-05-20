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
public:
  static const glm::vec3 DEFAULT_POSITION;
  static const glm::vec3 DEFAULT_ROTATION;
  static const glm::vec4 DEFAULT_AMBIENT_COLOR;
  static const glm::vec4 DEFAULT_DIFFUSE_COLOR;
  static const glm::vec4 DEFAULT_SPECULAR_COLOR;
  static constexpr float DEFAULT_MASS = 0.f;

protected:
  Object(const btTransform &transform, btScalar mass, btVector3 &inertia);

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
  std::string textureFile;

  std::string name;

public:
  const float* getPoints() const;
  int getPointsNumber() const;

  const float* getNormals() const;
  const unsigned int* getIndices() const;

  const float* getTextureCoos() const;
  
  inline int getTrigsNumber() const {
    return indices.size() / 3;
  }
  inline int getIndicesNumber() const {
    return indices.size();
  }

  inline const glm::vec4 &getAmbientColor() const {
    return ambientColor;
  }
  void setAmbientColor(const glm::vec4 &ambientColor);

  inline const glm::vec4 &getDiffuseColor() const {
    return diffuseColor;
  }
  void setDiffuseColor(const glm::vec4 &color);

  inline const glm::vec4 &getSpecularColor() const {
    return specularColor;
  }
  void setSpecularColor(const glm::vec4 &color);

  inline float getShininess() const {
    return shininess;
  }
  void setShininess(float shininess);

  inline btScalar getMass() const { return mass; }
  void setMass(btScalar mass);

  inline const btVector3& getInertia() const { return inertia; }
  void setInertia(const btVector3& inertia);

  btCollisionShape* getCollisionShape() const;
  void setCollisionShape(btCollisionShape* collisionShape);

  inline btRigidBody* getRigidBody() const {
    return rigidBody;
  }
  void setRigidBody(btRigidBody* rigidBody);

  btDefaultMotionState* getMotionState() const;
  void setMotionState(btDefaultMotionState* motionState);

  btRigidBody::btRigidBodyConstructionInfo* 
               getConstructionInfo() const;
  void setConstructionInfo(btRigidBody::btRigidBodyConstructionInfo* 
                           constructionInfo);

  inline void getOpenGLMatrix(btScalar* matrix) const {
    transform.getOpenGLMatrix(matrix);
  }

  const std::string &getTextureFile() const {
    return textureFile;
  }
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

  Subtype& setTextureFile(std::string textureFile);

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
  std::string textureFile;
};
