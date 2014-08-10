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
void setColors(const Object *object, ShaderProgram &shaderProgram);
void setOrientation(const Object *object, ShaderProgram &shaderProgram,
                    const glm::mat4 &originalModelView);

//-----------------------------------------------------------------------------
void Drawer::initGPUObjects(const ShaderProgram &shaderProgram,
                            const World &world) {

  initWorldObjects(shaderProgram, world);
}

//-----------------------------------------------------------------------------
void Drawer::initWorldObjects(const ShaderProgram &shaderProgram, const World &world) {
  std::for_each(constBeginObjects(world), constEndObjects(world),
                [&](const Object *object) {
    intptr_t objectAddress = reinterpret_cast<intptr_t>(object);
    GLuint vaoId = 0;
    // Create VAO.
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    GLuint vertexVBOId = setupVertexVBO(object, shaderProgram);
    GLuint indexVBOId = setupIndexVBO(object);
    GLuint normalVBOId = setupNormalVBO(object, shaderProgram);

    // Unbind.
    glBindVertexArray(0);

    vaoMap[objectAddress] = vaoId;
    vboIds.insert(vboIds.end(), { vertexVBOId, indexVBOId, normalVBOId });
  });
}

//-----------------------------------------------------------------------------
void Drawer::initMirror(const ShaderProgram &shaderProgram, const Mirror* mirror) {
  intptr_t objectAddress = reinterpret_cast<intptr_t>(mirror);
  GLuint vaoId = 0;
  // Create VAO.
  glGenVertexArrays(1, &vaoId);
  glBindVertexArray(vaoId);
  
  GLuint vertexVBOId = setupVertexVBO(mirror, shaderProgram);
  GLuint indexVBOId = setupIndexVBO(mirror);
  GLuint textureVBOId = setupTextureVBO(mirror, shaderProgram);

  // Unbind.
  glBindVertexArray(0);

  vaoMap[objectAddress] = vaoId;
  vboIds.insert(vboIds.end(), { vertexVBOId, indexVBOId, textureVBOId });
}

//-----------------------------------------------------------------------------
GLuint Drawer::setupVertexVBO(const Object *object,
                              const ShaderProgram &shaderProgram) {
  GLuint vertexVBOId = 0;
  glGenBuffers(1, &vertexVBOId);
  glBindBuffer(GL_ARRAY_BUFFER, vertexVBOId);
  glBufferData(GL_ARRAY_BUFFER, object->getPointsNumber() * sizeof(glm::vec3),
               object->getPoints(), GL_STATIC_DRAW);
  shaderProgram.setAttribute("vertexPosition", 3, GL_FLOAT);
  return vertexVBOId;
}

//-----------------------------------------------------------------------------
GLuint Drawer::setupNormalVBO(const Object *object,
                              const ShaderProgram &shaderProgram) {
  GLuint normalVBOId = 0;
  glGenBuffers(1, &normalVBOId);
  glBindBuffer(GL_ARRAY_BUFFER, normalVBOId);
  glBufferData(GL_ARRAY_BUFFER, object->getPointsNumber() * sizeof(glm::vec3),
               object->getNormals(), GL_STATIC_DRAW);
  shaderProgram.setAttribute("vertexNormal", 3, GL_FLOAT);
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
                               const ShaderProgram &shaderProgram) {
  GLuint textureVBOId = 0;
  glGenBuffers(1, &textureVBOId);
  glBindBuffer(GL_ARRAY_BUFFER, textureVBOId);
  glBufferData(GL_ARRAY_BUFFER, object->getPointsNumber() * sizeof(glm::vec2),
               object->getTextureCoos(), GL_STATIC_DRAW);
  shaderProgram.setAttribute("vertexTextureCoordinates", 2, GL_FLOAT);
  return textureVBOId;
}

//-----------------------------------------------------------------------------
Drawer::~Drawer() {
  glBindVertexArray(0);

  for (auto iter : vaoMap) {
    glDeleteVertexArrays(1, &iter.second);
  }

  if (vboIds.size() > 0)
    glDeleteBuffers(vboIds.size(), vboIds.data());
}

//-----------------------------------------------------------------------------
void Drawer::drawObject(const Object *object, ShaderProgram &shaderProgram,
                        const glm::mat4 &originalModelView) const {

  setColors(object, shaderProgram);
  setOrientation(object, shaderProgram, originalModelView);

  GLuint vao = vaoMap.at(reinterpret_cast<intptr_t>(object));
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, object->getIndicesNumber(), GL_UNSIGNED_INT,
                 nullptr);
  glBindVertexArray(0);
}

//------------------------------------------------------------------------------
void setColors(const Object *object, ShaderProgram &shaderProgram) {
  shaderProgram.setUniform("material.ambient", object->getAmbientColor());
  shaderProgram.setUniform("material.diffuse", object->getDiffuseColor());
  shaderProgram.setUniform("material.specular", object->getSpecularColor());
  shaderProgram.setUniform("material.shininess", object->getShininess());
}

//------------------------------------------------------------------------------
void setOrientation(const Object *object, ShaderProgram &shaderProgram,
                    const glm::mat4 &originalModelView) {
  btScalar transformMatrix[16];
  object->getOpenGLMatrix(transformMatrix);
  glm::mat4 modelViewMatrix = glm::make_mat4x4(transformMatrix);

  modelViewMatrix = originalModelView * modelViewMatrix;
  shaderProgram.setUniform("modelViewMatrix", modelViewMatrix);
  shaderProgram.setUniform("normalMatrix",
                           glm::inverseTranspose(glm::mat3(modelViewMatrix)));
}
