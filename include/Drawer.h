#pragma once

#include <GL/glew.h>

#include <glm/fwd.hpp>

#include <unordered_map>
#include <memory>
#include <vector>

class Box;
class Object;
class Mirror;
class Plane;
class ShaderProgram;
class Sphere;
class Wedge;
class World;

class btVector3;

class Drawer {
public:
  ~Drawer();

public:
  void initGPUObjects(const ShaderProgram &shader, const World &world);
  void initMirror(const ShaderProgram &shader, const Mirror* mirror);

  // Drawing functions.
  void drawObject(const Object *object, ShaderProgram &shader,
                  const glm::mat4 &original) const;
  void drawObjectForShadow(const Object *object, ShaderProgram &shader,
                           const glm::mat4 &modelView,
                           const glm::mat4 &projection) const;

private:
  void initWorldObjects(const ShaderProgram &shader, const World &world);

  GLuint setupVertexVBO(const Object *object, const ShaderProgram &shader);
  GLuint setupNormalVBO(const Object *object, const ShaderProgram &shader);
  GLuint setupIndexVBO(const Object *object);
  GLuint setupTextureVBO(const Object *object, const ShaderProgram &shader);

private:
  // Maps world objects (identifies by their memory locations) 
  // to VAOs. 
  std::unordered_map<intptr_t, GLuint> vaoMap;
  // Ids of VBOs associated with the VAOs. These are kept so I know what to delete
  // to free the memory.
  std::vector<GLuint> vboIds;
};
