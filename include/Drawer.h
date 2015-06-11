#pragma once

#include "BlurShader.h"
#include "CanvasShader.h"
#include "LightBulbShader.h"
#include "PhongShader.h"

#include <GL/glew.h>

#include <glm/fwd.hpp>

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

class Box;
class LightBulb;
class LightedObjectShader;
class Mirror;
class Object;
class Plane;
class ShaderProgram;
class Sphere;
class Wedge;
class World;

class Drawer {
public:
  Drawer();
  ~Drawer();

public:
  void initGPUObjects(const std::map<const std::string,
                                     std::vector<const Object *>> &shaderMap);
  void initTextures(const World &world);
  void initGPUShadowObjects(const ShaderProgram &shadowShader,
                            const World &world);
  void initMirror(const ShaderProgram &mirrorShader, const Mirror *mirror);

  // Drawing functions.
  void drawWorld(const World *world, const glm::mat4 &originalModelView,
                 const glm::mat4 &projection,
                 const glm::mat4 &originalShadowModelView,
                 const glm::mat4 &shadowProjection, const int lightMask,
                 const glm::vec4 &cameraPosition) const;

  void drawObjectForShadow(const Object *object, ShaderProgram &shader,
                           const glm::mat4 &originalModelView,
                           const glm::mat4 &projection) const;
private:
  void fillObjectsVectors(const std::map<
      const std::string, std::vector<const Object *>> &shaderNameMap);
  void createGPUBuffers();

  void createLightBulbsGPUBuffers();
  void createLightBulbGPUBuffers(const Object *object);

  void createPhongObjectsGPUBuffers();
  void createPhongObjectGPUBuffers(const Object *object);

  GLuint setupVertexVBO(const Object *object, const ShaderProgram &shader);
  GLuint setupNormalVBO(const Object *object, const ShaderProgram &shader);
  GLuint setupIndexVBO(const Object *object);
  GLuint setupTextureVBO(const Object *object, const ShaderProgram &shader);

  void invokeDrawCall(const Object *object) const;

  void drawPhongObjects(const World *world, 
                        const glm::mat4 &originalModelView,
                        const glm::mat4 &projection,
                        const glm::mat4 &originalShadowModelView,
                        const glm::mat4 &shadowProjection,
                        const int lightMask) const;

  static void setPhongLights(const World *world, const PhongShader &shader,
                             const glm::mat4 &modelView, const int lightMask);

  void drawPhongObject(const Object *object,
                       const glm::mat4 &originalModelView,
                       const glm::mat4 &projection,
                       const glm::mat4 &originalShadowModelView,
                       const glm::mat4 &shadowProjection) const;

  void drawLightBulbs(const glm::mat4 &originalModelView,
                      const glm::mat4 &projection,
                      const glm::vec4 &cameraPosition,
                      const int lightMask) const;

  void drawLightBulb(const Object *lightBulb,
                     const glm::mat4 &originalModelView,
                     const glm::mat4 &projection,
                     const glm::vec4 &cameraPosition) const;

  void blurLightBulbs(const BlurShader &blurShader,
                      const GLuint outputFrameBuffer,
                      const GLuint inputTexture) const;
  void drawFinalImage() const;

private:
  std::vector<const Object *> lightBulbs;
  std::vector<const Object *> phongObjects;

  LightBulbShader lightBulbShader;
  PhongShader phongShader;
  CanvasShader canvasShader;
  BlurShader blurShader;

  // Mapping between shaders and world objects.
  // Mapping between world objects and VAOs.
  std::unordered_map<const Object *, GLuint> vaoWorldMap;
  // Mapping between world objects shadows and VAOs.
  std::unordered_map<const Object *, GLuint> vaoShadowMap;
  // Mapping between world objects and their vertex VBO.
  std::unordered_map<const Object *, GLuint> vboWorldMap;
  // Mapping between world objects and their texture objects.
  std::unordered_map<const Object *, GLuint> textureMap;

  // Ids of VBOs associated with the VAOs. These are kept so I know what to
  // delete
  // to free the memory.
  std::vector<GLuint> vboIds;

  // Temporary stuff.
  GLuint sceneFBOId;
  GLuint sceneTexture;

  GLuint lightBulbFBOId;
  GLuint lightBulbTexture;

  GLuint bulbFBOId;
  GLuint bulbTexture;

  GLuint blurredBulbFBOId;
  GLuint blurredBulbTexture;

  GLuint dboId;

  //ShaderProgram *basicShader;
  GLuint canvasId;
  GLuint stencilId;
};
