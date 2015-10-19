#include "Drawer.h"

#include "Box.h"
#include "Light.h"
#include "LightedObjectShader.h"
#include "MathUtils.h"
#include "Mirror.h"
#include "Object.h"
#include "Plane.h"
#include "SysDefines.h"
#include "World.h"

#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/ext.hpp>

#include <algorithm>
#include <iostream>

//-----------------------------------------------------------------------------
void setColors(const Object *object, const PhongShader &shader);
void setColors(const Object *object, const PhongNormalMappingShader &shader);
void setOrientation(const Object *object, const PhongShader &shader,
                    const glm::mat4 &originalModelView,
                    const glm::mat4 &projection,
                    const glm::mat4 &originalShadowModelView,
                    const glm::mat4 &shadowProjection);
void setOrientation(const Object *object,
                    const PhongNormalMappingShader &shader,
                    const glm::mat4 &originalModelView,
                    const glm::mat4 &projection,
                    const glm::mat4 &originalShadowModelView,
                    const glm::mat4 &shadowProjection);
void setLightBulbOrientation(const Object *object,
                             const LightBulbShader &shader,
                             const glm::mat4 &originalModelView,
                             const glm::mat4 &projection,
                             const glm::vec4 &cameraPosition);
void setOrientationForShadow(const Object *object, const ShaderProgram &shader,
                             const glm::mat4 &originalModelView,
                             const glm::mat4 &projection);
glm::mat4 getObjectModelView(const Object *object,
                             const glm::mat4 &originalModelView);
// Returns the fboId and the textureId.
std::pair<GLuint, GLuint> generateFBOColor(const glm::ivec2 screenSize);
GLuint createDBO(GLuint fboId, const glm::ivec2 screenSize);

//-----------------------------------------------------------------------------
Drawer::Drawer(glm::ivec2 screenSize)
    : m_screenSize(screenSize),
      m_lightBulbShader("lightBulb.vert", "lightBulb.frag"),
      m_phongShader("phong.vert", "phong.frag"),
      m_phongNormalShader("phong_normal_mapping.vert",
                          "phong_normal_mapping.frag"),
      m_canvasShader("canvas.vert", "canvas.frag"),
      m_blurShader("blur.vert", "blur.frag"),
      m_mirrorShader("mirror.vert", "mirror.frag") {}

//-----------------------------------------------------------------------------
Drawer::~Drawer() {
  for (auto &iter : m_vaoWorldMap)
    glDeleteVertexArrays(1, &iter.second);

  if (m_vboIds.size() > 0)
    glDeleteBuffers(m_vboIds.size(), m_vboIds.data());

  for (auto &iter : m_textureMap) {
    glDeleteTextures(1, &iter.second);
  } 

  for (auto &iter : m_normalTextureMap) {
    glDeleteTextures(1, &iter.second);
  }

  if(m_mirrorFBO != 0) {
    glDeleteFramebuffers(1, &m_mirrorFBO);
    glDeleteTextures(1, &m_mirrorTexture);
    glGenRenderbuffers(1, &m_mirrorDBO);
  }
}

//-----------------------------------------------------------------------------
void Drawer::initGPUObjects(const std::map<
    const std::string, std::vector<const Object *>> &shaderNameMap,
    const Mirror *mirror) {
  m_mirror = mirror;
  fillObjectsVectors(shaderNameMap);
  createGPUBuffers();
}

//-----------------------------------------------------------------------------
void Drawer::fillObjectsVectors(const std::map<
    const std::string, std::vector<const Object *>> &shaderNameMap) {
  for (const auto &x : shaderNameMap) {
    const std::string shaderName = x.first;
    auto objectVector = x.second;

    if (shaderName == "phong") {
      m_phongObjects = objectVector;
    }

    if (shaderName == "lightBulb") {
      m_lightBulbs = objectVector;
    }

    if (shaderName == "phongNormalMapping") {
      m_phongNormalMappingObjects = objectVector;
    }
  }
}

//-----------------------------------------------------------------------------
void Drawer::createGPUBuffers() {
  createLightBulbsGPUBuffers();
  createPhongObjectsGPUBuffers();
  createPhongNormalMappingObjectsGPUBuffers();
  createMirrorObjectGPUBuffers();
}

