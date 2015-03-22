#include "Drawer.h"

#include "Box.h"
#include "Light.h"
#include "MathUtils.h"
#include "Mirror.h"
#include "Object.h"
#include "Plane.h"
#include "ShaderProgram.h"
#include "SysDefines.h"
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
// Returns the fboId and the textureId.
std::pair<GLuint, GLuint> generateFBOColor();
GLuint createDBO(GLuint fboId);

//-----------------------------------------------------------------------------
void Drawer::initGPUObjects(
    const std::map<const std::string, std::vector<const Object *>> &shaderFileMap,
    const World &world) {

  for (auto x : shaderFileMap) {
    const std::string shaderName = x.first;
    auto objectVector = x.second;

    ShaderProgram *program =
        new ShaderProgram(shaderName, shaderName + ".vert", shaderName + ".frag");

    shaderMap[program] = objectVector;
  }

  for (auto x : shaderMap) {
    const ShaderProgram *shader = x.first;
    const auto objectVector = x.second;

    for (const auto &object : objectVector) {

      GLuint vaoId = 0;
      // Create VAO.
      glGenVertexArrays(1, &vaoId);
      glBindVertexArray(vaoId);

      GLuint vertexVBOId = setupVertexVBO(object, *shader);
      GLuint indexVBOId = setupIndexVBO(object);
      GLuint normalVBOId = setupNormalVBO(object, *shader);
      GLuint textureVBOId = setupTextureVBO(object, *shader);

      // Unbind.
      glBindVertexArray(0);

      vaoWorldMap.insert(std::pair<const Object *, GLuint>(object, vaoId));
      vboIds.insert(vboIds.end(),
                    {vertexVBOId, indexVBOId, normalVBOId, textureVBOId});
    }
  }

  // Setup canvas.
  canvasShader =
      new ShaderProgram("canvas", "canvas.vert", "canvas.frag");
  glGenVertexArrays(1, &canvasId);
  glBindVertexArray(canvasId);

  // Setup vertices.
  std::vector<glm::vec2> canvasPoints = {{-1, 1}, {1, 1}, {1, -1}, {-1, -1}};
  GLuint canvasVertexVBOId = 0;
  glGenBuffers(1, &canvasVertexVBOId);
  glBindBuffer(GL_ARRAY_BUFFER, canvasVertexVBOId);
  glBufferData(GL_ARRAY_BUFFER, canvasPoints.size() * sizeof(glm::vec2),
               canvasPoints.data(), GL_STATIC_DRAW);
  canvasShader->setAttribute("vertexPosition", 2, GL_FLOAT);
  
  // Setup indices.
  std::vector<unsigned int> canvasIndices = {0, 1, 2, 2, 3, 0};
  GLuint canvasIndexVBOId = 0;
  glGenBuffers(1, &canvasIndexVBOId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, canvasIndexVBOId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, canvasIndices.size() * sizeof(unsigned int),
               canvasIndices.data(), GL_STATIC_DRAW);

  glBindVertexArray(0);
  vboIds.insert(vboIds.end(), {canvasVertexVBOId, canvasIndexVBOId});
  

  // Extract the lightBulb shader program.
  // See if there are any lightBulbs, if not, specialize the draw function so to have a single, pass.
  // If not fallback to the case in which use 2 passes with opencl.
  // I think this can be implemented with the Strategy Pattern.

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

        textureMap[object] = currentTexture;
      }
      SDL_FreeSurface(texSurface);
    }
  });

  auto lightBulbData = generateFBOColor(); 
  lightBulbFBOId = std::get<0>(lightBulbData);
  lightBulbTexture = std::get<1>(lightBulbData);

  auto sceneData = generateFBOColor(); 
  sceneFBOId = std::get<0>(sceneData);
  sceneTexture = std::get<1>(sceneData);
  
  assert(lightBulbFBOId != sceneFBOId);
  assert(lightBulbTexture != sceneTexture);

  dboId = createDBO(sceneFBOId);
}

