#include "Light.h"

#include <string>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

const glm::vec4 Light::DEFAULT_AMBIENT_COLOR = { 0.f, 0.f, 0.f, 1.f };
const glm::vec4 Light::DEFAULT_DIFFUSE_COLOR = { 1.f, 1.f, 1.f, 1.f };
const glm::vec4 Light::DEFAULT_SPECULAR_COLOR = { 0.f, 0.f, 0.f, 1.f };
const glm::vec3 Light::DEFAULT_DIRECTION = { 0.f, 0.f, -1.f };
const glm::vec3 Light::DEFAULT_POSITION = { 0.f, 0.f, 0.f };
const float Light::DEFAULT_SPOT_CUTOFF = 180.0f;

// -----------------------------------------------------------------------------
void Light::setAmbientColor(const glm::vec4 &color) { ambientColor = color; }

const glm::vec4 &Light::getAmbientColor() const { return ambientColor; }

void Light::setDiffuseColor(const glm::vec4 &color) { diffuseColor = color; }

const glm::vec4 &Light::getDiffuseColor() const { return diffuseColor; }

void Light::setSpecularColor(const glm::vec4 &color) { specularColor = color; }

const glm::vec4 &Light::getSpecularColor() const { return specularColor; }

void Light::draw(ShaderProgram &shaderProgram,
                 const glm::mat4 &) const {
  shaderProgram.setUniform("lights[" + std::to_string(number) + "].ambient",
                           ambientColor);
  shaderProgram.setUniform("lights[" + std::to_string(number) + "].diffuse",
                           diffuseColor);
  shaderProgram.setUniform("lights[" + std::to_string(number) + "].specular",
                           specularColor);
  shaderProgram.setUniform("defaultSpotCutOff", Light::DEFAULT_SPOT_CUTOFF);
}

// -----------------------------------------------------------------------------
DirectionalLight::DirectionalLight(const glm::vec3 &direction) : Light() {
  this->direction = glm::vec4(direction, 0.0f);
}

void DirectionalLight::setDirection(const glm::vec3 &direction) {
  this->direction = glm::vec4(direction, 0.0f);
}

const glm::vec4 &DirectionalLight::getDirection() { return direction; }

void DirectionalLight::draw(ShaderProgram &shaderProgram,
                            const glm::mat4 &modelView) const {
  glm::mat4 modelViewRotation(modelView);
  modelViewRotation[3][0] = 0.0f;
  modelViewRotation[3][1] = 0.0f;
  modelViewRotation[3][2] = 0.0f;

  glm::vec4 screenSpaceDirection = modelViewRotation * direction;
  glm::normalize(screenSpaceDirection);

  Light::draw(shaderProgram, modelView);
  shaderProgram.setUniform("lights[" + std::to_string(number) + "].position",
                           screenSpaceDirection);
}

// -----------------------------------------------------------------------------
PositionalLight::PositionalLight(const glm::vec3 &position) : Light() {
  this->position = glm::vec4(position, 1.0f);
}

void PositionalLight::setPosition(const glm::vec3 &position) {
  this->position = glm::vec4(position, 1.0f);
}

const glm::vec4 &PositionalLight::getPosition() { return position; }

void PositionalLight::setConstantAttenuation(float attenuation) {
  this->constantAttenuation = attenuation;
}

float PositionalLight::getConstantAttenuation() { return constantAttenuation; }

void PositionalLight::setLinearAttenuation(float attenuation) {
  this->linearAttenuation = attenuation;
}

float PositionalLight::getLinearAttenuation() { return linearAttenuation; }

void PositionalLight::setQuadraticAttenuation(float attenuation) {
  this->quadraticAttenuation = attenuation;
}

float PositionalLight::getQuadraticAttenuation() {
  return quadraticAttenuation;
}

void PositionalLight::draw(ShaderProgram &shaderProgram,
                           const glm::mat4 &modelView) const {
  Light::draw(shaderProgram, modelView);
  shaderProgram.setUniform("lights[" + std::to_string(number) +
                               "].constantAttenuation",
                           constantAttenuation);
  shaderProgram.setUniform("lights[" + std::to_string(number) +
                               "].linearAttenuation",
                           linearAttenuation);
  shaderProgram.setUniform("lights[" + std::to_string(number) +
                               "].quadraticAttenuation",
                           quadraticAttenuation);
  shaderProgram.setUniform("lights[" + std::to_string(number) + "].spotCutOff",
                           180.0f);
  shaderProgram.setUniform("lights[" + std::to_string(number) + "].position",
                           modelView * position);
}

// -----------------------------------------------------------------------------
SpotLight::SpotLight(const glm::vec3 &position, const glm::vec3 &direction)
    : PositionalLight(position) {
  this->direction = glm::vec4(direction, 0.0f);
}

