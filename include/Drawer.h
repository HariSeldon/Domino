#pragma once

#include "BlurShader.h"
#include "CanvasShader.h"
#include "LightBulbShader.h"
#include "MirrorShader.h"
#include "PhongShader.h"
#include "PhongNormalMappingShader.h"
#include "TextureManager.h"    

#include <GL/glew.h>

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>

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
  Drawer(const glm::ivec2 screenSize);
  ~Drawer();

public:
  void initGPUObjects(const std::map<const std::string,
                                     std::vector<const Object *>> &shaderMap,
                      const Mirror *mirror);
  void initTextures(const World &world);
  void initGPUShadowObjects(const ShaderProgram &shadowShader,
                            const World &world);
  void initMirror(const Mirror *mirror);

  // Drawing functions.
  void drawWorld(const World *world, const glm::mat4 &originalModelView,
                 const glm::mat4 &projection,
                 const glm::mat4 &originalShadowModelView,
                 const glm::mat4 &shadowProjection, const int lightMask,
                 const glm::vec4 &cameraPosition) const;
  void drawWorldForMirror(const World *world,
                          const glm::mat4 &originalModelView,
                          const glm::mat4 &projection,
                          const glm::mat4 &originalShadowModelView,
                          const glm::mat4 &shadowProjection,
                          const int lightMask,
                          const glm::vec4 &cameraPosition) const;

  void drawObjectForShadow(const Object *object, ShaderProgram &shader,
                           const glm::mat4 &originalModelView,
                           const glm::mat4 &projection) const;
  inline void enableMirror() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_mirrorFBO);
    checkOpenGLError("Drawer: enableMirror-glBindFrameBuffer");
    glViewport(0, 0, m_screenSize.x, m_screenSize.y);
    checkOpenGLError("Drawer: enableMirror-glViewport");
  }
  void drawMirror(const glm::mat4 &originalModelView,
                  const glm::mat4 &projection) const;
  inline void disableMirror() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    checkOpenGLError("Drawer: disableMirror-glBindFramebuffer");
  }

private:
  void fillObjectsVectors(const std::map<
      const std::string, std::vector<const Object *>> &shaderNameMap);
  void createGPUBuffers();

  void createLightBulbsGPUBuffers();
  void createLightBulbGPUBuffers(const Object *object);
  void createCanvasBuffer();

  void createPhongObjectsGPUBuffers();
  void createPhongObjectGPUBuffers(const Object *object);

  void createPhongNormalMappingObjectsGPUBuffers();
  void createPhongNormalMappingObjectGPUBuffers(const Object *object);

  void createMirrorObjectGPUBuffers();

  void createObjectTextures(const Object *object);
  void createMirrorObjects();
  void createFrameBuffers();

  GLuint setupVertexVBO(const Object *object, const ShaderProgram &shader);
  GLuint setupNormalVBO(const Object *object, const ShaderProgram &shader);
  GLuint setupIndexVBO(const Object *object);
  GLuint setupTextureVBO(const Object *object, const ShaderProgram &shader);
  GLuint setupTangentVBO(const Object *object, const ShaderProgram &shader);

  void invokeDrawCall(const Object *object) const;

  void drawNonReflectiveObjects(const World *world,
                                const glm::mat4 &originalModelView,
                                const glm::mat4 &projection,
                                const glm::mat4 &originalShadowModelView,
                                const glm::mat4 &shadowProjection,
                                const int lightMask,
                                const glm::vec4 &cameraPosition) const;
  void drawPhongObjects(const World *world, 
                        const glm::mat4 &originalModelView,
                        const glm::mat4 &projection,
                        const glm::mat4 &originalShadowModelView,
                        const glm::mat4 &shadowProjection,
                        const int lightMask) const;

  void drawPhongNormalMappingObjects(const World *world, 
                                     const glm::mat4 &originalModelView,
                                     const glm::mat4 &projection,
                                     const glm::mat4 &originalShadowModelView,
                                     const glm::mat4 &shadowProjection,
                                     const int lightMask) const;

  static void setPhongLights(const World *world, const PhongShader &shader,
                             const glm::mat4 &modelView, const int lightMask);
  static void setPhongLights(const World *world,
                             const PhongNormalMappingShader &shader,
                             const glm::mat4 &modelView, const int lightMask);

  void drawPhongObject(const Object *object,
                       const glm::mat4 &originalModelView,
                       const glm::mat4 &projection,
                       const glm::mat4 &originalShadowModelView,
                       const glm::mat4 &shadowProjection) const;

  void drawPhongNormalMappingObject(const Object *object,
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
  std::vector<const Object *> m_lightBulbs;
  std::vector<const Object *> m_phongObjects;
  std::vector<const Object *> m_phongNormalMappingObjects;
  const Mirror *m_mirror = nullptr;

  glm::ivec2 m_screenSize;

  LightBulbShader m_lightBulbShader;
  PhongShader m_phongShader;
  PhongNormalMappingShader m_phongNormalShader; 
  CanvasShader m_canvasShader;
  BlurShader m_blurShader;
  MirrorShader m_mirrorShader;

  TextureManager m_textureManager;

  // Mapping between shaders and world objects.
  // Mapping between world objects and VAOs.
  std::unordered_map<const Object *, GLuint> m_vaoWorldMap;
  // Mapping between world objects shadows and VAOs.
  std::unordered_map<const Object *, GLuint> m_vaoShadowMap;
  // Mapping between world objects and their vertex VBO.
  std::unordered_map<const Object *, GLuint> m_vboWorldMap;
  // Mapping between world objects and their texture objects.
  std::unordered_map<const Object *, GLuint> m_textureMap;
  // Mapping between world objects and their normal texture objects.
  std::unordered_map<const Object *, GLuint> m_normalTextureMap;

  // Ids of VBOs associated with the VAOs. These are kept so I know what to
  // delete to free the memory.
  std::vector<GLuint> m_vboIds;

  GLuint m_mirrorFBO = 0;
  GLuint m_mirrorTexture = 0;
  GLuint m_mirrorDBO = 0;

  GLuint m_sceneFBOId = 0;
  GLuint m_sceneTexture = 0;

  GLuint m_lightBulbFBOId = 0;
  GLuint m_lightBulbTexture = 0;

  GLuint m_bulbFBOId = 0;
  GLuint m_bulbTexture = 0;

  GLuint m_blurredBulbFBOId = 0;
  GLuint m_blurredBulbTexture = 0;

  GLuint m_dboId = 0;

  GLuint m_canvasId = 0;
  GLuint m_stencilId = 0;
};