//-----------------------------------------------------------------------------
void Drawer::initGPUShadowObjects(const ShaderProgram &shadowShader,
                                  const World &world) {
  for (auto mapIter : vaoWorldMap) {
    const Object *object = mapIter.first;

    GLuint vaoId = 0;

    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    // Bind the vertex buffer to the vao.
    // I should not need this.
    GLuint vertexVBOId = setupVertexVBO(object, shadowShader);
    GLuint indexVBOId = setupIndexVBO(object);

    glBindVertexArray(0);

    vaoShadowMap.insert(std::pair<const Object *, GLuint>(object, vaoId));
    vboIds.push_back(indexVBOId);
  }
}

//-----------------------------------------------------------------------------
void Drawer::initMirror(const ShaderProgram &shader, const Mirror *mirror) {
  GLuint vaoId = 0;
  // Create VAO.
  glGenVertexArrays(1, &vaoId);
  glBindVertexArray(vaoId);

  GLuint vertexVBOId = setupVertexVBO(mirror, shader);
  GLuint indexVBOId = setupIndexVBO(mirror);
  GLuint textureVBOId = setupTextureVBO(mirror, shader);

  // Unbind.
  glBindVertexArray(0);

  vaoWorldMap[mirror] = vaoId;
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

  // FIXME Remember to delete all the frame buffers.

}

