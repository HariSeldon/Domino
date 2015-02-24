#pragma once

#include "Camera.h"
#include "Light.h"
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
class SceneContainer;
class World;

// Support functions.
// -----------------------------------------------------------------------------
int addBox(lua_State *luaState);
int addDirectionalLight(lua_State *luaState);
int addMesh(lua_State *luaState);
int addPlane(lua_State *luaState);
int addPositionalLight(lua_State *luaState);
int addSphere(lua_State *luaState);
int addSpotLight(lua_State *luaState);
int setBackgroundColor(lua_State *luaState);
int setCamera(lua_State *luaState);
int setGravity(lua_State *luaState);

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
  ScriptEngine(SceneContainer *container);

public:
  void setBackgroundColor(float r, float g, float b, float a);
  void setGravity(float x, float y, float z);
  void setCamera(float x, float y, float z, float angleX, float angleY);
  void addPlane(float side);
  void addMesh();
  void addBox();
  void addSphere();

public:
  SceneContainer *container;
  LightBuilder lightBuilder;
};

void runScript(const std::string &scriptName);
