#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class LightedObjectShader;

// -----------------------------------------------------------------------------
class Light {
public:
  static const glm::vec4 DEFAULT_AMBIENT_COLOR;
  static const glm::vec4 DEFAULT_DIFFUSE_COLOR;
  static const glm::vec4 DEFAULT_SPECULAR_COLOR;
  static const glm::vec3 DEFAULT_DIRECTION;
  static const glm::vec3 DEFAULT_POSITION;
  static const float DEFAULT_SPOT_CUTOFF;
  static const float DEFAULT_SPOT_EXPONENT;
  static const float DEFAULT_CONSTANT_ATTENUATION;
  static const float DEFAULT_LINEAR_ATTENUATION;
  static const float DEFAULT_QUADRATIC_ATTENUATION;

public:
  virtual ~Light(){};

  virtual void setUniforms(const LightedObjectShader &shader,
                           const glm::mat4 &modelView) const;

  void setDiffuseColor(const glm::vec4 &color);
  const glm::vec4 &getDiffuseColor() const;

  void setAmbientColor(const glm::vec4 &color);
  const glm::vec4 &getAmbientColor() const;

  void setSpecularColor(const glm::vec4 &color);
  const glm::vec4 &getSpecularColor() const;

protected:
  int m_number;
  glm::vec4 m_ambientColor;
  glm::vec4 m_diffuseColor;
  glm::vec4 m_specularColor;

  friend class LightBuilder;
};

// -----------------------------------------------------------------------------
class DirectionalLight : public Light {
private:
  DirectionalLight(const glm::vec3 &direction);

public:
  virtual ~DirectionalLight(){};

  void setUniforms(const LightedObjectShader &shader,
                   const glm::mat4 &modelView) const override;
  void setDirection(const glm::vec3 &direction);
  const glm::vec4 &getDirection();

private:
  glm::vec4 m_direction;

  friend class LightBuilder;
};

// -----------------------------------------------------------------------------
class PositionalLight : public Light {
protected:
  PositionalLight(const glm::vec3 &position);

public:
  virtual ~PositionalLight(){};

  void setPosition(const glm::vec3 &position);
  const glm::vec4 &getPosition();

  void setConstantAttenuation(float attenuation);
  float getConstantAttenuation();

  void setLinearAttenuation(float attenuation);
  float getLinearAttenuation();

  void setQuadraticAttenuation(float attenuation);
  float getQuadraticAttenuation();

  void setUniforms(const LightedObjectShader &shader,
                   const glm::mat4 &modelView) const override;

protected:
  glm::vec4 m_position;
  float m_constantAttenuation;
  float m_linearAttenuation;
  float m_quadraticAttenuation;

  friend class LightBuilder;
};

// -----------------------------------------------------------------------------
class SpotLight : public PositionalLight {
private:
  SpotLight(const glm::vec3 &position, const glm::vec3 &direction);

public:
  virtual ~SpotLight() {}

  void setDirection(const glm::vec3 &direction);
  const glm::vec4 &getDirection();

  void setCutOff(float cutOff);
  float getCutOff();

  void setExponent(float exponent);
  float getExponent();

  void setUniforms(const LightedObjectShader &shader,
                   const glm::mat4 &modelView) const override;

private:
  glm::vec4 m_direction;
  float m_cutOff;
  float m_exponent;

  friend class LightBuilder;
};

// -----------------------------------------------------------------------------
class LightBuilder {
public:
  LightBuilder();

public:
  LightBuilder &setPosition(const glm::vec3 &position);
  LightBuilder &setDirection(const glm::vec3 &direction);

  LightBuilder &setAmbientColor(const glm::vec4 &color);
  LightBuilder &setDiffuseColor(const glm::vec4 &color);
  LightBuilder &setSpecularColor(const glm::vec4 &color);

  LightBuilder &setCutOff(float cutOff);
  LightBuilder &setSpotExponent(float spotExponent);
  LightBuilder &setConstantAttenuation(float attenuation);
  LightBuilder &setLinearAttenuation(float attenuation);
  LightBuilder &setQuadraticAttenuation(float attenuation);

  DirectionalLight *createDirectional();
  PositionalLight *createPositional();
  SpotLight *createSpot();

private:
  void setup();
  void createHelper(Light *light);

private:
  int m_counter = 0;
  // Light attributes.
  glm::vec3 m_position;
  glm::vec3 m_direction;

  glm::vec4 m_ambientColor;
  glm::vec4 m_diffuseColor;
  glm::vec4 m_specularColor;

  float m_cutOff = 0.f;
  float m_spotExponent = 0.f;
  float m_constantAttenuation = 0.f;
  float m_linearAttenuation = 0.f;
  float m_quadraticAttenuation = 0.f;
};
