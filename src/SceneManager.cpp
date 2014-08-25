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
#include <iostream>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/ext.hpp>

const float SceneManager::VIEW_ANGLE = 50.0f;
const float SceneManager::Z_NEAR = 0.1f;
const float SceneManager::Z_FAR = 2000.0f;

const glm::vec4 SceneManager::CLEAR_COLOR = { 0.2f, 0.4f, 0.6f, 1.f };

const std::string SceneManager::FONT_FILE = "VeraMono.ttf";
const std::string SceneManager::MAIN_VERTEX_SHADER = "phong_shadow.vert";
const std::string SceneManager::MAIN_FRAGMENT_SHADER = "phong_shadow.frag";

// -----------------------------------------------------------------------------
SceneManager::SceneManager(const glm::ivec2 &screenSize)
    : worldShader(MAIN_VERTEX_SHADER, MAIN_FRAGMENT_SHADER),
      textManager(TextManager(FONT_PATH + FONT_FILE, FONT_HEIGHT, screenSize)),
      fps(0), lightMask((2 << (world.getLightsNumber() - 1)) - 1) {
  setupProjection(screenSize);
  initGPU();
  glClearColor(CLEAR_COLOR.x, CLEAR_COLOR.y, CLEAR_COLOR.z, CLEAR_COLOR.w);
  checkOpenGLError("GLInitializer: glClearColor");
}

// -----------------------------------------------------------------------------
void SceneManager::setupProjection(const glm::ivec2& screenSize) {
  glViewport(0, 0, screenSize.x, screenSize.y);
  float aspectRatio = (float)screenSize.x / (float)screenSize.y;
  projection = glm::perspective(SceneManager::VIEW_ANGLE, aspectRatio,
                                SceneManager::Z_NEAR, SceneManager::Z_FAR);
}

// -----------------------------------------------------------------------------
void SceneManager::initGPU() {
  drawer.initGPUObjects(worldShader, world);
  drawer.initGPUShadowObjects(shadowManager.getShader(), world);
  if (Mirror *mirror = world.getMirror()) {
    ShaderProgram &mirrorShader = mirror->getShaderProgram();
    drawer.initMirror(mirrorShader, mirror);
  }
}

// -----------------------------------------------------------------------------
SceneManager::~SceneManager() {
  glUseProgram(0);
}

// -----------------------------------------------------------------------------
void SceneManager::drawScene() {

//  mirrorRenderingPass();
  shadowRenderingPass();
  screenRenderingPass();
}

// -----------------------------------------------------------------------------
void SceneManager::drawWorld(const glm::mat4 &modelView,
                             ShaderProgram &shader) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  drawObjects(modelView, shader);
  drawLights(modelView, shader);
}

// -----------------------------------------------------------------------------
void SceneManager::drawObjects(const glm::mat4 &modelView,
                               ShaderProgram &shader) {
  glm::mat4 shadowView =
      glm::lookAt(glm::vec3(0, 2, 0), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
  float side = 8;
  glm::mat4 shadowProjection =
      glm::ortho<float>(-side, side, -side, side, 0, side);

  std::for_each(constBeginObjects(world), constEndObjects(world),
                [&](const Object *object) {
    drawer.drawObject(object, shader, modelView, projection, shadowView,
                      shadowProjection);
  });
}

// -----------------------------------------------------------------------------
void SceneManager::drawLights(const glm::mat4 &modelView,
                              ShaderProgram &shader) {
  shader.setUniform("ambientColor", world.getAmbientColor());
  shader.setUniform("lightsNumber", world.getLightsNumber());
  shader.setUniform("lightMask", lightMask);
  std::for_each(
      constBeginLights(world), constEndLights(world),
      [&](const Light *light) { light->draw(shader, modelView); });
}

// -----------------------------------------------------------------------------
void SceneManager::drawMirror(const glm::mat4 &modelView) {
  if (Mirror *mirror = world.getMirror()) {
    ShaderProgram &mirrorShader = mirror->getShaderProgram();
    mirrorShader.useProgram();
    mirrorShader.setUniform("projectionMatrix", projection);
    mirrorShader.setUniform("texture", 0);
    //drawer.drawObject(mirror, mirrorShader, modelView, projection);
  }
}

// -----------------------------------------------------------------------------
void SceneManager::mirrorRenderingPass() {
  if (Mirror *mirror = world.getMirror()) {
    // Draw the scene on the mirror texture from the point of view of the
    // mirror.
    mirror->enableMirror();
    glm::mat4 cameraView = mirror->getModelView();
    drawWorld(cameraView, worldShader);
    mirror->disableMirror();
  }
}

// -----------------------------------------------------------------------------
void SceneManager::shadowRenderingPass() {
  glm::mat4 shadowView =
      glm::lookAt(glm::vec3(0, 2, 0), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));

  float side = 8;
  glm::mat4 shadowProjection =
      glm::ortho<float>(-side, side, -side, side, 0, side);

  ShaderProgram &shadowShader = shadowManager.getShader();
  shadowShader.useProgram();
  shadowManager.enableShadow();
  drawShadowWorld(shadowView, shadowProjection, shadowShader);
  shadowManager.disableShadow();
}

// -----------------------------------------------------------------------------
void SceneManager::drawShadowWorld(const glm::mat4 &modelView,
                                   const glm::mat4 &projection, 
                                   ShaderProgram &shader) {
  // FIXME I think I should need to clear just the depth buffer.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  checkOpenGLError("drawShadowWorld: glClear");

  std::for_each(constBeginObjects(world), constEndObjects(world),
                [&](const Object *object) {
    drawer.drawObjectForShadow(object, shader, modelView, projection);
  });
}

// -----------------------------------------------------------------------------
void SceneManager::screenRenderingPass() {
  worldShader.useProgram();
  cameraMutex.lock();
  // FIXME
  // domino: Camera.cpp:55:
  // glm::mat4 Camera::applyView(): Assertion `eye == position && "Wrong
  // transformation construction"' failed.

  worldShader.setUniform("shadowTexture", 0);

  glm::mat4 modelView = camera.applyView();
  cameraMutex.unlock();
  drawWorld(modelView, worldShader);

  //  drawMirror(modelView);

  // Draw text on top of the scene.
  drawText();
}

// -----------------------------------------------------------------------------
void SceneManager::drawText() {
  glDisable(GL_DEPTH_TEST);
  textManager.addText("Frames per second: " + std::to_string(fps), { 0, 780 });
  textManager.renderText();
  glEnable(GL_DEPTH_TEST);
}

// -----------------------------------------------------------------------------
void SceneManager::setFps(int fps) {
  this->fps = fps;
} 

// -----------------------------------------------------------------------------
void SceneManager::updateCameraRotation(glm::ivec2 diff) {  
  currentXRotation += diff.x;
  currentYRotation += diff.y;
}

// -----------------------------------------------------------------------------
void SceneManager::updateCameraPosition(unsigned char mask) {
  cameraMutex.lock();

  if (mask & 8)
    camera.moveForward();
  if (mask & 4)
    camera.moveBackward();
  if (mask & 2)
    camera.rotateLeft();
  if (mask & 1)
    camera.rotateRight();

  // When rotating the camera the lights change intensity.
  camera.rotate(currentYRotation, currentXRotation);
  cameraMutex.unlock();

  currentYRotation = 0;
  currentXRotation = 0;
}

// -----------------------------------------------------------------------------
int SceneManager::getLightMask() const {
  return lightMask; 
}

// -----------------------------------------------------------------------------
void SceneManager::updateLightMask(int lightMask) {
  this->lightMask = lightMask;
}
