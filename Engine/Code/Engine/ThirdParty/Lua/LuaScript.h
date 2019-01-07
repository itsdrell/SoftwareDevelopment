#ifndef LUASCRIPT_H
#define LUASCRIPT_H

#include <string>
#include <vector>
#include <iostream>

extern "C" {
# include "LuaSource/lua.h"
# include "LuaSource/lauxlib.h" 
# include "LuaSource/lualib.h"
}

#pragma warning(disable: 4100)

#define dirPath ("Data/Scripts/Lua/")

//====================================================================================
//									NOTES:
//____________________________________________________________________________________
// 
// Followed this tutorial https://eliasdaler.wordpress.com/2013/10/11/lua_cpp_binder/
// Only thing im gonna do is change some names and maybe add more helpers but ^
// is 99% of this class and is a cool tutorial. 10/10 would learn again
//
//====================================================================================

class LuaScript 
{
public:
	//LuaScript() {} // careful with this gave me some issues defining variables as members and using this as a default constructor
    LuaScript(const std::string& fileName);
    ~LuaScript();
    void printError(const std::string& variableName, const std::string& reason);
    std::vector<int> getIntVector(const std::string& name);
    std::vector<std::string> getTableKeys(const std::string& name);

	lua_State* GetLuaState() const { return L; }
    
    inline void clean() {
      int n = lua_gettop(L);
      lua_pop(L, n);
    }

    template<typename T>
    T get(const std::string& variableName) {
      if(!L) {
        printError(variableName, "Script is not loaded");
        return lua_getdefault<T>();
      }
      
      T result;
      if(lua_gettostack(variableName)) { // variable succesfully on top of stack
        result = lua_get<T>(variableName);  
      } else {
        result = lua_getdefault<T>();
      }

     
      clean();
      return result;
    }

    bool lua_gettostack(const std::string& variableName) {
      level = 0;
      std::string var = "";
        for(unsigned int i = 0; i < variableName.size(); i++) {
          if(variableName.at(i) == '.') {
            if(level == 0) {
              lua_getglobal(L, var.c_str());
            } else {
              lua_getfield(L, -1, var.c_str());
            }
            
            if(lua_isnil(L, -1)) {
              printError(variableName, var + " is not defined");
              return false;
            } else {
              var = "";
              level++;
            }
          } else {
            var += variableName.at(i);
          }
        }
        if(level == 0) {
          lua_getglobal(L, var.c_str());
        } else {
          lua_getfield(L, -1, var.c_str());
        }
        if(lua_isnil(L, -1)) {
            printError(variableName, var + " is not defined");
            return false;
        }

        return true;       
    }

    // Generic get
    template<typename T>
    T lua_get(const std::string& variableName) {
      return 0;
    }

    template<typename T>
    T lua_getdefault() {
      return 0;
    }
   
private:
    lua_State*			L;
    std::string			m_filename;
    int					level;
};

//===============================================================================================
// These are the two functions I wanna use in my game like Love2D does

bool LuaUpdate(LuaScript& mainScript, float ds);
bool LuaRender(LuaScript& mainScript);

//===============================================================================================
// Specializations / helpers

template <> 
inline bool LuaScript::lua_get<bool>(const std::string& variableName) {
    return (bool)lua_toboolean(L, -1);
}

template <> 
inline float LuaScript::lua_get<float>(const std::string& variableName) {
    if(!lua_isnumber(L, -1)) {
      printError(variableName, "Not a number");
    }
    return (float)lua_tonumber(L, -1);
}

template <>
inline int LuaScript::lua_get<int>(const std::string& variableName) {
    if(!lua_isnumber(L, -1)) {
      printError(variableName, "Not a number");
    }
    return (int)lua_tonumber(L, -1);
}

template <>
inline std::string LuaScript::lua_get<std::string>(const std::string& variableName) {
    std::string s = "null";
    if(lua_isstring(L, -1)) {
      s = std::string(lua_tostring(L, -1));
    } else {
      printError(variableName, "Not a string");
    }
    return s;
}

template<>
inline std::string LuaScript::lua_getdefault<std::string>() {
  return "null";
}

#endif
