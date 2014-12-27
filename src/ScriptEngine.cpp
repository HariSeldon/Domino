#include "ScriptEngine.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Box.h"
#include "Camera.h"
#include "Light.h"
#include "Mesh.h"
#include "Plane.h"
#include "SysDefines.h"
#include "World.h"

Camera *tmpCamera = nullptr;
World *tmpWorld = nullptr;

static luaL_Reg ScriptEngineTable[] = {{nullptr, nullptr}};

static luaL_Reg ScriptEngineMetatable[] = {
    {"_setGravity", setGravity},
    {"_setCamera", setCamera},
    {"_addPositionalLight", addPositionalLight},
    {"_addDirectionalLight", addDirectionalLight},
    {"_addPlane", addPlane},
    {"_addMesh", addMesh},
    {"_addBox", addBox},
    {nullptr, nullptr}};

ScriptEngine* NewScriptEngine(lua_State* L) { 
  return new ScriptEngine(tmpWorld, tmpCamera); 
}

// =============================================================================
LuaState::LuaState() : luaState(luaL_newstate()) {
  luaL_openlibs(luaState);
}

// -----------------------------------------------------------------------------
LuaState::~LuaState() {
  lua_close(luaState);
}

// -----------------------------------------------------------------------------
void LuaState::runScript(const std::string &scriptFile) {
  luaW_register<ScriptEngine>(luaState, "ScriptEngine", ScriptEngineTable,
                              ScriptEngineMetatable, NewScriptEngine);
  int errors = luaL_dofile(luaState, scriptFile.c_str());
  if (errors) {
    std::cout << "Error running lua script: " << scriptFile << "\n"
              << luaL_checkstring(luaState, -1) << std::endl;
    exit(1);
  }
}

// =============================================================================
ScriptEngine::ScriptEngine(World *world, Camera *camera)
    : world(world), camera(camera) {}

// -----------------------------------------------------------------------------
void runScript(const std::string &scriptName) {
  LuaState state; 
  std::string scriptFile = SCRIPT_PATH + scriptName; 
  state.runScript(scriptFile);
}

// -----------------------------------------------------------------------------
int setGravity(lua_State *luaState) {
  ScriptEngine *engine = luaW_check<ScriptEngine>(luaState, 1);
  float gravityX = static_cast<float>(luaL_checknumber(luaState, 2));
  float gravityY = static_cast<float>(luaL_checknumber(luaState, 3));
  float gravityZ = static_cast<float>(luaL_checknumber(luaState, 4));
  
  engine->world->setGravity({gravityX, gravityY, gravityZ});
  return 0;
}

// -----------------------------------------------------------------------------
int setClearColor(lua_State *luaState) {
  ScriptEngine *engine = luaW_check<ScriptEngine>(luaState, 1);
  float colorR = static_cast<float>(luaL_checknumber(luaState, 2));
  float colorG = static_cast<float>(luaL_checknumber(luaState, 3));
  float colorB = static_cast<float>(luaL_checknumber(luaState, 4));
  float colorA = static_cast<float>(luaL_checknumber(luaState, 5));

  return 0;
}

// -----------------------------------------------------------------------------
int setCamera(lua_State *luaState) {
  ScriptEngine *engine = luaW_check<ScriptEngine>(luaState, 1);
  float positionX = static_cast<float>(luaL_checknumber(luaState, 2));
  float positionY = static_cast<float>(luaL_checknumber(luaState, 3));
  float positionZ = static_cast<float>(luaL_checknumber(luaState, 4));
  float rotationX = static_cast<float>(luaL_checknumber(luaState, 5));
  float rotationY = static_cast<float>(luaL_checknumber(luaState, 6));
  float viewAngle = static_cast<float>(luaL_checknumber(luaState, 7));
  float zNear = static_cast<float>(luaL_checknumber(luaState, 8));
  float zFar = static_cast<float>(luaL_checknumber(luaState, 9));

  engine->camera->assign({positionX, positionY, positionZ, 1.0},
                         {rotationX, rotationY}, viewAngle, zNear, zFar);
  return 0;
}

