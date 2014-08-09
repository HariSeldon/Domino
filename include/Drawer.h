#pragma once

#include <GL/glew.h>

#include <glm/fwd.hpp>

#include <unordered_map>
#include <memory>
#include <vector>

class Box;
class Object;
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
  void initGPUObjects(const ShaderProgram &shaderProgram, const World &world);

  // Drawing functions.
  void drawObject(const Object *object, ShaderProgram &shaderProgram,
                  glm::mat4 &original) const;

private:
  GLuint setupVertexVBO(const Object *object, const ShaderProgram &shaderProgram);
  GLuint setupNormalVBO(const Object *object, const ShaderProgram &shaderProgram);
  GLuint setupIndexVBO(const Object *object, const ShaderProgram &shaderProgram);

private:
  // Maps world objects (identifies by their memory locations) 
  // to VAOs. 
  std::unordered_map<intptr_t, GLuint> vaoMap;
  // Ids of VBOs associated with the VAOs. These are kept so I know what to delete
  // to free the memory.
  std::vector<GLuint> vboIds;
};
