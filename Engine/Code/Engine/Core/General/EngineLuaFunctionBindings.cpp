#include "EngineLuaFunctionBindings.hpp"
#include "Engine/ThirdParty/Lua/LuaScript.h"
#include "Engine/ThirdParty/Lua/LuaWrapper/luawrapper.hpp"
#include "Engine/ThirdParty/Lua/LuaWrapper/luawrapperutil.hpp"
#include "../Tools/ErrorWarningAssert.hpp"
#include "../Tools/DevConsole.hpp"
#include "../../Renderer/Renderer.hpp"
#include "EngineCommon.hpp"
#include <memory>

//===============================================================================================
void LuaStartUp(LuaScript* theScriptToBind)
{
	BindLuaFunctionsToScript(theScriptToBind);
	//BindClassesToLua(theScriptToBind); // happens in LuaScript construction to prevent errors
}

//-----------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------
void BindClassesToLua(lua_State* L)
{
	luaopen_Rgba(L);
}

//===============================================================================================
int l_Write(lua_State * L)
{
	const char* str = lua_tostring(L, 1); // get function argument
	DevConsole::GetInstance()->AddConsoleDialogue(str); // calling C++ function with this argument...
	return 0; // nothing to return!
}

//-----------------------------------------------------------------------------------------------
// Params in Lua are: x, y, r, filled(true or false), Rgba
int l_DrawCircle(lua_State* L)
{
	// get all the parameters we put in Lua
	float x = (float) lua_tonumber(L, 1);
	float y = (float) lua_tonumber(L, 2);
	float radius = (float) lua_tonumber(L, 3);
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
// params are: minx, miny, maxx, maxy, filled, Rgba
int l_DrawAABB2(lua_State* L)
{
	// get all the parameters we put in Lua
	float minx = (float) lua_tonumber(L, 1);
	float miny = (float) lua_tonumber(L, 2);
	float maxx = (float) lua_tonumber(L, 3);
	float maxy = (float) lua_tonumber(L, 4);
	bool filled = lua_toboolean(L, 5);

	Rgba* color = luaW_check<Rgba>(L,6);

	Renderer* r = Renderer::GetInstance();
	
	// do the dang draw
	r->DrawAABB2(AABB2(minx, miny, maxx, maxy), *color, filled);


	return 0;
}

Rgba* Rgba_new(lua_State * L)
{
	const char r = (const char) luaL_checknumber(L, 1);
	const char g = (const char) luaL_checknumber(L, 2);
	const char b = (const char) luaL_checknumber(L, 3);
	const char a = (const char) luaL_checknumber(L, 4);

	//return new Rgba(r,g,b,a); // a leak!

	// putting r,g,b,a in the parenthesis vvvv constructed something that was white :l
	Rgba* newColor = std::make_shared<Rgba>().get(); // using shared to prevent leaks
	newColor->SetAsBytes(r,g,b,a); // had to set after construction
	luaW_release(L, newColor);
	
	return newColor;
}

int Rgba_printColor(lua_State * L)
{
	Rgba* color = luaW_check<Rgba>(L,1);
	String colorAsString = color->GetRGBString();
	lua_pushstring(L, colorAsString.c_str());

	DevConsole::GetInstance()->AddConsoleDialogue(colorAsString);

	return 0;
}

int luaopen_Rgba(lua_State* L)
{
	luaW_register<Rgba>(L, "Rgba", Rgba_table, Rgba_metatable, Rgba_new);
	return 1;
}
