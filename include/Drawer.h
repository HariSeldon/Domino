#pragma once

#include <GL/glew.h>

#include <glm/fwd.hpp>

#include <map>
#include <memory>
#include <unordered_map>
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

class SDL_Surface;

class Drawer {
public:
  ~Drawer();

public:
  void
  initGPUObjects(const std::map<const std::string, std::vector<const Object *>> &shaderMap,
                 const World &world);
  void initTextures(const World& world);
  void initGPUShadowObjects(const ShaderProgram &shadowShader,
                            const World &world);
  void initMirror(const ShaderProgram &mirrorShader, const Mirror *mirror);

  // Drawing functions.
  void drawObjects(const World *world,
                   const glm::mat4 &originalModelView,
                   const glm::mat4 &projection,
                   const glm::mat4 &originalShadowModelView,
                   const glm::mat4 &shadowProjection,
                   int lightMask) const;
  void drawObject(const Object *object, ShaderProgram &shader,
                  const glm::mat4 &originalModelView,
                  const glm::mat4 &projection,
                  const glm::mat4 &originalShadowModelView,
                  const glm::mat4 &shadowProjection) const; 
  void drawObjectForShadow(const Object *object, ShaderProgram &shader,
                           const glm::mat4 &originalModelView,
                           const glm::mat4 &projection) const;
  void setLights(const World *world, ShaderProgram *shader,
                 const glm::mat4 &modelView, int lightMask) const;

private:
  GLuint setupVertexVBO(const Object *object, const ShaderProgram &shader);
  GLuint setupNormalVBO(const Object *object, const ShaderProgram &shader);
  GLuint setupIndexVBO(const Object *object);
  GLuint setupTextureVBO(const Object *object, const ShaderProgram &shader);

private:
  // Mapping between world objects and shaders.
  std::map<ShaderProgram *, std::vector<const Object*>> shaderMap;
  // Mapping between world objects and VAOs.
  std::unordered_map<const Object *, GLuint> vaoWorldMap;
  // Mapping between world objects shadows and VAOs.
  std::unordered_map<const Object *, GLuint> vaoShadowMap;
  // Mapping between world objects and their vertex VBO.
  std::unordered_map<const Object *, GLuint> vboWorldMap;
  // Mapping between world objects and their texture objects.
  std::unordered_map<const Object *, GLuint> textureMap;

  // Ids of VBOs associated with the VAOs. These are kept so I know what to delete
  // to free the memory.
  std::vector<GLuint> vboIds;
};
