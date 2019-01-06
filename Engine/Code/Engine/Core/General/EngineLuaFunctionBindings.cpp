#include "EngineLuaFunctionBindings.hpp"
#include "Engine/ThirdParty/Lua/LuaScript.h"
#include "../Tools/ErrorWarningAssert.hpp"
#include "../Tools/DevConsole.hpp"
#include "../../Renderer/Renderer.hpp"

void BindLuaFunctionsToScript(LuaScript * theScriptToBind)
{
	lua_State* theState = theScriptToBind->GetLuaState();

	// Print to dev console
	lua_pushcfunction(theState, l_Write);
	lua_setglobal(theState, "Write");

	// Draw Circle
	lua_pushcfunction(theState, l_DrawCircle);
	lua_setglobal(theState, "DrawCircle");

	// Draw AABB2
	lua_pushcfunction(theState, l_DrawAABB2);
	lua_setglobal(theState, "DrawAABB2");

	lua_pcall(theState, 0, 0, 0);
}

//===============================================================================================
int l_Write(lua_State * L)
{
	const char* str = lua_tostring(L, 1); // get function argument
	DevConsole::GetInstance()->AddConsoleDialogue(str); // calling C++ function with this argument...
	return 0; // nothing to return!
}

//-----------------------------------------------------------------------------------------------
// Params in Lua are: x, y, r, filled(true or false)
int l_DrawCircle(lua_State* L)
{
	// get all the parameters we put in Lua
	float x = lua_tonumber(L, 1);
	float y = lua_tonumber(L, 2);
	float radius = lua_tonumber(L, 3);
	bool filled = lua_toboolean(L, 4);
	
	Renderer* r = Renderer::GetInstance();
	// do the dang draw
	if(filled)
		r->DrawCircleFilled(Vector2(x,y), radius);
	else
		r->DrawCircle(Vector2(x,y), radius);
	
	return 0;
}

//-----------------------------------------------------------------------------------------------
// params are: minx, miny, maxx, maxy, filled
int l_DrawAABB2(lua_State* L)
{
	// get all the parameters we put in Lua
	float minx = lua_tonumber(L, 1);
	float miny = lua_tonumber(L, 2);
	float maxx = lua_tonumber(L, 3);
	float maxy = lua_tonumber(L, 4);
	bool filled = lua_toboolean(L, 5);

	Renderer* r = Renderer::GetInstance();
	
	// do the dang draw
	r->DrawAABB2(AABB2(minx, miny, maxx, maxy), Rgba::WHITE, filled);


	return 0;
}