// -----------------------------------------------------------------------------
int addPositionalLight(lua_State *luaState) {
  ScriptEngine *engine = luaW_check<ScriptEngine>(luaState, 1);
  float positionX = static_cast<float>(luaL_checknumber(luaState, 2));
  float positionY = static_cast<float>(luaL_checknumber(luaState, 3));
  float positionZ = static_cast<float>(luaL_checknumber(luaState, 4));
  float ambientColorR = static_cast<float>(luaL_checknumber(luaState, 5));
  float ambientColorG = static_cast<float>(luaL_checknumber(luaState, 6));
  float ambientColorB = static_cast<float>(luaL_checknumber(luaState, 7));
  float ambientColorA = static_cast<float>(luaL_checknumber(luaState, 8));
  float diffuseColorR = static_cast<float>(luaL_checknumber(luaState, 9));
  float diffuseColorG = static_cast<float>(luaL_checknumber(luaState, 10));
  float diffuseColorB = static_cast<float>(luaL_checknumber(luaState, 11));
  float diffuseColorA = static_cast<float>(luaL_checknumber(luaState, 12));
  float specularColorR = static_cast<float>(luaL_checknumber(luaState, 13));
  float specularColorG = static_cast<float>(luaL_checknumber(luaState, 14));
  float specularColorB = static_cast<float>(luaL_checknumber(luaState, 15));
  float specularColorA = static_cast<float>(luaL_checknumber(luaState, 16));
  float constantAttenuation = static_cast<float>(luaL_checknumber(luaState, 17));
  float linearAttenuation = static_cast<float>(luaL_checknumber(luaState, 18));
  float quadraticAttenuation = static_cast<float>(luaL_checknumber(luaState, 19));
  
  LightBuilder lightBuilder;
  Light *light = lightBuilder
                      .setPosition(glm::vec3(positionX, positionY, positionZ))      
                      .setAmbientColor({ambientColorR, ambientColorG, ambientColorB, ambientColorA})
                      .setDiffuseColor({diffuseColorR, diffuseColorG, diffuseColorB, diffuseColorA})
                      .setSpecularColor({specularColorR, specularColorG, specularColorB, specularColorA})
                      .setConstantAttenuation(constantAttenuation)
                      .setLinearAttenuation(linearAttenuation)
                      .setQuadraticAttenuation(quadraticAttenuation)
                      .createPositional();

  engine->world->addLight(light);
  return 0;
}

// -----------------------------------------------------------------------------
int addDirectionalLight(lua_State *luaState) {
  ScriptEngine *engine = luaW_check<ScriptEngine>(luaState, 1);
  float directionX = static_cast<float>(luaL_checknumber(luaState, 2));
  float directionY = static_cast<float>(luaL_checknumber(luaState, 3));
  float directionZ = static_cast<float>(luaL_checknumber(luaState, 4));
  float ambientColorR = static_cast<float>(luaL_checknumber(luaState, 5));
  float ambientColorG = static_cast<float>(luaL_checknumber(luaState, 6));
  float ambientColorB = static_cast<float>(luaL_checknumber(luaState, 7));
  float ambientColorA = static_cast<float>(luaL_checknumber(luaState, 8));
  float diffuseColorR = static_cast<float>(luaL_checknumber(luaState, 9));
  float diffuseColorG = static_cast<float>(luaL_checknumber(luaState, 10));
  float diffuseColorB = static_cast<float>(luaL_checknumber(luaState, 11));
  float diffuseColorA = static_cast<float>(luaL_checknumber(luaState, 12));
  float specularColorR = static_cast<float>(luaL_checknumber(luaState, 13));
  float specularColorG = static_cast<float>(luaL_checknumber(luaState, 14));
  float specularColorB = static_cast<float>(luaL_checknumber(luaState, 15));
  float specularColorA = static_cast<float>(luaL_checknumber(luaState, 16));

  LightBuilder lightBuilder;
  Light *light = lightBuilder.setDirection({directionX, directionY, directionZ})
                     .setAmbientColor({ambientColorR, ambientColorG,
                                       ambientColorB, ambientColorA})
                     .setDiffuseColor({diffuseColorR, diffuseColorG,
                                       diffuseColorB, diffuseColorA})
                     .setSpecularColor({specularColorR, specularColorG,
                                        specularColorB, specularColorA})
                     .createDirectional();

  engine->world->addLight(light);
  return 0;
}

