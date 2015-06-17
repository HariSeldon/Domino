#pragma once

#include "Object.h"
#include "ShaderProgram.h"
#include "SysUtils.h"

#include <GL/glew.h>

#include <glm/vec2.hpp>

#include <LinearMath/btScalar.h>

class btVector3;

class Mirror : public Object {
public:
  Mirror(btTransform &transform, btScalar mass, btVector3 &inertia,
         const glm::vec2 &sides);

public:
  glm::mat4 getModelView() const;
  inline glm::vec2 getSize() const {
    return sides;
  }

private:
  void computePoints(const glm::vec2 &sides);
  void setupBulletShape();

private:
  const glm::vec2 sides;
    
  friend class MirrorBuilder;
};

//-----------------------------------------------------------------------------
class MirrorBuilder : public ObjectBuilder<MirrorBuilder> {
public:
  MirrorBuilder();

  MirrorBuilder &setSides(const glm::vec2 &sides);

  Mirror *create();

private:
  glm::vec2 sides;
};
