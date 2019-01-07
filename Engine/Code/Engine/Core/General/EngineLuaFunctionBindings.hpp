#pragma once
#include "Engine/Core/General/Rgba.hpp"
#include "Engine/ThirdParty/Lua/LuaWrapper/luawrapper.hpp"

class LuaScript;
struct lua_State;

// I could later split this up so its only bindings here
// and all functions those bindings called in another Lua cpp/hpp

//===============================================================================================
// The function we call to do all the stuff
//===============================================================================================
void LuaStartUp(LuaScript* theScriptToBind);
void BindLuaFunctionsToScript(LuaScript* theScriptToBind);
void BindClassesToLua(lua_State* L);

//===============================================================================================
// The functions to bind (lua params in the cpp)
//==============================================================================================
static int l_Write(lua_State* L);
static int l_DrawCircle(lua_State* L);
static int l_DrawAABB2(lua_State* L);


//===============================================================================================
// Classes (this might be better if it was put in the cpp/hpp of the classes
//===============================================================================================
//-----------------------------------------------------------------------------------------------
// Rgba
Rgba* Rgba_new(lua_State* L);
int Rgba_printColor(lua_State* L);

// for static class functions
static luaL_Reg Rgba_table[] =
{
	{NULL, NULL}
};

// for member functions
static luaL_Reg Rgba_metatable[] =
{
	{ "PrintColor", Rgba_printColor },
	{NULL, NULL}
};

static int luaopen_Rgba(lua_State* L);
//-----------------------------------------------------------------------------------------------