// -----------------------------------------------------------------------------
int addPlane(lua_State *luaState) {
  ScriptEngine *engine = luaW_check<ScriptEngine>(luaState, 1);
  float side = static_cast<float>(luaL_checknumber(luaState, 2));
  float mass = static_cast<float>(luaL_checknumber(luaState, 3));
  float positionX = static_cast<float>(luaL_checknumber(luaState, 4));
  float positionY = static_cast<float>(luaL_checknumber(luaState, 5));
  float positionZ = static_cast<float>(luaL_checknumber(luaState, 6));
  float rotationX = static_cast<float>(luaL_checknumber(luaState, 7));
  float rotationY = static_cast<float>(luaL_checknumber(luaState, 8));
  float rotationZ = static_cast<float>(luaL_checknumber(luaState, 9));
  float ambientColorR = static_cast<float>(luaL_checknumber(luaState, 10));
  float ambientColorG = static_cast<float>(luaL_checknumber(luaState, 11));
  float ambientColorB = static_cast<float>(luaL_checknumber(luaState, 12));
  float ambientColorA = static_cast<float>(luaL_checknumber(luaState, 13));
  float diffuseColorR = static_cast<float>(luaL_checknumber(luaState, 14));
  float diffuseColorG = static_cast<float>(luaL_checknumber(luaState, 15));
  float diffuseColorB = static_cast<float>(luaL_checknumber(luaState, 16));
  float diffuseColorA = static_cast<float>(luaL_checknumber(luaState, 17));
  float specularColorR = static_cast<float>(luaL_checknumber(luaState, 18));
  float specularColorG = static_cast<float>(luaL_checknumber(luaState, 19));
  float specularColorB = static_cast<float>(luaL_checknumber(luaState, 20));
  float specularColorA = static_cast<float>(luaL_checknumber(luaState, 21));
  const char *textureFile = luaL_checkstring(luaState, 22);
  
  PlaneBuilder planeBuilder;
  btQuaternion rotation(rotationX, rotationY, rotationZ);
  Plane *plane = planeBuilder
                      .setTransform(btTransform(rotation, btVector3(positionX, positionY, positionZ)))
                      .setSide(side)
                      .setMass(mass)
                      .setAmbientColor({ambientColorR, ambientColorG, ambientColorB, ambientColorA})
                      .setDiffuseColor({diffuseColorR, diffuseColorG, diffuseColorB, diffuseColorA})
                      .setSpecularColor({specularColorR, specularColorG, specularColorB, specularColorA})
                      .setTextureFile(textureFile)
                      .create();

  engine->world->addObject(plane);
  return 0;
}

