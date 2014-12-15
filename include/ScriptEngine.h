#ifndef SCRIPT_ENGINE_H
#define SCRIPT_ENGINE_H

#include "Camera.h"
#include "World.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include "ext/luawrapper.hpp"

#include <iostream>
#include <string>

class Camera;
class World;

// Support functions.
// -----------------------------------------------------------------------------
int setGravity(lua_State *luaState);
int setCamera(lua_State *luaState);
int addPositionalLight(lua_State *luaState);
int addDirectionalLight(lua_State *luaState);
int addSpotLight(lua_State *luaState);
int addPlane(lua_State *luaState);
int addMesh(lua_State *luaState);
int addBox(lua_State *luaState);
int addSphere(lua_State *luaState);

// ============================================================================= 
class LuaState {
public:
  LuaState();
  ~LuaState();

public:
  void runScript(const std::string &scriptFile);

private:
  lua_State *luaState;
};

// ============================================================================= 
class ScriptEngine {
public:
  ScriptEngine(World *world, Camera *camera);

public:
  void setGravity(float x, float y, float z);
  void setCamera(float x, float y, float z, float angleX, float angleY);
  void addPlane(float side);
  void addMesh();
  void addBox();
  void addSphere();

public:
  Camera *camera;
  World *world;
};

void runScript(const std::string &scriptName);

#endif
