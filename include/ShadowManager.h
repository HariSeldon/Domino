#pragma once

#include "ShaderProgram.h"

#include <glm/vec2.hpp>

#include <string>

#include <GL/glew.h>

class ShadowManager {
public:
  ShadowManager(const glm::ivec2 screenSize);
  ~ShadowManager();

public:
  void enableShadow() const;
  void disableShadow() const;
//  ShaderProgram &getShader();
  GLuint getTexture() const;

private:
  void createFBO();
  void createShadowTexture();
  void attachTexture();

private:
  glm::ivec2 m_screenSize;
  GLuint m_fboId = 0;
  GLuint m_shadowTexture = 0;
//  ShaderProgram shadowShader;
  static const std::string SHADOW_VERTEX_SHADER;
  static const std::string SHADOW_FRAGMENT_SHADER;
}; 
