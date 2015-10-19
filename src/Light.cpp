#include "Light.h"

#include "LightedObjectShader.h"

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <string>

const glm::vec4 Light::DEFAULT_AMBIENT_COLOR = {0.f, 0.f, 0.f, 1.f};
const glm::vec4 Light::DEFAULT_DIFFUSE_COLOR = {1.f, 1.f, 1.f, 1.f};
const glm::vec4 Light::DEFAULT_SPECULAR_COLOR = {0.f, 0.f, 0.f, 1.f};
const glm::vec3 Light::DEFAULT_DIRECTION = {0.f, 0.f, -1.f};
const glm::vec3 Light::DEFAULT_POSITION = {0.f, 0.f, 0.f};
const float Light::DEFAULT_SPOT_CUTOFF = 180.0f;
const float Light::DEFAULT_SPOT_EXPONENT = 0.0f;
const float Light::DEFAULT_CONSTANT_ATTENUATION = 1.0f;
const float Light::DEFAULT_LINEAR_ATTENUATION = 0.0f;
const float Light::DEFAULT_QUADRATIC_ATTENUATION = 0.0f;

// -----------------------------------------------------------------------------
void Light::setAmbientColor(const glm::vec4 &color) { m_ambientColor = color; }

const glm::vec4 &Light::getAmbientColor() const { return m_ambientColor; }

void Light::setDiffuseColor(const glm::vec4 &color) { m_diffuseColor = color; }

const glm::vec4 &Light::getDiffuseColor() const { return m_diffuseColor; }

void Light::setSpecularColor(const glm::vec4 &color) { m_specularColor = color; }

const glm::vec4 &Light::getSpecularColor() const { return m_specularColor; }

void Light::setUniforms(const LightedObjectShader &shader,
                        const glm::mat4 &) const {
  shader.setLightUniform(m_number, LightedObjectShader::lightAmbient,
                        m_ambientColor);
  shader.setLightUniform(m_number, LightedObjectShader::lightDiffuse,
                        m_diffuseColor);
  shader.setLightUniform(m_number, LightedObjectShader::lightSpecular,
                          m_specularColor);
}

// -----------------------------------------------------------------------------
DirectionalLight::DirectionalLight(const glm::vec3 &direction) : Light() {
  m_direction = glm::vec4(direction, 0.0f);
}

void DirectionalLight::setDirection(const glm::vec3 &direction) {
  m_direction = glm::vec4(direction, 0.0f);
}

const glm::vec4 &DirectionalLight::getDirection() { return m_direction; }

void DirectionalLight::setUniforms(const LightedObjectShader &shader,
                                   const glm::mat4 &modelView) const {
  glm::mat4 modelViewRotation(modelView);
  modelViewRotation[3][0] = 0.0f;
  modelViewRotation[3][1] = 0.0f;
  modelViewRotation[3][2] = 0.0f;

  glm::vec4 screenSpaceDirection = modelViewRotation * m_direction;
  glm::normalize(screenSpaceDirection);

  Light::setUniforms(shader, modelView);
  shader.setLightUniform(m_number, LightedObjectShader::lightPosition,
                          screenSpaceDirection);
}

// -----------------------------------------------------------------------------
PositionalLight::PositionalLight(const glm::vec3 &position) : Light() {
  m_position = glm::vec4(position, 1.0f);
}

void PositionalLight::setPosition(const glm::vec3 &position) {
  m_position = glm::vec4(position, 1.0f);
}

const glm::vec4 &PositionalLight::getPosition() { return m_position; }

void PositionalLight::setConstantAttenuation(float attenuation) {
  m_constantAttenuation = attenuation;
}

float PositionalLight::getConstantAttenuation() { return m_constantAttenuation; }

void PositionalLight::setLinearAttenuation(float attenuation) {
  m_linearAttenuation = attenuation;
}

float PositionalLight::getLinearAttenuation() { return m_linearAttenuation; }

void PositionalLight::setQuadraticAttenuation(float attenuation) {
  m_quadraticAttenuation = attenuation;
}

float PositionalLight::getQuadraticAttenuation() {
  return m_quadraticAttenuation;
}

void PositionalLight::setUniforms(const LightedObjectShader &shader,
                                  const glm::mat4 &modelView) const {
  Light::setUniforms(shader, modelView);
  shader.setLightUniform(m_number, LightedObjectShader::lightConstantAttenuation,
                        m_constantAttenuation);
  shader.setLightUniform(m_number, LightedObjectShader::lightLinearAttenuation,

                        m_linearAttenuation);
  shader.setLightUniform(m_number,
                        LightedObjectShader::lightQuadraticAttenuation,
                        m_quadraticAttenuation);
  shader.setLightUniform(m_number,
                        LightedObjectShader::lightSpotCutOff,
                        Light::DEFAULT_SPOT_CUTOFF);
  shader.setLightUniform(m_number, LightedObjectShader::lightPosition,
                          modelView * m_position);
}

// -----------------------------------------------------------------------------
SpotLight::SpotLight(const glm::vec3 &position, const glm::vec3 &direction)
    : PositionalLight(position) {
  m_direction = glm::vec4(direction, 0.0f);
}

