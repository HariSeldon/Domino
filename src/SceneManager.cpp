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

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/ext.hpp>

#include "SDL2/SDL_image.h"

const std::string SceneManager::FONT_FILE = "VeraMono.ttf";

// -----------------------------------------------------------------------------
SceneManager::SceneManager(const glm::ivec2 &screenSize,
                           SceneContainer *container)
    : world(container->getWorld()), camera(container->getCamera()),
      textManager(TextManager(FONT_PATH + FONT_FILE, FONT_HEIGHT, screenSize)),
      BACKGROUND_COLOR(container->getBackgroundColor()),
      currentYRotation(0.0), currentXRotation(0.0), currentOffset(0.0),
      positionMutex(SDL_CreateMutex()), fps(0) {
  lightMask = (2 << (world->getLightsNumber() - 1)) - 1;
  setupProjection(screenSize);
  initGPU(container);
  checkOpenGLError("GLInitializer: glClearColor");
}

// -----------------------------------------------------------------------------
void SceneManager::setupProjection(const glm::ivec2 &screenSize) {
  glViewport(0, 0, screenSize.x, screenSize.y);
  float aspectRatio = (float)screenSize.x / (float)screenSize.y;
  projection = glm::perspective(camera->getViewAngle(), aspectRatio,
                                camera->getZNear(), camera->getZFar());
}

// -----------------------------------------------------------------------------
void SceneManager::initGPU(SceneContainer *container) {
  // The drawing function depends on the objects in the scene.
  drawer.initGPUObjects(container->getShaderMap());
  //drawer.initGPUShadowObjects(shadowManager.getShader(), *world);
  //if (Mirror *mirror = world->getMirror()) {
  //  ShaderProgram &mirrorShader = mirror->getShaderProgram();
  //  drawer.initMirror(mirrorShader, mirror);
  //}
  drawer.initTextures(*world);
}

// -----------------------------------------------------------------------------
SceneManager::~SceneManager() {
  delete world;
  delete camera;
  glUseProgram(0);
  SDL_DestroyMutex(positionMutex);
}

// -----------------------------------------------------------------------------
void SceneManager::drawScene() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mirrorRenderingPass();
  //shadowRenderingPass();
  screenRenderingPass();
}

// -----------------------------------------------------------------------------
void SceneManager::screenRenderingPass() {
  glm::mat4 modelView = camera->applyView();
  drawWorld(modelView, glm::vec3(camera->getPosition()));
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

  glClearColor(BACKGROUND_COLOR.x, BACKGROUND_COLOR.y, BACKGROUND_COLOR.z,
               BACKGROUND_COLOR.w);
  drawer.drawWorld(world, modelView, projection, shadowView, shadowProjection,
                   lightMask, camera->getPosition());
  drawer.drawMirror(modelView, projection);
}

//// -----------------------------------------------------------------------------
//void SceneManager::drawMirror(const glm::mat4 &) {
////  if (Mirror *mirror = world->getMirror()) {
////    ShaderProgram &mirrorShader = mirror->getShaderProgram();
////    mirrorShader.useProgram();
////    //mirrorShader.setUniform("projectionMatrix", projection);
////    //mirrorShader.setUniform("texture", 0);
//////    drawer.drawObject(mirror, mirrorShader, modelView, projection);
////  }
//}

// -----------------------------------------------------------------------------
void SceneManager::mirrorRenderingPass() {
  // FIXME remove this if.
  if (Mirror *mirror = world->getMirror()) {
    // Draw the scene on the mirror texture from the point of view of the
    // mirror.
    drawer.enableMirror();
    glm::mat4 cameraView = mirror->getModelView();
    auto tmp = mirror->getPosition();
    glm::vec3 mirrorPosition {tmp.x(), tmp.y(), tmp.z()};
    drawWorld(cameraView, mirrorPosition);
    drawer.disableMirror();
  }
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

  std::for_each(constBeginObjects(*world), constEndObjects(*world),
                [&](const Object *object) {
    drawer.drawObjectForShadow(object, shader, modelView, projection);
  });
}

// -----------------------------------------------------------------------------
void SceneManager::drawText() {
  glDisable(GL_DEPTH_TEST);
  textManager.addText("Frames per second: " + std::to_string(fps), { 0, 780 });
  textManager.renderText();
  glEnable(GL_DEPTH_TEST);
}

// -----------------------------------------------------------------------------
void SceneManager::setFps(int fps) { this->fps = fps; }

// -----------------------------------------------------------------------------
void SceneManager::updateCameraPosition() {
  SDL_LockMutex(positionMutex);

  camera->move(currentOffset);
  camera->rotate({currentYRotation, -currentXRotation});

  currentOffset = 0.0f;
  currentXRotation = 0.0f;
  currentYRotation = 0.0f;

  SDL_UnlockMutex(positionMutex);
}

// -----------------------------------------------------------------------------
void SceneManager::updateCurrentCameraRotation(glm::vec2 diff) {
  SDL_LockMutex(positionMutex);
  currentXRotation += diff.x;
  currentYRotation += diff.y;
  SDL_UnlockMutex(positionMutex);
}

// -----------------------------------------------------------------------------
void SceneManager::updateCurrentCameraPosition(unsigned char mask) {
  SDL_LockMutex(positionMutex);
  if (mask & 8)
    currentOffset += Camera::STEP; 
  if (mask & 4)
    currentOffset -= Camera::STEP; 
  if (mask & 2)
    currentXRotation -= Camera::FIXED_ROTATION_ANGLE * 1500;
  if (mask & 1)
    currentXRotation += Camera::FIXED_ROTATION_ANGLE * 1500;
  SDL_UnlockMutex(positionMutex);
}

// -----------------------------------------------------------------------------
int SceneManager::getLightMask() const { return lightMask; }

// -----------------------------------------------------------------------------
void SceneManager::updateLightMask(int lightMask) {
  this->lightMask = lightMask;
}

// -----------------------------------------------------------------------------
void SceneManager::stepSimulation() { world->stepSimulation(); }