//-----------------------------------------------------------------------------
void Drawer::createLightBulbsGPUBuffers() {
  for (auto& object : m_lightBulbs)
    createLightBulbGPUBuffers(object);
}

//-----------------------------------------------------------------------------
void Drawer::createCanvasBuffer() {
  // Setup canvas.
  glGenVertexArrays(1, &m_canvasId);
  glBindVertexArray(m_canvasId);

  // Setup vertices.
  std::vector<glm::vec2> canvasPoints = {{-1, 1}, {1, 1}, {1, -1}, {-1, -1}};
  GLuint canvasVertexVBOId = 0;
  glGenBuffers(1, &canvasVertexVBOId);
  glBindBuffer(GL_ARRAY_BUFFER, canvasVertexVBOId);
  glBufferData(GL_ARRAY_BUFFER, canvasPoints.size() * sizeof(glm::vec2),
               canvasPoints.data(), GL_STATIC_DRAW);
  m_canvasShader.setAttribute("vertexPosition", 2, GL_FLOAT);

  // Setup indices.
  std::vector<unsigned int> canvasIndices = {0, 1, 2, 2, 3, 0};
  GLuint canvasIndexVBOId = 0;
  glGenBuffers(1, &canvasIndexVBOId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, canvasIndexVBOId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               canvasIndices.size() * sizeof(unsigned int),
               canvasIndices.data(), GL_STATIC_DRAW);

  glBindVertexArray(0);
  m_vboIds.insert(m_vboIds.end(), {canvasVertexVBOId, canvasIndexVBOId});
}

//-----------------------------------------------------------------------------
void Drawer::createPhongObjectsGPUBuffers() {
  for (auto &object : m_phongObjects)
    createPhongObjectGPUBuffers(object);
}

//-----------------------------------------------------------------------------
void Drawer::createPhongNormalMappingObjectsGPUBuffers() {
  for (auto &object : m_phongNormalMappingObjects)
    createPhongNormalMappingObjectGPUBuffers(object);
}

//-----------------------------------------------------------------------------
void Drawer::createMirrorObjectGPUBuffers() {
  if(m_mirror == nullptr)
    return;

  GLuint vaoId = 0;
  // Create VAO.
  glGenVertexArrays(1, &vaoId);
  glBindVertexArray(vaoId);

  GLuint vertexVBOId = setupVertexVBO(m_mirror, m_mirrorShader);
  GLuint indexVBOId = setupIndexVBO(m_mirror);
  GLuint normalVBOId = setupNormalVBO(m_mirror, m_mirrorShader);

  // Unbind.
  glBindVertexArray(0);

  m_vaoWorldMap.insert(std::pair<const Object *, GLuint>(m_mirror, vaoId));
  m_vboIds.insert(m_vboIds.end(), {vertexVBOId, indexVBOId, normalVBOId});
}

//-----------------------------------------------------------------------------
void Drawer::createLightBulbGPUBuffers(const Object *object) {
  GLuint vaoId = 0;
  // Create VAO.
  glGenVertexArrays(1, &vaoId);
  glBindVertexArray(vaoId);

  GLuint vertexVBOId = setupVertexVBO(object, m_lightBulbShader);
  GLuint indexVBOId = setupIndexVBO(object);

  // Unbind.
  glBindVertexArray(0);

  m_vaoWorldMap.insert(std::pair<const Object *, GLuint>(object, vaoId));
  m_vboIds.insert(m_vboIds.end(), {vertexVBOId, indexVBOId});
}

//-----------------------------------------------------------------------------
void Drawer::createPhongObjectGPUBuffers(const Object *object) {
  GLuint vaoId = 0;
  // Create VAO.
  glGenVertexArrays(1, &vaoId);
  glBindVertexArray(vaoId);

  GLuint vertexVBOId = setupVertexVBO(object, m_phongShader);
  GLuint indexVBOId = setupIndexVBO(object);
  GLuint normalVBOId = setupNormalVBO(object, m_phongShader);
  GLuint textureVBOId = setupTextureVBO(object, m_phongShader);

  // Unbind.
  glBindVertexArray(0);

  m_vaoWorldMap.insert(std::pair<const Object *, GLuint>(object, vaoId));
  m_vboIds.insert(m_vboIds.end(),
                  {vertexVBOId, indexVBOId, normalVBOId, textureVBOId});
}