void SpotLight::setDirection(const glm::vec3 &direction) {
  m_direction = glm::vec4(direction, 0.0f);
}

const glm::vec4 &SpotLight::getDirection() { return m_direction; }

void SpotLight::setCutOff(float cutOff) { m_cutOff = cutOff; }

float SpotLight::getCutOff() { return m_cutOff; }

void SpotLight::setExponent(float exponent) { m_exponent = exponent; }

float SpotLight::getExponent() { return m_exponent; }

void SpotLight::setUniforms(const LightedObjectShader &shader,
                            const glm::mat4 &modelView) const {
  glm::mat4 modelViewRotation(modelView);
  modelViewRotation[3][0] = 0.0f;
  modelViewRotation[3][1] = 0.0f;
  modelViewRotation[3][2] = 0.0f;

  glm::vec4 screenSpaceDirection = modelViewRotation * m_direction;
  glm::normalize(screenSpaceDirection);

  PositionalLight::setUniforms(shader, modelView);
  shader.setLightUniform(m_number, LightedObjectShader::lightSpotCutOff, m_cutOff);
  shader.setLightUniform(m_number, LightedObjectShader::lightSpotCosCutOff,
                         glm::cos(glm::radians(m_cutOff)));
  shader.setLightUniform(m_number, LightedObjectShader::lightSpotExponent,
                         m_exponent);
  shader.setLightUniform(m_number, LightedObjectShader::lightSpotDirection,
                          glm::vec3(screenSpaceDirection));
}

// -----------------------------------------------------------------------------
LightBuilder::LightBuilder() { setup(); }

void LightBuilder::setup() {
  m_ambientColor = Light::DEFAULT_AMBIENT_COLOR;
  m_diffuseColor = Light::DEFAULT_DIFFUSE_COLOR;
  m_specularColor = Light::DEFAULT_SPECULAR_COLOR;

  m_direction = Light::DEFAULT_DIRECTION;
  m_position = Light::DEFAULT_POSITION;

  m_constantAttenuation = Light::DEFAULT_CONSTANT_ATTENUATION;
  m_linearAttenuation = Light::DEFAULT_LINEAR_ATTENUATION;
  m_quadraticAttenuation = Light::DEFAULT_QUADRATIC_ATTENUATION;

  m_cutOff = Light::DEFAULT_SPOT_CUTOFF;
  m_spotExponent = Light::DEFAULT_SPOT_EXPONENT;
}

LightBuilder &LightBuilder::setPosition(const glm::vec3 &position) {
  m_position = position;
  return *this;
}

LightBuilder &LightBuilder::setDirection(const glm::vec3 &direction) {
  m_direction = direction;
  return *this;
}

LightBuilder &LightBuilder::setAmbientColor(const glm::vec4 &color) {
  m_ambientColor = color;
  return *this;
}

LightBuilder &LightBuilder::setDiffuseColor(const glm::vec4 &color) {
  m_diffuseColor = color;
  return *this;
}

LightBuilder &LightBuilder::setSpecularColor(const glm::vec4 &color) {
  m_specularColor = color;
  return *this;
}

LightBuilder &LightBuilder::setCutOff(float cutOff) {
  m_cutOff = cutOff;
  return *this;
}

LightBuilder &LightBuilder::setSpotExponent(float spotExponent) {
  m_spotExponent = spotExponent;
  return *this;
}

LightBuilder &LightBuilder::setConstantAttenuation(float attenuation) {
  m_constantAttenuation = attenuation;
  return *this;
}

LightBuilder &LightBuilder::setLinearAttenuation(float attenuation) {
  m_linearAttenuation = attenuation;
  return *this;
}

LightBuilder &LightBuilder::setQuadraticAttenuation(float attenuation) {
  m_quadraticAttenuation = attenuation;
  return *this;
}

void LightBuilder::createHelper(Light *light) {
  light->m_number = m_counter;
  ++m_counter;
  light->setAmbientColor(m_ambientColor);
  light->setDiffuseColor(m_diffuseColor);
  light->setSpecularColor(m_specularColor);
}

DirectionalLight *LightBuilder::createDirectional() {
  DirectionalLight *light = new DirectionalLight(m_direction);
  createHelper(light);
  setup();
  return light;
}

PositionalLight *LightBuilder::createPositional() {
  PositionalLight *light = new PositionalLight(m_position);
  createHelper(light);
  light->setConstantAttenuation(m_constantAttenuation);
  light->setLinearAttenuation(m_linearAttenuation);
  light->setQuadraticAttenuation(m_quadraticAttenuation);
  setup();
  return light;
}

SpotLight *LightBuilder::createSpot() {
  SpotLight *light = new SpotLight(m_position, m_direction);
  createHelper(light);
  light->setCutOff(m_cutOff);
  light->setExponent(m_spotExponent);
  light->setConstantAttenuation(m_constantAttenuation);
  light->setLinearAttenuation(m_linearAttenuation);
  light->setQuadraticAttenuation(m_quadraticAttenuation);
  setup();
  return light;
}