void SpotLight::setDirection(const glm::vec3 &direction) {
  this->direction = glm::vec4(direction, 0.0f);
}

const glm::vec4 &SpotLight::getDirection() { return direction; }

void SpotLight::setCutOff(float cutOff) { this->cutOff = cutOff; }

float SpotLight::getCutOff() { return cutOff; }

void SpotLight::setExponent(float exponent) { this->exponent = exponent; }

float SpotLight::getExponent() { return exponent; }

void SpotLight::draw(ShaderProgram &shaderProgram,
                     const glm::mat4 &modelView) const {
  glm::mat4 modelViewRotation(modelView);
  modelViewRotation[3][0] = 0.0f;
  modelViewRotation[3][1] = 0.0f;
  modelViewRotation[3][2] = 0.0f;

  glm::vec4 screenSpaceDirection = modelViewRotation * direction;
  glm::normalize(screenSpaceDirection);

  PositionalLight::draw(shaderProgram, modelView);

  shaderProgram.setUniform("lights[" + std::to_string(number) + "].spotCutOff",
                           cutOff);
  shaderProgram.setUniform(
      "lights[" + std::to_string(number) + "].spotExponent", exponent);
  shaderProgram.setUniform("lights[" + std::to_string(number) +
                               "].spotDirection",
                           glm::vec3(screenSpaceDirection));
}

// -----------------------------------------------------------------------------
LightBuilder::LightBuilder() : counter(0) { setup(); }

void LightBuilder::setup() {
  ambientColor = Light::DEFAULT_AMBIENT_COLOR;
  diffuseColor = Light::DEFAULT_DIFFUSE_COLOR;
  specularColor = Light::DEFAULT_SPECULAR_COLOR;

  direction = Light::DEFAULT_DIRECTION;
  position = Light::DEFAULT_POSITION;

  constantAttenuation = Light::DEFAULT_CONSTANT_ATTENUATION;
  linearAttenuation = Light::DEFAULT_LINEAR_ATTENUATION;
  quadraticAttenuation = Light::DEFAULT_QUADRATIC_ATTENUATION;

  cutOff = Light::DEFAULT_SPOT_CUTOFF;
  spotExponent = Light::DEFAULT_SPOT_EXPONENT;
}

LightBuilder &LightBuilder::setPosition(const glm::vec3 &position) {
  this->position = position;
  return *this;
}

LightBuilder &LightBuilder::setDirection(const glm::vec3 &direction) {
  this->direction = direction;
  return *this;
}

LightBuilder &LightBuilder::setAmbientColor(const glm::vec4 &color) {
  ambientColor = color;
  return *this;
}

LightBuilder &LightBuilder::setDiffuseColor(const glm::vec4 &color) {
  diffuseColor = color;
  return *this;
}

LightBuilder &LightBuilder::setSpecularColor(const glm::vec4 &color) {
  specularColor = color;
  return *this;
}

LightBuilder &LightBuilder::setCutOff(float cutOff) {
  this->cutOff = cutOff;
  return *this;
}

LightBuilder &LightBuilder::setSpotExponent(float spotExponent) {
  this->spotExponent = spotExponent;
  return *this;
}

LightBuilder &LightBuilder::setConstantAttenuation(float attenuation) {
  constantAttenuation = attenuation;
  return *this;
}

LightBuilder &LightBuilder::setLinearAttenuation(float attenuation) {
  linearAttenuation = attenuation;
  return *this;
}

LightBuilder &LightBuilder::setQuadraticAttenuation(float attenuation) {
  quadraticAttenuation = attenuation;
  return *this;
}

void LightBuilder::createHelper(Light *light) {
  light->number = counter;
  ++counter;
  light->setAmbientColor(ambientColor);
  light->setDiffuseColor(diffuseColor);
  light->setSpecularColor(specularColor);
}

DirectionalLight *LightBuilder::createDirectional() {
  DirectionalLight *light = new DirectionalLight(direction);
  createHelper(light);
  setup();
  return light;
}

PositionalLight *LightBuilder::createPositional() {
  PositionalLight *light = new PositionalLight(position);
  createHelper(light);
  light->setConstantAttenuation(constantAttenuation);
  light->setLinearAttenuation(linearAttenuation);
  light->setQuadraticAttenuation(quadraticAttenuation);
  setup();
  return light;
}

SpotLight *LightBuilder::createSpot() {
  SpotLight *light = new SpotLight(position, direction);
  createHelper(light);
  light->setCutOff(cutOff);
  light->setExponent(spotExponent);
  light->setConstantAttenuation(constantAttenuation);
  light->setLinearAttenuation(linearAttenuation);
  light->setQuadraticAttenuation(quadraticAttenuation);
  setup();
  return light;
}
