#pragma once

#include "ShaderProgram.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

// -----------------------------------------------------------------------------
class Light {
public:
  static const glm::vec4 DEFAULT_AMBIENT_COLOR;
  static const glm::vec4 DEFAULT_DIFFUSE_COLOR;
  static const glm::vec4 DEFAULT_SPECULAR_COLOR;
  static const glm::vec3 DEFAULT_DIRECTION;
  static const glm::vec3 DEFAULT_POSITION;
  static const float DEFAULT_SPOT_CUTOFF;
  static constexpr float DEFAULT_SPOT_EXPONENT = 0.0f;
  static constexpr float DEFAULT_CONSTANT_ATTENUATION = 1.0f;
  static constexpr float DEFAULT_LINEAR_ATTENUATION = 0.0f;
  static constexpr float DEFAULT_QUADRATIC_ATTENUATION = 0.0f;
  
public:
  virtual ~Light() {};

  virtual void draw(ShaderProgram &shaderProgram, const glm::mat4 &modelView) const;

  void setDiffuseColor(const glm::vec4 &color);
  const glm::vec4 &getDiffuseColor() const;

  void setAmbientColor(const glm::vec4 &color);
  const glm::vec4 &getAmbientColor() const;

  void setSpecularColor(const glm::vec4 &color);
  const glm::vec4 &getSpecularColor() const;  

protected:
  int number;
  glm::vec4 ambientColor;
  glm::vec4 diffuseColor;
  glm::vec4 specularColor;

friend class LightBuilder;
};

// -----------------------------------------------------------------------------
class DirectionalLight : public Light {
private:
  DirectionalLight(const glm::vec3& direction);

public:
  virtual ~DirectionalLight() {};

  void draw(ShaderProgram &shaderProgram, const glm::mat4 &modelView) const;
  void setDirection(const glm::vec3& direction);
  const glm::vec4 &getDirection();

private:
  glm::vec4 direction;

friend class LightBuilder;
};

// -----------------------------------------------------------------------------
class PositionalLight : public Light {
protected:
  PositionalLight(const glm::vec3& position);

public:
  virtual ~PositionalLight() {};

  void setPosition(const glm::vec3& position);
  const glm::vec4 &getPosition();

  void setConstantAttenuation(float attenuation);
  float getConstantAttenuation();

  void setLinearAttenuation(float attenuation);
  float getLinearAttenuation();

  void setQuadraticAttenuation(float attenuation);
  float getQuadraticAttenuation();

  void draw(ShaderProgram &shaderProgram, const glm::mat4 &modelView) const;

protected:
  glm::vec4 position;
  float constantAttenuation;
  float linearAttenuation;
  float quadraticAttenuation;

friend class LightBuilder;
};

// -----------------------------------------------------------------------------
class SpotLight : public PositionalLight {
private:
  SpotLight(const glm::vec3& position, const glm::vec3& direction);

public:
  virtual ~SpotLight() {}

  void setDirection(const glm::vec3& direction);
  const glm::vec4 &getDirection();

  void setCutOff(float cutOff);
  float getCutOff();

  void setExponent(float exponent);
  float getExponent();

  void draw(ShaderProgram &shaderProgram, const glm::mat4 &modelView) const;

private:
  glm::vec4 direction;
  float cutOff;
  float exponent;
 
friend class LightBuilder; 
};

// -----------------------------------------------------------------------------
class LightBuilder {
public:
  LightBuilder();

public:
  LightBuilder& setPosition(const glm::vec3& position);
  LightBuilder& setDirection(const glm::vec3& direction);

  LightBuilder& setAmbientColor(const glm::vec4& color);
  LightBuilder& setDiffuseColor(const glm::vec4& color);
  LightBuilder& setSpecularColor(const glm::vec4& color);

  LightBuilder& setCutOff(float cutOff);
  LightBuilder& setSpotExponent(float spotExponent);
  LightBuilder& setConstantAttenuation(float attenuation);
  LightBuilder& setLinearAttenuation(float attenuation);
  LightBuilder& setQuadraticAttenuation(float attenuation);

  DirectionalLight* createDirectional(); 
  PositionalLight* createPositional();
  SpotLight* createSpot();

private:
  void setup();
  void createHelper(Light* light);

private:
  int counter;
  // Light attributes.
  glm::vec3 position;
  glm::vec3 direction;

  glm::vec4 ambientColor;
  glm::vec4 diffuseColor;
  glm::vec4 specularColor;

  float cutOff;
  float spotExponent;
  float constantAttenuation;
  float linearAttenuation;
  float quadraticAttenuation;
};
