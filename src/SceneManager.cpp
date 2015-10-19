#include "SceneManager.h"

#include "Drawer.h"
#include "Light.h"
#include "Mirror.h"
#include "ShaderProgram.h"
#include "ShadowManager.h"
#include "SysDefines.h"
#include "TextManager.h"
#include "World.h"

#include <algorithm>
#include <thread>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/ext.hpp>

#include "SDL2/SDL_image.h"

const std::string SceneManager::FONT_FILE = "VeraMono.ttf";

// -----------------------------------------------------------------------------
SceneManager::SceneManager(const glm::ivec2 screenSize,
                           SceneContainer *container)
    : m_world(container->getWorld()), m_camera(container->getCamera()),
      m_textManager(TextManager(FONT_PATH + FONT_FILE, FONT_HEIGHT, screenSize)),
      m_drawer(screenSize),
      m_shadowManager(screenSize),
      m_BACKGROUND_COLOR(container->getBackgroundColor()),
      m_positionMutex(SDL_CreateMutex()) {
  m_lightMask = (2 << (m_world->getLightsNumber() - 1)) - 1;
  setupProjection(screenSize);
  initGPU(container);
  glClearColor(m_BACKGROUND_COLOR.x, m_BACKGROUND_COLOR.y, m_BACKGROUND_COLOR.z,
               m_BACKGROUND_COLOR.w);
  checkOpenGLError("GLInitializer: glClearColor");
}

// -----------------------------------------------------------------------------
void SceneManager::setupProjection(const glm::ivec2 &screenSize) {
  glViewport(0, 0, screenSize.x, screenSize.y);
  float aspectRatio =
      static_cast<float>(screenSize.x) / static_cast<float>(screenSize.y);
  m_projection = glm::perspective(m_camera->getViewAngle(), aspectRatio,
                                m_camera->getZNear(), m_camera->getZFar());
}

// -----------------------------------------------------------------------------
void SceneManager::initGPU(SceneContainer *container) {
  m_drawer.initGPUObjects(container->getShaderMap(),
                        container->getWorld()->getMirror());
  //drawer.initGPUShadowObjects(shadowManager.getShader(), *world);
  m_drawer.initTextures(*m_world);
  if(container->getWorld()->getMirror() != nullptr)
    m_mirrorPass = &SceneManager::mirrorRenderingPass;
  else
    m_mirrorPass = &SceneManager::noMirrorRenderingPass;
}

// -----------------------------------------------------------------------------
SceneManager::~SceneManager() {
  delete m_world;
  delete m_camera;
  glUseProgram(0);
  SDL_DestroyMutex(m_positionMutex);
}

// -----------------------------------------------------------------------------
void SceneManager::drawScene() { 
//  auto begin = std::chrono::system_clock::now();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_mirrorPass(this);
  //shadowRenderingPass();
  screenRenderingPass();
  glFinish();
//  auto end = std::chrono::system_clock::now();
//  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
//  std::cout << duration << "\n";
}

// -----------------------------------------------------------------------------
inline void SceneManager::screenRenderingPass() {
  glm::mat4 modelView = m_camera->applyView();
  drawWorld(modelView, glm::vec3(m_camera->getPosition()));
  drawText();
}

// -----------------------------------------------------------------------------
void SceneManager::drawWorld(const glm::mat4 &modelView,
                             const glm::vec3 &cameraPosition) {
  glm::mat4 shadowView =
      glm::lookAt(glm::vec3(-11, 4, 0), glm::vec3(-9.29, 3.29, 0),
                  glm::vec3(10.29, 4.71, 0));
  float side = 11;
  glm::mat4 shadowProjection =
      glm::ortho<float>(-side, side, -5, side / 1.6, 0, 2.5 * side);

  m_drawer.drawWorld(m_world, modelView, m_projection, shadowView, shadowProjection,
                   m_lightMask, glm::vec4(cameraPosition, 1.f));
}