//-----------------------------------------------------------------------------
void Drawer::createPhongNormalMappingObjectGPUBuffers(const Object *object) {
  GLuint vaoId = 0;
  // Create VAO.
  glGenVertexArrays(1, &vaoId);
  glBindVertexArray(vaoId);

  GLuint vertexVBOId = setupVertexVBO(object, m_phongNormalShader);
  GLuint indexVBOId = setupIndexVBO(object);
  GLuint normalVBOId = setupNormalVBO(object, m_phongNormalShader);
  GLuint textureVBOId = setupTextureVBO(object, m_phongNormalShader);

  // Unbind.
  glBindVertexArray(0);

  m_vaoWorldMap.insert(std::pair<const Object *, GLuint>(object, vaoId));
  m_vboIds.insert(m_vboIds.end(),
                  {vertexVBOId, indexVBOId, normalVBOId, textureVBOId});
}

//-----------------------------------------------------------------------------
void Drawer::initTextures(const World &world) {
  std::for_each(constBeginObjects(world), constEndObjects(world),
                [&](const Object *object) { createObjectTextures(object); });
  if(m_mirror != nullptr)
    createMirrorObjects();
}

//-----------------------------------------------------------------------------
void Drawer::createFrameBuffers() {
  auto lightBulbData = generateFBOColor(m_screenSize);
  m_lightBulbFBOId = std::get<0>(lightBulbData);
  m_lightBulbTexture = std::get<1>(lightBulbData);

  auto bulbData = generateFBOColor(m_screenSize);
  m_bulbFBOId = std::get<0>(bulbData);
  m_bulbTexture = std::get<1>(bulbData);

  auto finalLightBulbData = generateFBOColor(m_screenSize);
  m_blurredBulbFBOId = std::get<0>(finalLightBulbData);
  m_blurredBulbTexture = std::get<1>(finalLightBulbData);

  auto sceneData = generateFBOColor(m_screenSize);
  m_sceneFBOId = std::get<0>(sceneData);
  m_sceneTexture = std::get<1>(sceneData);

  m_dboId = createDBO(m_sceneFBOId, m_screenSize);
}

//-----------------------------------------------------------------------------
void Drawer::createObjectTextures(const Object *object) {
  auto textureFile = object->getTextureFile();
  auto normalTextureFile = object->getNormalTextureFile();

  if (!textureFile.empty() && !normalTextureFile.empty()) {
    // FIXME make the code independent from the relative position of the two
    // textures.
    auto currentTexture =
        m_textureManager.createTextureArray({textureFile, normalTextureFile});
    m_textureMap[object] = currentTexture;
    return;
  }

  if (!textureFile.empty()) {
    auto currentTexture =
        m_textureManager.createTexture(textureFile);
    m_textureMap[object] = currentTexture;
  }
}

//-----------------------------------------------------------------------------
void Drawer::createMirrorObjects() {
  auto mirrorData = generateFBOColor(m_screenSize);
  m_mirrorFBO = std::get<0>(mirrorData);
  m_mirrorTexture = std::get<1>(mirrorData);
  m_mirrorDBO = createDBO(m_mirrorFBO, m_screenSize);
}

//-----------------------------------------------------------------------------
void Drawer::initGPUShadowObjects(const ShaderProgram &,
                                  const World &) {
  // TODO.
  //  for (auto mapIter : vaoWorldMap) {
  //    const Object *object = mapIter.first;
  //
  //    GLuint vaoId = 0;
  //
  //    glGenVertexArrays(1, &vaoId);
  //    glBindVertexArray(vaoId);
  //
  //    // Bind the vertex buffer to the vao.
  //    // I should not need this.
  //    GLuint vertexVBOId = setupVertexVBO(object, &shadowShader);
  //    GLuint indexVBOId = setupIndexVBO(object);
  //
  //    glBindVertexArray(0);
  //
  //    vaoShadowMap.insert(std::pair<const Object *, GLuint>(object, vaoId));
  //    vboIds.push_back(indexVBOId);
  //  }
}

