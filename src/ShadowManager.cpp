#include "ShadowManager.h"

#include "ShaderProgram.h"
#include "SysUtils.h"

#include <cassert>
#include <iostream>

const std::string ShadowManager::SHADOW_VERTEX_SHADER = "shadow.vert";
const std::string ShadowManager::SHADOW_FRAGMENT_SHADER = "shadow.frag";

//-----------------------------------------------------------------------------
ShadowManager::ShadowManager()
    : fboId(0), shadowTexture(0) {
      //shadowShader("shadow", SHADOW_VERTEX_SHADER, SHADOW_FRAGMENT_SHADER)
//  createFBO();
//  createShadowTexture();
//  attachTexture();
}

ShadowManager::~ShadowManager() {
  glGenTextures(1, &shadowTexture);
  glDeleteFramebuffers(1, &fboId);
}

//-----------------------------------------------------------------------------
GLuint ShadowManager::getTexture() const {
  return shadowTexture;
}

//-----------------------------------------------------------------------------
void ShadowManager::createFBO() {
  glGenFramebuffers(1, &fboId);
  checkOpenGLError("Shadow: glGenRenderbuffers");
}

//-----------------------------------------------------------------------------
void ShadowManager::createShadowTexture() {
  glGenTextures(1, &shadowTexture);
  checkOpenGLError("Shadow: glGenTextures");

  glBindTexture(GL_TEXTURE_2D, shadowTexture);
  checkOpenGLError("Shadow: glBindTexture");

  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1280, 800, 0,
               GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  checkOpenGLError("Shadow: glTexImage2D");

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  checkOpenGLError("Shadow: glTexParameteri");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  checkOpenGLError("Shadow: glTexParameteri");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  checkOpenGLError("Shadow: glTexParameteri");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  checkOpenGLError("Shadow: glTexParameteri");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  checkOpenGLError("Shadow: glTexParameteri");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
                  GL_COMPARE_R_TO_TEXTURE);
  checkOpenGLError("Shadow: glTexParameteri");

  glBindTexture(GL_TEXTURE_2D, 0);
}

//-----------------------------------------------------------------------------
void ShadowManager::attachTexture() {
  glBindTexture(GL_TEXTURE_2D, shadowTexture);
  checkOpenGLError("Shadow: attachTexture-glBindTexture");
  glBindFramebuffer(GL_FRAMEBUFFER, fboId);
  checkOpenGLError("ShadowManager: attachTexture-glBindBuffer");
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTexture, 0);
  checkOpenGLError("Shadow: attachTexture-glFramebufferTexture");

  glDrawBuffer(GL_NONE);
  checkOpenGLError("Shadow: glDrawBuffer");
  glReadBuffer(GL_NONE);
  checkOpenGLError("Shadow: glReadBuffer");

  assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE &&
         "Error setting frame buffer");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//-----------------------------------------------------------------------------
void ShadowManager::enableShadow() const {
  glBindFramebuffer(GL_FRAMEBUFFER, fboId);
  checkOpenGLError("ShadowManager: enableShadow-glBindFrameBuffer");
  glViewport(0, 0, 1280, 800);
  glBindTexture(GL_TEXTURE_2D, shadowTexture);
  checkOpenGLError("ShadowManager: enableShadow-glBindTexture");
}

//-----------------------------------------------------------------------------
void ShadowManager::disableShadow() const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//-----------------------------------------------------------------------------
//ShaderProgram &ShadowManager::getShader() { return shadowShader; }
