#pragma once

class LuaScript;
struct lua_State;

// I could later split this up so its only bindings here
// and all functions those bindings called in another Lua cpp/hpp

//===============================================================================================
// The function we call to do all the stuff
//===============================================================================================
void BindLuaFunctionsToScript(LuaScript* theScriptToBind);

//===============================================================================================
// The functions to bind (lua params in the cpp)
//==============================================================================================
static int l_Write(lua_State* L);
static int l_DrawCircle(lua_State* L);
static int l_DrawAABB2(lua_State* L);