//-----------------------------------------------------------------------------
GLuint Drawer::setupVertexVBO(const Object *object,
                              const ShaderProgram &shader) {
  GLuint vertexVBOId = 0;
  glGenBuffers(1, &vertexVBOId);
  glBindBuffer(GL_ARRAY_BUFFER, vertexVBOId);
  glBufferData(GL_ARRAY_BUFFER, object->getPointsNumber() * sizeof(glm::vec3),
               object->getPoints(), GL_STATIC_DRAW);
  shader.setAttribute("vertexPosition", 3, GL_FLOAT);
  return vertexVBOId;
}

//-----------------------------------------------------------------------------
GLuint Drawer::setupNormalVBO(const Object *object,
                              const ShaderProgram &shader) {
  GLuint normalVBOId = 0;
  glGenBuffers(1, &normalVBOId);
  glBindBuffer(GL_ARRAY_BUFFER, normalVBOId);
  glBufferData(GL_ARRAY_BUFFER, object->getPointsNumber() * sizeof(glm::vec3),
               object->getNormals(), GL_STATIC_DRAW);
  shader.setAttribute("vertexNormal", 3, GL_FLOAT);
  return normalVBOId;
}

//-----------------------------------------------------------------------------
GLuint Drawer::setupTextureVBO(const Object *object,
                               const ShaderProgram &shader) {
  GLuint texVBOId = 0;
  glGenBuffers(1, &texVBOId);
  glBindBuffer(GL_ARRAY_BUFFER, texVBOId);
  glBufferData(GL_ARRAY_BUFFER, object->getPointsNumber() * sizeof(glm::vec2),
               object->getTextureCoos(), GL_STATIC_DRAW);
  shader.setAttribute("vertexTextureCoordinates", 2, GL_FLOAT);
  return texVBOId;
}

//-----------------------------------------------------------------------------
GLuint Drawer::setupTangentVBO(const Object *object,
                               const ShaderProgram &shader) {
  GLuint tangentVBOId = 0;
  glGenBuffers(1, &tangentVBOId);
  glBindBuffer(GL_ARRAY_BUFFER, tangentVBOId);
  glBufferData(GL_ARRAY_BUFFER, object->getPointsNumber() * sizeof(glm::vec3),
               object->getTangents(), GL_STATIC_DRAW);
  shader.setAttribute("vertexTangent", 3, GL_FLOAT);
  return tangentVBOId;
}

//-----------------------------------------------------------------------------
GLuint Drawer::setupIndexVBO(const Object *object) {
  GLuint indexVBOId = 0;
  glGenBuffers(1, &indexVBOId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               object->getIndicesNumber() * sizeof(unsigned int),
               object->getIndices(), GL_STATIC_DRAW);
  return indexVBOId;
}

//-----------------------------------------------------------------------------
void Drawer::drawWorld(const World *world, const glm::mat4 &originalModelView,
                       const glm::mat4 &projection,
                       const glm::mat4 &originalShadowModelView,
                       const glm::mat4 &shadowProjection, const int lightMask,
                       const glm::vec4 &cameraPosition) const {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawNonReflectiveObjects(world, originalModelView, projection,
                           originalShadowModelView, shadowProjection, lightMask,
                           cameraPosition);
  if(m_mirror != nullptr)
    drawMirror(originalModelView, projection);
  //  blurLightBulbs(blurShader, blurredBulbFBOId, lightBulbTexture);
  //  drawFinalImage();
}

//-----------------------------------------------------------------------------
void Drawer::drawWorldForMirror(const World *world,
                                const glm::mat4 &originalModelView,
                                const glm::mat4 &projection,
                                const glm::mat4 &originalShadowModelView,
                                const glm::mat4 &shadowProjection,
                                const int lightMask,
                                const glm::vec4 &cameraPosition) const {

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawNonReflectiveObjects(world, originalModelView, projection,
                           originalShadowModelView, shadowProjection, lightMask,
                           cameraPosition);
}

//-----------------------------------------------------------------------------
void Drawer::drawNonReflectiveObjects(const World *world,
                                      const glm::mat4 &originalModelView,
                                      const glm::mat4 &projection,
                                      const glm::mat4 &originalShadowModelView,
                                      const glm::mat4 &shadowProjection,
                                      const int lightMask,
                                      const glm::vec4 &cameraPosition) const {
  drawPhongObjects(world, originalModelView, projection,
                   originalShadowModelView, shadowProjection, lightMask);
  drawPhongNormalMappingObjects(world, originalModelView, projection,
                                originalShadowModelView, shadowProjection,
                                lightMask);
  drawLightBulbs(originalModelView, projection, cameraPosition, lightMask);
}

