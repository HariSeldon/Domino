#pragma once

#include "ShaderProgram.h"

#include <string>

#include <GL/glew.h>

class ShadowManager {
public:
  ShadowManager();
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
  GLuint fboId;
  GLuint shadowTexture;
//  ShaderProgram shadowShader;
  static const std::string SHADOW_VERTEX_SHADER;
  static const std::string SHADOW_FRAGMENT_SHADER;
}; 