// -----------------------------------------------------------------------------
int addBox(lua_State *luaState) {
  ScriptEngine *engine = luaW_check<ScriptEngine>(luaState, 1);
  float sideX = static_cast<float>(luaL_checknumber(luaState, 2));
  float sideY = static_cast<float>(luaL_checknumber(luaState, 3));
  float sideZ = static_cast<float>(luaL_checknumber(luaState, 4));
  float mass = static_cast<float>(luaL_checknumber(luaState, 5));
  float positionX = static_cast<float>(luaL_checknumber(luaState, 6));
  float positionY = static_cast<float>(luaL_checknumber(luaState, 7));
  float positionZ = static_cast<float>(luaL_checknumber(luaState, 8));
  float rotationX = static_cast<float>(luaL_checknumber(luaState, 9));
  float rotationY = static_cast<float>(luaL_checknumber(luaState, 10));
  float rotationZ = static_cast<float>(luaL_checknumber(luaState, 11));
  float ambientColorR = static_cast<float>(luaL_checknumber(luaState, 12));
  float ambientColorG = static_cast<float>(luaL_checknumber(luaState, 13));
  float ambientColorB = static_cast<float>(luaL_checknumber(luaState, 14));
  float ambientColorA = static_cast<float>(luaL_checknumber(luaState, 15));
  float diffuseColorR = static_cast<float>(luaL_checknumber(luaState, 16));
  float diffuseColorG = static_cast<float>(luaL_checknumber(luaState, 17));
  float diffuseColorB = static_cast<float>(luaL_checknumber(luaState, 18));
  float diffuseColorA = static_cast<float>(luaL_checknumber(luaState, 19));
  float specularColorR = static_cast<float>(luaL_checknumber(luaState, 20));
  float specularColorG = static_cast<float>(luaL_checknumber(luaState, 21));
  float specularColorB = static_cast<float>(luaL_checknumber(luaState, 22));
  float specularColorA = static_cast<float>(luaL_checknumber(luaState, 23));
  const char *textureFile = luaL_checkstring(luaState, 24);
  
  BoxBuilder boxBuilder;
  btQuaternion rotation(rotationX, rotationY, rotationZ);
  Box *box = boxBuilder
             .setTransform(btTransform(rotation, btVector3(positionX, positionY, positionZ)))
             .setSides({sideX, sideY, sideZ})
             .setMass(mass)
             .setAmbientColor({ambientColorR, ambientColorG, ambientColorB, ambientColorA})
             .setDiffuseColor({diffuseColorR, diffuseColorG, diffuseColorB, diffuseColorA})
             .setSpecularColor({specularColorR, specularColorG, specularColorB, specularColorA})
             .setTextureFile(textureFile)
             .create();

  engine->world->addObject(box);
  return 0;
}

// -----------------------------------------------------------------------------
int addMesh(lua_State *luaState) {
  ScriptEngine *engine = luaW_check<ScriptEngine>(luaState, 1);
  const char *meshFile = luaL_checkstring(luaState, 2);
  float mass = static_cast<float>(luaL_checknumber(luaState, 3));
  float positionX = static_cast<float>(luaL_checknumber(luaState, 4));
  float positionY = static_cast<float>(luaL_checknumber(luaState, 5));
  float positionZ = static_cast<float>(luaL_checknumber(luaState, 6));
  float rotationX = static_cast<float>(luaL_checknumber(luaState, 7));
  float rotationY = static_cast<float>(luaL_checknumber(luaState, 8));
  float rotationZ = static_cast<float>(luaL_checknumber(luaState, 9));
  float ambientColorR = static_cast<float>(luaL_checknumber(luaState, 10));
  float ambientColorG = static_cast<float>(luaL_checknumber(luaState, 11));
  float ambientColorB = static_cast<float>(luaL_checknumber(luaState, 12));
  float ambientColorA = static_cast<float>(luaL_checknumber(luaState, 13));
  float diffuseColorR = static_cast<float>(luaL_checknumber(luaState, 14));
  float diffuseColorG = static_cast<float>(luaL_checknumber(luaState, 15));
  float diffuseColorB = static_cast<float>(luaL_checknumber(luaState, 16));
  float diffuseColorA = static_cast<float>(luaL_checknumber(luaState, 17));
  float specularColorR = static_cast<float>(luaL_checknumber(luaState, 18));
  float specularColorG = static_cast<float>(luaL_checknumber(luaState, 19));
  float specularColorB = static_cast<float>(luaL_checknumber(luaState, 20));
  float specularColorA = static_cast<float>(luaL_checknumber(luaState, 21));
  
  MeshBuilder meshBuilder;
  btQuaternion rotation(rotationX, rotationY, rotationZ);
  Mesh *mesh = meshBuilder
                      .setMeshFile(meshFile)
                      .setTransform(btTransform(rotation, btVector3(positionX, positionY, positionZ)))
                      .setMass(mass)
                      .setAmbientColor({ambientColorR, ambientColorG, ambientColorB, ambientColorA})
                      .setDiffuseColor({diffuseColorR, diffuseColorG, diffuseColorB, diffuseColorA})
                      .setSpecularColor({specularColorR, specularColorG, specularColorB, specularColorA})
                      .create();

  engine->world->addObject(mesh);
  return 0;
}

