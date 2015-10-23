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

protected:
  Object(const btTransform &transform, btScalar mass, btVector3 &inertia);

public:
  virtual ~Object();

protected:
  // Physical parameters.
  btScalar m_mass = 0;
  btVector3 m_inertia;
  btCollisionShape* m_collisionShape = nullptr;
  btRigidBody* m_rigidBody = nullptr;
  btDefaultMotionState* m_motionState = nullptr;
  btRigidBody::btRigidBodyConstructionInfo* m_constructionInfo = nullptr;

  // Shape parameters.
  std::vector<glm::vec3> m_points;
  std::vector<glm::vec3> m_normals;
  std::vector<unsigned int> m_indices;
  std::vector<glm::vec2> m_textureCoos;
  std::vector<glm::vec3> m_tangents;

  // Material colors.
  glm::vec4 m_ambientColor;
  glm::vec4 m_diffuseColor;
  glm::vec4 m_specularColor;
  float m_shininess = 0.f;
  std::string m_textureFile;
  std::string m_normalTextureFile;

  std::string m_name;

public:
  const float* getPoints() const;
  int getPointsNumber() const;

  const float* getNormals() const;
  const unsigned int* getIndices() const;

  const float* getTextureCoos() const;
  
  const float* getTangents() const;

  inline int getTrigsNumber() const {
    return m_indices.size() / 3;
  }
  inline int getIndicesNumber() const {
    return m_indices.size();
  }

  inline const glm::vec4 &getAmbientColor() const {
    return m_ambientColor;
  }
  void setAmbientColor(const glm::vec4 &ambientColor);

  inline const glm::vec4 &getDiffuseColor() const {
    return m_diffuseColor;
  }
  void setDiffuseColor(const glm::vec4 &color);

  inline const glm::vec4 &getSpecularColor() const {
    return m_specularColor;
  }
  void setSpecularColor(const glm::vec4 &color);

  inline float getShininess() const {
    return m_shininess;
  }
  void setShininess(float shininess);

  inline btScalar getMass() const { return m_mass; }
  void setMass(btScalar mass);

  inline const btVector3& getInertia() const { return m_inertia; }
  void setInertia(const btVector3& inertia);

  btCollisionShape* getCollisionShape() const;
  void setCollisionShape(btCollisionShape* collisionShape);

  inline btRigidBody* getRigidBody() const {
    return m_rigidBody;
  }
  void setRigidBody(btRigidBody* rigidBody);

  btDefaultMotionState* getMotionState() const;
  void setMotionState(btDefaultMotionState* motionState);

  btRigidBody::btRigidBodyConstructionInfo* 
               getConstructionInfo() const;
  void setConstructionInfo(btRigidBody::btRigidBodyConstructionInfo* 
                           constructionInfo);

  inline void getOpenGLMatrix(btScalar* matrix) const {
    m_transform.getOpenGLMatrix(matrix);
  }

  inline const std::string &getTextureFile() const {
    return m_textureFile;
  }

  inline const std::string &getNormalTextureFile() const {
    return m_normalTextureFile;
  }
};

//-----------------------------------------------------------------------------
template <class Subtype>
class ObjectBuilder {
public:
  ObjectBuilder();
  virtual ~ObjectBuilder();

public:
  Subtype& setTransform(const btTransform& transform);
  Subtype& setMass(btScalar mass);
  Subtype& setInertia(const btVector3& inertia);

  Subtype& setAmbientColor(const glm::vec4& color);
  Subtype& setDiffuseColor(const glm::vec4& color);
  Subtype& setSpecularColor(const glm::vec4& color);
  Subtype& setShininess(float shininess);

  Subtype& setTextureFile(std::string textureFile);
  Subtype& setNormalTextureFile(std::string normalTextureFile);

  virtual Object* create() = 0;

protected:
  void setColors(Object* object);

protected:
  btTransform m_transform;
  btScalar m_mass = 0;
  btVector3 m_inertia;
  glm::vec4 m_ambientColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
  glm::vec4 m_diffuseColor = glm::vec4(0.f, 0.f, 0.f, 1.f);
  glm::vec4 m_specularColor = glm::vec4(0.f, 0.f, 0.f, 1.f);
  float m_shininess = 0.f;
  std::string m_textureFile;
  std::string m_normalTextureFile;
};
