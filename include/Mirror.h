#pragma once

#include "Box.h"
#include "ShaderProgram.h"
#include "SysUtils.h"

#include <GL/glew.h>

#include <glm/vec2.hpp>

#include <LinearMath/btScalar.h>

class btVector3;

class Mirror : public Box {
public:
  Mirror(btTransform &transform, btScalar mass, btVector3 &inertia,
         const btVector3 &sides);

public:
  glm::mat4 computeMirrorView() const;
  glm::vec3 computeNormal() const;
  inline glm::vec2 getSize() const {
    return sides;
  }

private:
  const glm::vec2 sides;
    
  friend class MirrorBuilder;
};

//-----------------------------------------------------------------------------
class MirrorBuilder : public ObjectBuilder<MirrorBuilder> {
public:
  MirrorBuilder();

  MirrorBuilder &setSides(const btVector3 &sides);

  Mirror *create();

private:
  btVector3 sides;
};