//-----------------------------------------------------------------------------
void Drawer::drawPhongObjects(const World *world,
                              const glm::mat4 &originalModelView,
                              const glm::mat4 &projection,
                              const glm::mat4 &originalShadowModelView,
                              const glm::mat4 &shadowProjection,
                              const int lightMask) const {
  //  glBindFramebuffer(GL_FRAMEBUFFER, sceneFBOId);
  //  checkOpenGLError("Drawer: drawObjects-glBindFrameBuffer");
  //  glViewport(0, 0, screenSize.x, screenSize.y);

  m_phongShader.useProgram();
  setPhongLights(world, m_phongShader, originalModelView, lightMask);
  for (const auto &obj : m_phongObjects) {
    drawPhongObject(obj, originalModelView, projection, originalShadowModelView,
                    shadowProjection);
  }

  //  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//-----------------------------------------------------------------------------
void Drawer::drawPhongNormalMappingObjects(
    const World *world, const glm::mat4 &originalModelView,
    const glm::mat4 &projection, const glm::mat4 &originalShadowModelView,
    const glm::mat4 &shadowProjection, const int lightMask) const {
  //  glBindFramebuffer(GL_FRAMEBUFFER, sceneFBOId);
  //  checkOpenGLError("Drawer: drawObjects-glBindFrameBuffer");
  //  glViewport(0, 0, screenSize.x, screenSize.y);

  m_phongNormalShader.useProgram();
  setPhongLights(world, m_phongNormalShader, originalModelView, lightMask);
  for (const auto &obj : m_phongNormalMappingObjects) {
    drawPhongNormalMappingObject(obj, originalModelView, projection,
                                 originalShadowModelView, shadowProjection);
  }

  //  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//-----------------------------------------------------------------------------
void Drawer::drawLightBulbs(const glm::mat4 &originalModelView,
                            const glm::mat4 &projection,
                            const glm::vec4 &cameraPosition,
                            const int lightMask) const {
  //  glBindFramebuffer(GL_FRAMEBUFFER, lightBulbFBOId);
  //  checkOpenGLError("Drawer: drawObjects-glBindFrameBuffer");
  //  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
  //                            GL_RENDERBUFFER, dboId);
  //  checkOpenGLError("Drawer: drawObjects-glFramebufferRenderbuffer");
  //  glViewport(0, 0, screenSize.x, screenSize.y);
  //
  //  glClearColor(0.f, 0.f, 0.f, 1);
  //  glClear(GL_COLOR_BUFFER_BIT);

  m_lightBulbShader.useProgram();
  for (auto index = 0u; index < m_lightBulbs.size(); ++index) {
    if (((1 << index) & lightMask) == 0)
      continue;
    drawLightBulb(m_lightBulbs[index], originalModelView, projection,
                  cameraPosition);
  }

  //  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
  //                            GL_RENDERBUFFER, 0);
  //  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//-----------------------------------------------------------------------------
void Drawer::drawMirror(const glm::mat4 &originalModelView,
                        const glm::mat4 &projection) const {
  m_mirrorShader.useProgram();

  btScalar transform[16];
  m_mirror->getOpenGLMatrix(transform);
  glm::mat4 mirrorModelView = glm::make_mat4x4(transform);
  auto modelView = originalModelView * mirrorModelView;
  auto normalMatrix = glm::inverseTranspose(glm::mat3(mirrorModelView));

  m_mirrorShader.setUniform(MirrorShader::mvpMatrix, projection * modelView);
  m_mirrorShader.setUniform(MirrorShader::mirrorSize, m_mirror->getSize());
  m_mirrorShader.setUniform(MirrorShader::mirrorNormal,
                          m_mirror->computeNormal());
  m_mirrorShader.setUniform(MirrorShader::normalMatrix, normalMatrix);

  // Set color and normal texture.
  glBindTexture(GL_TEXTURE_2D, m_mirrorTexture);
  m_mirrorShader.setUniform(MirrorShader::texture, 0);

  invokeDrawCall(m_mirror);
  glBindTexture(GL_TEXTURE_2D, 0);
}

//-----------------------------------------------------------------------------
void Drawer::blurLightBulbs(const BlurShader &blurShader,
                            const GLuint outputFrameBuffer,
                            const GLuint inputTexture) const {
  glBindFramebuffer(GL_FRAMEBUFFER, outputFrameBuffer);
  checkOpenGLError("Drawer: drawObjects-glBindFrameBuffer");
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, m_dboId);
  checkOpenGLError("Drawer: drawObjects-glFramebufferRenderbuffer");

  glViewport(0, 0, m_screenSize.x, m_screenSize.y);
  checkOpenGLError("Drawer: drawObjects-glViewPort");

  blurShader.useProgram();
  glBindTexture(GL_TEXTURE_2D, inputTexture);
  checkOpenGLError("Drawer: drawObjects-glBindTexture");

  glBindVertexArray(m_canvasId);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
  glBindTexture(GL_TEXTURE_2D, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//-----------------------------------------------------------------------------
void Drawer::drawFinalImage() const {
  // Second pass.
  m_canvasShader.useProgram();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_sceneTexture);
  m_canvasShader.setUniform(CanvasShader::firstTexture, 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, m_blurredBulbTexture);
  m_canvasShader.setUniform(CanvasShader::secondTexture, 1);

  glBindVertexArray(m_canvasId);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

//-----------------------------------------------------------------------------
void Drawer::setPhongLights(const World *world, const PhongShader &shader,
                            const glm::mat4 &modelView, const int lightMask) {
  shader.setUniform(PhongShader::ambientColor, world->getAmbientColor());
  shader.setUniform(PhongShader::lightsNumber, world->getLightsNumber());
  shader.setUniform(PhongShader::lightMask, lightMask);
  std::for_each(
      constBeginLights(*world), constEndLights(*world),
      [&](const Light *light) { light->setUniforms(shader, modelView); });
}
//-----------------------------------------------------------------------------
void Drawer::setPhongLights(const World *world,
                            const PhongNormalMappingShader &shader,
                            const glm::mat4 &modelView, const int lightMask) {
  shader.setUniform(PhongNormalMappingShader::ambientColor,
                    world->getAmbientColor());
  shader.setUniform(PhongNormalMappingShader::lightsNumber,
                    world->getLightsNumber());
  shader.setUniform(PhongNormalMappingShader::lightMask, lightMask);
  std::for_each(
      constBeginLights(*world), constEndLights(*world),
      [&](const Light *light) { light->setUniforms(shader, modelView); });
}

//-----------------------------------------------------------------------------
void Drawer::drawPhongObject(const Object *object,
                             const glm::mat4 &originalModelView,
                             const glm::mat4 &projection,
                             const glm::mat4 &originalShadowModelView,
                             const glm::mat4 &shadowProjection) const {
  setColors(object, m_phongShader);
  setOrientation(object, m_phongShader, originalModelView, projection,
                 originalShadowModelView, shadowProjection);
  // Set color and normal texture.
  glBindTexture(GL_TEXTURE_2D, m_textureMap.at(object));
  checkOpenGLError("drawPhongObject: glBindTexture");

  m_phongShader.setUniform(PhongShader::texture, 0);

  invokeDrawCall(object);
  glBindTexture(GL_TEXTURE_2D, 0);
}

//-----------------------------------------------------------------------------
void Drawer::drawPhongNormalMappingObject(
    const Object *object, const glm::mat4 &originalModelView,
    const glm::mat4 &projection, const glm::mat4 &originalShadowModelView,
    const glm::mat4 &shadowProjection) const {
  setColors(object, m_phongNormalShader);
  setOrientation(object, m_phongNormalShader, originalModelView, projection,
                 originalShadowModelView, shadowProjection);
  glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureMap.at(object));
  checkOpenGLError("drawPhongNormalMappingObject: glBindTexture");
  m_phongNormalShader.setUniform(PhongNormalMappingShader::textureArray, 0);

  invokeDrawCall(object);
  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

//-----------------------------------------------------------------------------
void Drawer::drawLightBulb(const Object *lightBulb,
                           const glm::mat4 &originalModelView,
                           const glm::mat4 &projection,
                           const glm::vec4 &cameraPosition) const {
  setLightBulbOrientation(lightBulb, m_lightBulbShader, originalModelView,
                          projection, cameraPosition);
  invokeDrawCall(lightBulb);
}

//-----------------------------------------------------------------------------
void Drawer::invokeDrawCall(const Object *object) const {
  GLuint vao = m_vaoWorldMap.at(object);
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, object->getIndicesNumber(), GL_UNSIGNED_INT,
                 nullptr);
  glBindVertexArray(0);
}

//------------------------------------------------------------------------------
void Drawer::drawObjectForShadow(const Object *object, ShaderProgram &shader,
                                 const glm::mat4 &originalModelView,
                                 const glm::mat4 &projection) const {
  setOrientationForShadow(object, shader, originalModelView, projection);

  GLuint vao = m_vaoShadowMap.at(object);
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, object->getIndicesNumber(), GL_UNSIGNED_INT,
                 nullptr);
  glBindVertexArray(0);
}

//------------------------------------------------------------------------------
void setColors(const Object *object, const PhongShader &shader) {
  shader.setUniform(PhongShader::materialAmbient, object->getAmbientColor());
  // The diffuse color is replaces by a texture.
  shader.setUniform(PhongShader::materialSpecular, object->getSpecularColor());
  shader.setUniform(PhongShader::materialShininess, object->getShininess());
}

//------------------------------------------------------------------------------
void setColors(const Object *object, const PhongNormalMappingShader &shader) {
  shader.setUniform(PhongNormalMappingShader::materialAmbient,
                    object->getAmbientColor());
  // The diffuse color is replaces by a texture.
  shader.setUniform(PhongNormalMappingShader::materialSpecular,
                    object->getSpecularColor());
  shader.setUniform(PhongNormalMappingShader::materialShininess,
                    object->getShininess());
}

//------------------------------------------------------------------------------
void setOrientation(const Object *object, const PhongShader &shader,
                    const glm::mat4 &originalModelView,
                    const glm::mat4 &projection,
                    const glm::mat4 &,
                    const glm::mat4 &) {
  //  glm::mat4 biasMatrix(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0,
  //  0.5,
  //                       0.0, 0.5, 0.5, 0.5, 1.0);
  //  glm::mat4 shadowMVP =
  //      biasMatrix * shadowProjection * originalShadowModelView * modelView;

  auto modelView = getObjectModelView(object, originalModelView);

  shader.setUniform(PhongShader::mvpMatrix, projection * modelView);
  shader.setUniform(PhongShader::modelViewMatrix, modelView);
  shader.setUniform(PhongShader::normalMatrix,
                    glm::inverseTranspose(glm::mat3(modelView)));
  //  shader->setUniform("mvpShadowMatrix", shadowMVP);
}
//------------------------------------------------------------------------------
void setOrientation(const Object *object,
                    const PhongNormalMappingShader &shader,
                    const glm::mat4 &originalModelView,
                    const glm::mat4 &projection,
                    const glm::mat4 &,
                    const glm::mat4 &) {
  //  glm::mat4 biasMatrix(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0,
  //  0.5,
  //                       0.0, 0.5, 0.5, 0.5, 1.0);
  //  glm::mat4 shadowMVP =
  //      biasMatrix * shadowProjection * originalShadowModelView * modelView;

  auto modelView = getObjectModelView(object, originalModelView);

  shader.setUniform(PhongNormalMappingShader::mvpMatrix,
                    projection * modelView);
  shader.setUniform(PhongNormalMappingShader::modelViewMatrix, modelView);
  shader.setUniform(PhongNormalMappingShader::normalMatrix,
                    glm::inverseTranspose(glm::mat3(modelView)));
  //  shader->setUniform("mvpShadowMatrix", shadowMVP);
}

//------------------------------------------------------------------------------
void setLightBulbOrientation(const Object *object,
                             const LightBulbShader &shader,
                             const glm::mat4 &originalModelView,
                             const glm::mat4 &projection,
                             const glm::vec4 &cameraPosition) {
  btScalar transform[16];
  object->getOpenGLMatrix(transform);
  auto modelView = glm::make_mat4x4(transform);
  auto lightBulbPosition = modelView[3];
  auto direction = glm::vec3(cameraPosition - lightBulbPosition);
  // Rotation around axis X.
  auto distanceXZ =
      glm::sqrt(direction.x * direction.x + direction.z * direction.z);
  auto directionYZ = glm::normalize(glm::vec2(distanceXZ, direction.y));
  // Rotation around axis Y.
  auto directionXZ = glm::normalize(glm::vec2(direction.x, direction.z));
  auto cy = directionXZ.y;
  auto sy = directionXZ.x;
  auto cx = directionYZ.x;
  auto sx = -directionYZ.y;

  glm::mat4 yxRotation = {cy, 0, -sy, 0, sx * sy, cx, sx * cy, 0, cx * sy, -sx,
                          cx * cy, 0, lightBulbPosition.x, lightBulbPosition.y,
                          lightBulbPosition.z, 1};
  modelView = originalModelView * yxRotation;
  shader.setUniform(PhongShader::mvpMatrix, projection * modelView);
}

//------------------------------------------------------------------------------
inline glm::mat4 getObjectModelView(const Object *object,
                                    const glm::mat4 &originalModelView) {

  btScalar transform[16];
  object->getOpenGLMatrix(transform);
  glm::mat4 modelView = glm::make_mat4x4(transform);
  return originalModelView * modelView;
}

//------------------------------------------------------------------------------
void setOrientationForShadow(const Object *, const ShaderProgram &,
                             const glm::mat4 &,
                             const glm::mat4 &) {
  // TODO
  //  btScalar transform[16];
  //  object->getOpenGLMatrix(transform);
  //  glm::mat4 modelView = glm::make_mat4x4(transform);
  //  glm::mat4 mvp = projection * originalModelView * modelView;
  //  shader.setUniform(LightedObjectShader::mvpMatrix, mvp);
}

//------------------------------------------------------------------------------
std::pair<GLuint, GLuint> generateFBOColor(const glm::ivec2 screenSize) {
  GLuint fboId = 0;
  GLuint textureId = 0;

  // Init lightBulb Texture.
  glGenFramebuffers(1, &fboId);
  checkOpenGLError("generateFBOColor: glGenFramebuffers");

  glGenTextures(1, &textureId);
  checkOpenGLError("generateFBOColor: glGenTextures");

  glBindTexture(GL_TEXTURE_2D, textureId);
  checkOpenGLError("generateFBOColor: glBindTexture");

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenSize.x, screenSize.y, 0,
               GL_RGBA, GL_FLOAT, nullptr);
  checkOpenGLError("generateFBOColor: glTexImage2D");

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  checkOpenGLError("generateFBOColor: glTexParameteri");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  checkOpenGLError("generateFBOColor: glTexParameteri");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  checkOpenGLError("generateFBOColor: glTexParameteri");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  checkOpenGLError("generateFBOColor: glTexParameteri");

  glBindFramebuffer(GL_FRAMEBUFFER, fboId);
  checkOpenGLError("generateFBOColor: attachTexture-glBindBuffer");
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, textureId, 0);
  checkOpenGLError("generateFBOColor: attachTexture-glFramebufferTexture");

  GLenum drawBuffers = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, &drawBuffers);
  checkOpenGLError("generateFBOColor: glDrawBuffers");

  assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE &&
         "Error setting frame buffer");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return {fboId, textureId};
}

//------------------------------------------------------------------------------
GLuint createDBO(GLuint fboId, const glm::ivec2 screenSize) {
  GLuint dboId = 0;
  glGenRenderbuffers(1, &dboId);
  checkOpenGLError("glGenRenderbuffers");

  glBindRenderbuffer(GL_RENDERBUFFER, dboId);
  checkOpenGLError("glBindRenderbuffer");

  // Define the size of the render buffer.
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenSize.x,
                        screenSize.y);
  checkOpenGLError("glRenderbufferStorage");

  glBindFramebuffer(GL_FRAMEBUFFER, fboId);
  checkOpenGLError("glBindFramebuffer");
  // Attach the render buffer to the currently bound framebuffer.
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, dboId);
  checkOpenGLError("glFramebufferRenderbuffer");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  return dboId;
}
