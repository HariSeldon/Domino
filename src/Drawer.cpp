#include "Drawer.h"

#include "Box.h"
#include "Plane.h"
#include "MathUtils.h"
#include "Mirror.h"
#include "Object.h"
#include "ShaderProgram.h"
#include "World.h"

#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/ext.hpp>

#include <algorithm>
#include <iostream>

//-----------------------------------------------------------------------------
void setColors(const Object *object, ShaderProgram &shader);
void setOrientation(const Object *object, ShaderProgram &shader,
                    const glm::mat4 &originalModelView);
void setOrientationForShadow(const Object *object, ShaderProgram &shader,
                             const glm::mat4 &originalModelView,
                             const glm::mat4 &projection);

//-----------------------------------------------------------------------------
void Drawer::initGPUObjects(const ShaderProgram &shader,
                            const World &world) {
  initWorldObjects(shader, world);
}

//-----------------------------------------------------------------------------
void Drawer::initWorldObjects(const ShaderProgram &shader,
                              const World &world) {
  std::for_each(constBeginObjects(world), constEndObjects(world),
                [&](const Object *object) {
    intptr_t objectAddress = reinterpret_cast<intptr_t>(object);
    GLuint vaoId = 0;
    // Create VAO.
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    GLuint vertexVBOId = setupVertexVBO(object, shader);
    GLuint indexVBOId = setupIndexVBO(object);
    GLuint normalVBOId = setupNormalVBO(object, shader);

    // Unbind.
    glBindVertexArray(0);

    vaoMap[objectAddress] = vaoId;
    vboIds.insert(vboIds.end(), { vertexVBOId, indexVBOId, normalVBOId });
  });
}

//-----------------------------------------------------------------------------
void Drawer::initMirror(const ShaderProgram &shader,
                        const Mirror *mirror) {
  intptr_t objectAddress = reinterpret_cast<intptr_t>(mirror);
  GLuint vaoId = 0;
  // Create VAO.
  glGenVertexArrays(1, &vaoId);
  glBindVertexArray(vaoId);

  GLuint vertexVBOId = setupVertexVBO(mirror, shader);
  GLuint indexVBOId = setupIndexVBO(mirror);
  GLuint textureVBOId = setupTextureVBO(mirror, shader);

  // Unbind.
  glBindVertexArray(0);

  vaoMap[objectAddress] = vaoId;
  vboIds.insert(vboIds.end(), { vertexVBOId, indexVBOId, textureVBOId });
}

//-----------------------------------------------------------------------------
GLuint Drawer::setupVertexVBO(const Object *object,
                              const ShaderProgram &shader) {
  GLuint vertexVBOId = 0;
  glGenBuffers(1, &vertexVBOId);
  glBindBuffer(GL_ARRAY_BUFFER, vertexVBOId);
  glBufferData(GL_ARRAY_BUFFER, object->getPointsNumber() * sizeof(glm::vec3),
               object->getPoints(), GL_STATIC_DRAW);
  shader.setAttribute("vertexPosition", 3, GL_FLOAT);
  return vertexVBOId;
}

//-----------------------------------------------------------------------------
GLuint Drawer::setupNormalVBO(const Object *object,
                              const ShaderProgram &shader) {
  GLuint normalVBOId = 0;
  glGenBuffers(1, &normalVBOId);
  glBindBuffer(GL_ARRAY_BUFFER, normalVBOId);
  glBufferData(GL_ARRAY_BUFFER, object->getPointsNumber() * sizeof(glm::vec3),
               object->getNormals(), GL_STATIC_DRAW);
  shader.setAttribute("vertexNormal", 3, GL_FLOAT);
  return normalVBOId;
}

//-----------------------------------------------------------------------------
GLuint Drawer::setupIndexVBO(const Object *object) {
  GLuint indexVBOId = 0;
  glGenBuffers(1, &indexVBOId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               object->getIndicesNumber() * sizeof(unsigned int),
               object->getIndices(), GL_STATIC_DRAW);
  return indexVBOId;
}

//-----------------------------------------------------------------------------
GLuint Drawer::setupTextureVBO(const Object *object,
                               const ShaderProgram &shader) {
  GLuint textureVBOId = 0;
  glGenBuffers(1, &textureVBOId);
  glBindBuffer(GL_ARRAY_BUFFER, textureVBOId);
  glBufferData(GL_ARRAY_BUFFER, object->getPointsNumber() * sizeof(glm::vec2),
               object->getTextureCoos(), GL_STATIC_DRAW);
  shader.setAttribute("vertexTextureCoordinates", 2, GL_FLOAT);
  return textureVBOId;
}

//-----------------------------------------------------------------------------
Drawer::~Drawer() {

  for (auto iter : vaoMap)
    glDeleteVertexArrays(1, &iter.second);

  if (vboIds.size() > 0)
    glDeleteBuffers(vboIds.size(), vboIds.data());
}

//-----------------------------------------------------------------------------
void Drawer::drawObject(const Object *object, ShaderProgram &shader,
                        const glm::mat4 &originalModelView) const {

  setColors(object, shader);
  setOrientation(object, shader, originalModelView);

  GLuint vao = vaoMap.at(reinterpret_cast<intptr_t>(object));
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, object->getIndicesNumber(), GL_UNSIGNED_INT,
                 nullptr);
  glBindVertexArray(0);
}

//------------------------------------------------------------------------------
void Drawer::drawObjectForShadow(const Object *object, ShaderProgram &shader,
                                 const glm::mat4 &originalModelView, 
                                 const glm::mat4 &projection) const {
  setOrientationForShadow(object, shader, originalModelView, projection);

  GLuint vao = vaoMap.at(reinterpret_cast<intptr_t>(object));
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, object->getIndicesNumber(), GL_UNSIGNED_INT,
                 nullptr);
  glBindVertexArray(0);
}

//------------------------------------------------------------------------------
void setColors(const Object *object, ShaderProgram &shader) {
  shader.setUniform("material.ambient", object->getAmbientColor());
  shader.setUniform("material.diffuse", object->getDiffuseColor());
  shader.setUniform("material.specular", object->getSpecularColor());
  shader.setUniform("material.shininess", object->getShininess());
}

//------------------------------------------------------------------------------
void setOrientation(const Object *object, ShaderProgram &shader,
                    const glm::mat4 &originalModelView) {
  btScalar transformMatrix[16];
  object->getOpenGLMatrix(transformMatrix);
  glm::mat4 modelViewMatrix = glm::make_mat4x4(transformMatrix);

  modelViewMatrix = originalModelView * modelViewMatrix;
  shader.setUniform("modelViewMatrix", modelViewMatrix);
  shader.setUniform("normalMatrix",
                           glm::inverseTranspose(glm::mat3(modelViewMatrix)));
}

//------------------------------------------------------------------------------
void setOrientationForShadow(const Object *object, ShaderProgram &shader,
                             const glm::mat4 &originalModelView, 
                             const glm::mat4 &projection) {
  btScalar transformMatrix[16];
  object->getOpenGLMatrix(transformMatrix);
  glm::mat4 modelViewMatrix = glm::make_mat4x4(transformMatrix);

  glm::mat4 mvpMatrix = projection * originalModelView * modelViewMatrix;
  shader.setUniform("mvpMatrix", mvpMatrix);
}
