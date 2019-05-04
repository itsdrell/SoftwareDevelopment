//-----------------------------------------------------------------------------------------------
// EngineBuildPreferences.hpp
//
// Defines build preferences that the Engine should use when building for this particular game.
//
// Note that this file is an exception to the rule "engine code shall not know about game code".
//	Purpose: Each game can now direct the engine via #defines to build differently for that game.
//	Downside: ALL games must now have this Code/Game/EngineBuildPreferences.hpp file.
//

//#define ENGINE_DISABLE_AUDIO	// (If uncommented) Disables AudioSystem code and fmod linkage.

#define PROFILING_ENABLED 

//#define NETWORKING_ENABLED
//#define LOG_SYSTEM_ENABLED
//#define HOOK_DEVCONSOLE_TO_LOG_SYSTEM


//#define XRIGHT_YUP_ZFORWARD
#define XFORWARD_YLEFT_ZUP

//#define WINDING_ORDER_CLOCKWISE // only works for built in shaders (which we dont even use so :()

