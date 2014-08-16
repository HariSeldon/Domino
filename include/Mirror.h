#pragma once

#include "Object.h"
#include "ShaderProgram.h"
#include "SysUtils.h"

#include <glm/fwd.hpp>

#include <GL/glew.h>

#include <LinearMath/btScalar.h>

class btVector3;

class Mirror : public Object {
public:
  Mirror(btTransform &transform, btScalar mass, btVector3 &inertia,
         const btScalar &side);

public:
  void enableMirror() const;
  void disableMirror() const;
   
  GLuint getTextureId() const;

  ShaderProgram *getShaderProgram(); 

  glm::mat4 getModelView();

private:
  void computePoints(const btScalar &side);
  void setupBulletShape();
  void createFBO();
  void createDBO();
  void createMirrorTexture();
  void attachTexture();

private:
  GLuint fboId;
  GLuint dboId;
  GLuint mirrorTexture;
  ShaderProgram *shader;
    

  friend class MirrorBuilder;
};

//-----------------------------------------------------------------------------
class MirrorBuilder : public ObjectBuilder<MirrorBuilder> {
public:
  MirrorBuilder();

  MirrorBuilder &setSide(btScalar side);

  Mirror *create();

private:
  btScalar side;
};
