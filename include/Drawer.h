#pragma once

#include <GL/glew.h>

#include <glm/fwd.hpp>

#include <unordered_map>
#include <memory>
#include <vector>

class Box;
class Object;
class Mirror;
class Plane;
class ShaderProgram;
class Sphere;
class Wedge;
class World;

class btVector3;

class Drawer {
public:
  ~Drawer();

public:
  void initGPUObjects(const ShaderProgram &worldShader, const World &world);
  void initGPUShadowObjects(const ShaderProgram &shadowShader,
                            const World &world);
  void initMirror(const ShaderProgram &mirrorShader, const Mirror *mirror);

  // Drawing functions.
  void drawObject(const Object *object, ShaderProgram &shader,
                  const glm::mat4 &originalModelView,
                  const glm::mat4 &projection,
                  const glm::mat4 &originalShadowModelView,
                  const glm::mat4 &shadowProjection) const; 
  void drawObjectForShadow(const Object *object, ShaderProgram &shader,
                           const glm::mat4 &originalModelView,
                           const glm::mat4 &projection) const;

private:
  GLuint setupVertexVBO(const Object *object, const ShaderProgram &shader);
  GLuint setupNormalVBO(const Object *object, const ShaderProgram &shader);
  GLuint setupIndexVBO(const Object *object);
  GLuint setupTextureVBO(const Object *object, const ShaderProgram &shader);

private:
  // Mapping between world objects (identified by their memory locations) 
  // to VAOs. 
  std::unordered_map<intptr_t, GLuint> vaoWorldMap;
  // Mapping between world objects shadows (identified by their memory locations) 
  // to VAOs.
  std::unordered_map<intptr_t, GLuint> vaoShadowMap;
  // Mapping between world objects (identified by their memory locations)
  // to their vertex VBO.
  std::unordered_map<intptr_t, GLuint> vboWorldMap; 

  // Ids of VBOs associated with the VAOs. These are kept so I know what to delete
  // to free the memory.
  std::vector<GLuint> vboIds;
};