// -----------------------------------------------------------------------------
void SceneManager::mirrorRenderingPass() {
  // Draw the scene on the mirror texture from the point of view of the mirror.
  auto mirror = m_world->getMirror();
  m_drawer.enableMirror();
  glm::mat4 cameraView = mirror->computeMirrorView();
  auto tmp = mirror->getPosition();
  glm::vec3 mirrorPosition {tmp.x(), tmp.y(), tmp.z()};
  glm::mat4 shadowView =
      glm::lookAt(glm::vec3(-11, 4, 0), glm::vec3(-9.29, 3.29, 0),
                  glm::vec3(10.29, 4.71, 0));
  float side = 11;
  glm::mat4 shadowProjection =
      glm::ortho<float>(-side, side, -5, side / 1.6, 0, 2.5 * side);
  m_drawer.drawWorldForMirror(m_world, cameraView, m_projection, shadowView,
                            shadowProjection, m_lightMask,
                            glm::vec4(mirrorPosition, 1.f));
  m_drawer.disableMirror();
}

// -----------------------------------------------------------------------------
void SceneManager::noMirrorRenderingPass() {
}

// -----------------------------------------------------------------------------
void SceneManager::shadowRenderingPass() {
//  glm::mat4 shadowView =
//      glm::lookAt(glm::vec3(-11, 4, 0), glm::vec3(-9.29, 3.29, 0),
//                  glm::vec3(10.29, 4.71, 0));
//
//  // The projection matrix is completelly arbitrary.
//  float side = 11;
//  glm::mat4 shadowProjection =
//      glm::ortho<float>(-side, side, -5, side / 1.6, 0, 2.5 * side);
//
//  ShaderProgram &shadowShader = shadowManager.getShader();
//  shadowShader.useProgram();
//  shadowManager.enableShadow();
//  drawShadowWorld(shadowView, shadowProjection, shadowShader);
//  shadowManager.disableShadow();
}

// -----------------------------------------------------------------------------
void SceneManager::drawShadowWorld(const glm::mat4 &modelView,
                                   const glm::mat4 &projection,
                                   ShaderProgram &shader) {
  glClear(GL_DEPTH_BUFFER_BIT);
  checkOpenGLError("drawShadowWorld: glClear");

  std::for_each(constBeginObjects(*m_world), constEndObjects(*m_world),
                [&](const Object *object) {
    m_drawer.drawObjectForShadow(object, shader, modelView, projection);
  });
}

// -----------------------------------------------------------------------------
void SceneManager::drawText() {
  glDisable(GL_DEPTH_TEST);
  m_textManager.addText("Frames per second: " + std::to_string(m_fps), { 0, 780 });
  m_textManager.renderText();
  glEnable(GL_DEPTH_TEST);
}

// -----------------------------------------------------------------------------
void SceneManager::setFps(int fps) { m_fps = fps; }

// -----------------------------------------------------------------------------
void SceneManager::updateCameraPosition() {
  SDL_LockMutex(m_positionMutex);

  m_camera->move(m_currentOffset);
  m_camera->rotate({m_currentYRotation, -m_currentXRotation});

  m_currentOffset = 0.0f;
  m_currentXRotation = 0.0f;
  m_currentYRotation = 0.0f;

  SDL_UnlockMutex(m_positionMutex);
}

// -----------------------------------------------------------------------------
void SceneManager::updateCurrentCameraRotation(glm::vec2 diff) {
  SDL_LockMutex(m_positionMutex);
  m_currentXRotation += diff.x;
  m_currentYRotation += diff.y;
  SDL_UnlockMutex(m_positionMutex);
}

// -----------------------------------------------------------------------------
void SceneManager::updateCurrentCameraPosition(unsigned char mask) {
  SDL_LockMutex(m_positionMutex);
  if (mask & 8)
    m_currentOffset += Camera::STEP; 
  if (mask & 4)
    m_currentOffset -= Camera::STEP; 
  if (mask & 2)
    m_currentXRotation -= Camera::FIXED_ROTATION_ANGLE * 1500;
  if (mask & 1)
    m_currentXRotation += Camera::FIXED_ROTATION_ANGLE * 1500;
  SDL_UnlockMutex(m_positionMutex);
}

// -----------------------------------------------------------------------------
int SceneManager::getLightMask() const { return m_lightMask; }

// -----------------------------------------------------------------------------
void SceneManager::updateLightMask(int lightMask) {
  m_lightMask = lightMask;
}

// -----------------------------------------------------------------------------
void SceneManager::stepSimulation() { m_world->stepSimulation(); }
