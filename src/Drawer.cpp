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

#include "SDL2/SDL_image.h"
#include "SDL2/SDL_pixels.h"

//-----------------------------------------------------------------------------
void setColors(const Object *object, ShaderProgram &shader);
void setOrientation(const Object *object, ShaderProgram &shader,
                    const glm::mat4 &originalModelView,
                    const glm::mat4 &projection,
                    const glm::mat4 &originalShadowModelView,
                    const glm::mat4 &shadowProjection);
void setOrientationForShadow(const Object *object, ShaderProgram &shader,
                             const glm::mat4 &originalModelView,
                             const glm::mat4 &projection);

//-----------------------------------------------------------------------------
void Drawer::initGPUObjects(const ShaderProgram &worldShader,
                            const World &world) {
  std::for_each(constBeginObjects(world), constEndObjects(world),
                [&](const Object *object) {
    intptr_t objectAddress = reinterpret_cast<intptr_t>(object);
    GLuint vaoId = 0;
    // Create VAO.
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    GLuint vertexVBOId = setupVertexVBO(object, worldShader);
    GLuint indexVBOId = setupIndexVBO(object);
    GLuint normalVBOId = setupNormalVBO(object, worldShader);
    GLuint textureVBOId = setupTextureVBO(object, worldShader);

    // Unbind.
    glBindVertexArray(0);

    vaoWorldMap[objectAddress] = vaoId;
    vboIds.insert(vboIds.end(),
                  {vertexVBOId, indexVBOId, normalVBOId, textureVBOId});
  });
}

//-----------------------------------------------------------------------------
void Drawer::initTextures(const World &world) {
  // Load textures.
  std::for_each(constBeginObjects(world), constEndObjects(world),
                [&](const Object *object) {
    const std::string &textureFile = object->getTextureFile();
    if (!textureFile.empty()) {
      SDL_Surface *texSurface = IMG_Load(textureFile.c_str());
      if (texSurface) {
        intptr_t objectAddress = reinterpret_cast<intptr_t>(object);

        GLuint currentTexture = 0;
        // Create the texture object.
        glGenTextures(1, &currentTexture);
        checkOpenGLError("Drawer: glGenTextures");

        glBindTexture(GL_TEXTURE_2D, currentTexture);
        checkOpenGLError("Drawer: glBindTexture");

        GLint format = 0;
        switch (texSurface->format->BytesPerPixel) {
        case 3:
          format = GL_RGB;
          break;
        case 4:
          format = GL_RGBA;
          break;
        default:
          std::cerr << "Unknown texture format.\n";
          exit(1);
        };

        glTexImage2D(GL_TEXTURE_2D, 0, format, texSurface->w, texSurface->h, 0,
                     format, GL_UNSIGNED_BYTE, texSurface->pixels);
        checkOpenGLError("Drawer: glTexImage2D");

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        checkOpenGLError("Drawer: glTexParameteri");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        checkOpenGLError("Drawer: glTexParameteri");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        checkOpenGLError("Drawer: glTexParameteri");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        checkOpenGLError("Drawer: glTexParameteri");

        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        textureMap[objectAddress] = currentTexture;
      }
      SDL_FreeSurface(texSurface);
    }
  });
}

//-----------------------------------------------------------------------------
void Drawer::initGPUShadowObjects(const ShaderProgram &shadowShader,
                                  const World &world) {
  for (auto mapIter : vaoWorldMap) {
    intptr_t objectAddress = mapIter.first;
    Object *object = reinterpret_cast<Object *>(objectAddress);

    GLuint vaoId = 0;

    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    // Bind the vertex buffer to the vao.
    // I should not need this.
    GLuint vertexVBOId = setupVertexVBO(object, shadowShader);
    GLuint indexVBOId = setupIndexVBO(object);

    glBindVertexArray(0);

    vaoShadowMap[objectAddress] = vaoId;
    vboIds.push_back(indexVBOId);
  }
}

//-----------------------------------------------------------------------------
void Drawer::initMirror(const ShaderProgram &shader, const Mirror *mirror) {
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

  vaoWorldMap[objectAddress] = vaoId;
  vboIds.insert(vboIds.end(), {vertexVBOId, indexVBOId, textureVBOId});
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
GLuint Drawer::setupTextureVBO(const Object *object,
                               const ShaderProgram &shader) {
  GLuint texVBOId = 0;
  glGenBuffers(1, &texVBOId);
  glBindBuffer(GL_ARRAY_BUFFER, texVBOId);
  glBufferData(GL_ARRAY_BUFFER, object->getPointsNumber() * sizeof(glm::vec2),
               object->getTextureCoos(), GL_STATIC_DRAW);
  shader.setAttribute("vertexTextureCoordinates", 2, GL_FLOAT);
  return texVBOId;
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
Drawer::~Drawer() {
  for (auto iter : vaoWorldMap)
    glDeleteVertexArrays(1, &iter.second);

  if (vboIds.size() > 0)
    glDeleteBuffers(vboIds.size(), vboIds.data());
}

//-----------------------------------------------------------------------------
void Drawer::drawObject(const Object *object, ShaderProgram &shader,
                        const glm::mat4 &originalModelView,
                        const glm::mat4 &projection,
                        const glm::mat4 &originalShadowModelView,
                        const glm::mat4 &shadowProjection) const {
  setColors(object, shader);
  glBindTexture(GL_TEXTURE_2D,
                textureMap.at(reinterpret_cast<intptr_t>(object)));

  shader.setUniform("texture", 0);
  setOrientation(object, shader, originalModelView, projection,
                 originalShadowModelView, shadowProjection);

  GLuint vao = vaoWorldMap.at(reinterpret_cast<intptr_t>(object));
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, object->getIndicesNumber(), GL_UNSIGNED_INT,
                 nullptr);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

//------------------------------------------------------------------------------
void Drawer::drawObjectForShadow(const Object *object, ShaderProgram &shader,
                                 const glm::mat4 &originalModelView,
                                 const glm::mat4 &projection) const {
  setOrientationForShadow(object, shader, originalModelView, projection);

  GLuint vao = vaoShadowMap.at(reinterpret_cast<intptr_t>(object));
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
                    const glm::mat4 &originalModelView,
                    const glm::mat4 &projection,
                    const glm::mat4 &originalShadowModelView,
                    const glm::mat4 &shadowProjection) {
  btScalar transform[16];
  object->getOpenGLMatrix(transform);
  glm::mat4 modelView = glm::make_mat4x4(transform);

  glm::mat4 biasMatrix(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5,
                       0.0, 0.5, 0.5, 0.5, 1.0);
  glm::mat4 shadowMVP =
      biasMatrix * shadowProjection * originalShadowModelView * modelView;
  modelView = originalModelView * modelView;

  // FIXME I can avoid setting the projection uniform every time.
  // If I cannot integrate projection and model view hoist this out.
  shader.setUniform("projectionMatrix", projection);
  shader.setUniform("modelViewMatrix", modelView);
  shader.setUniform("normalMatrix",
                    glm::inverseTranspose(glm::mat3(modelView)));
  shader.setUniform("mvpShadowMatrix", shadowMVP);
}

//------------------------------------------------------------------------------
void setOrientationForShadow(const Object *object, ShaderProgram &shader,
                             const glm::mat4 &originalModelView,
                             const glm::mat4 &projection) {
  btScalar transform[16];
  object->getOpenGLMatrix(transform);
  glm::mat4 modelView = glm::make_mat4x4(transform);
  glm::mat4 mvp = projection * originalModelView * modelView;
  shader.setUniform("mvpMatrix", mvp);
}