//-----------------------------------------------------------------------------
void Drawer::drawObjects(const World *world, const glm::mat4 &originalModelView,
                         const glm::mat4 &projection,
                         const glm::mat4 &originalShadowModelView,
                         const glm::mat4 &shadowProjection,
                         int lightMask) const {

//  auto lightBulbIter = std::find_if(
//      shaderMap.begin(), shaderMap.end(),
//      [](const std::pair<ShaderProgram *, std::vector<const Object *>> &x) {
//        return x.first->getName() == "lightBulb";
//      });
//
//  return;
//
//
//

  glBindFramebuffer(GL_FRAMEBUFFER, sceneFBOId);
  checkOpenGLError("Drawer: drawObjects-glBindFrameBuffer");
  glViewport(0, 0, 1280, 800);
  
  glBindTexture(GL_TEXTURE_2D, sceneTexture);
  checkOpenGLError("Drawer: drawObjects-glBindTexture");

  glClearColor(0.1, 0.2, 0.7, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Iterate all the other shaders.
  for (auto &x : shaderMap) {
    ShaderProgram *shader = x.first;

    if(shader->getName() == "lightBulb")
      continue;

    const auto &objectVector = x.second;
    shader->useProgram();

    setLights(world, shader, originalModelView, lightMask);
    for (const auto &object : objectVector) {
      drawObject(object, *shader, originalModelView, projection,
                 originalShadowModelView, shadowProjection);
    }
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Get the LightBulb shader. 
  for (auto &iter : shaderMap) {
    ShaderProgram *shader = iter.first;
    if(shader->getName() == "lightBulb") {
      shader->useProgram();
      drawLightBulbs(shader, iter.second, originalModelView, projection,
                     originalShadowModelView, shadowProjection);
      break;
    }
  }

  // Merge the frame buffer with the lightBulb texture.

  // Second pass.
  canvasShader->useProgram();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, lightBulbTexture);
  canvasShader->setUniform("firstTexture", 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, sceneTexture);
  canvasShader->setUniform("secondTexture", 1);

  glBindVertexArray(canvasId);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

//-----------------------------------------------------------------------------
void Drawer::drawLightBulbs(ShaderProgram *shader,
                            const std::vector<const Object *> &objects,
                            const glm::mat4 &originalModelView,
                            const glm::mat4 &projection,
                            const glm::mat4 &originalShadowModelView,
                            const glm::mat4 &shadowProjection) const {
  glBindFramebuffer(GL_FRAMEBUFFER, lightBulbFBOId);
  checkOpenGLError("Drawer: drawLightBulbs-glBindFrameBuffer");

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, dboId);
  checkOpenGLError("Drawer: drawLightBulbs-glFramebufferRenderbuffer");

  glViewport(0, 0, 1280, 800);
  
  glBindTexture(GL_TEXTURE_2D, lightBulbTexture);
  checkOpenGLError("Drawer: drawLightBulbs-glBindTexture");

  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw!
  for (const auto &object : objects) {
    drawObject(object, *shader, originalModelView, projection,
               originalShadowModelView, shadowProjection);
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//-----------------------------------------------------------------------------
void Drawer::setLights(const World *world, ShaderProgram *shader,
                       const glm::mat4 &modelView, int lightMask) const {
  shader->setUniform("ambientColor", world->getAmbientColor());
  shader->setUniform("lightsNumber", world->getLightsNumber());
  shader->setUniform("lightMask", lightMask);
  std::for_each(constBeginLights(*world), constEndLights(*world),
                [&](const Light *light) { light->draw(*shader, modelView); });
}

//-----------------------------------------------------------------------------
void Drawer::drawObject(const Object *object, ShaderProgram &shader,
                        const glm::mat4 &originalModelView,
                        const glm::mat4 &projection,
                        const glm::mat4 &originalShadowModelView,
                        const glm::mat4 &shadowProjection) const {
  setColors(object, shader);
  if(shader.getName() == "phong") {
    glBindTexture(GL_TEXTURE_2D, textureMap.at(object));
    shader.setUniform("texture", 0);
  }
  setOrientation(object, shader, originalModelView, projection,
                 originalShadowModelView, shadowProjection);

  GLuint vao = vaoWorldMap.at(object);
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

  GLuint vao = vaoShadowMap.at(object);
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

  shader.setUniform("mvpMatrix", projection * modelView);
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

//------------------------------------------------------------------------------
std::pair<GLuint, GLuint> generateFBOColor() {
  GLuint fboId = 0;
  GLuint textureId = 0;

  // Init lightBulb Texture.
  glGenFramebuffers(1, &fboId);
  checkOpenGLError("generateFBOColor: glGenRenderbuffers");

  glGenTextures(1, &textureId);
  checkOpenGLError("generateFBOColor: glGenTextures");

  glBindTexture(GL_TEXTURE_2D, textureId);
  checkOpenGLError("generateFBOColor: glBindTexture");

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1280, 800, 0,
//               GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
               GL_RGBA, GL_FLOAT, nullptr);
  checkOpenGLError("generateFBOColor: glTexImage2D");

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  checkOpenGLError("generateFBOColor: glTexParameteri");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  checkOpenGLError("generateFBOColor: glTexParameteri");

  glBindFramebuffer(GL_FRAMEBUFFER, fboId);
  checkOpenGLError("generateFBOColor: attachTexture-glBindBuffer");
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);
  checkOpenGLError("generateFBOColor: attachTexture-glFramebufferTexture");

  GLenum drawBuffers = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers(1, &drawBuffers);
  checkOpenGLError("generateFBOColor: glDrawBuffers");

  assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE &&
         "Error setting frame buffer");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);  

  return std::make_pair(fboId, textureId);
}

//------------------------------------------------------------------------------
GLuint createDBO(GLuint fboId) {
  GLuint dboId = 0;
  glGenRenderbuffers(1, &dboId);
  checkOpenGLError("glGenRenderbuffers");

  glBindRenderbuffer(GL_RENDERBUFFER, dboId);
  checkOpenGLError("glBindRenderbuffer");

  // Define the size of the render buffer.
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 800);
  checkOpenGLError("glRenderbufferStorage");

  glBindFramebuffer(GL_FRAMEBUFFER, fboId);
  checkOpenGLError("glBindFramebuffer");
  // Attach the render buffer to the currently bound framebuffer.
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, dboId);
  checkOpenGLError("glFramebufferRenderbuffer");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  return dboId;
}
