#pragma once
#include "Engine/Math/Vectors/Vector3.hpp"
#include "Engine/Math/Vectors/IntVector2.hpp"
#include "Engine/Core/General/Rgba.hpp"
#include "Engine/Core/Tools/ErrorWarningAssert.hpp"
#include <vector>
#include <string>
#include "Engine/Math/Vectors/Vector4.hpp"


//====================================================================================
// Forward Declare
//====================================================================================
class Blackboard;
class SpriteSheet;
class RenderBuffer;

//====================================================================================
// Type Defs + Defines
//====================================================================================
#define MAX_LIGHTS (8)
#define STATIC
#define EPSILON (FLT_EPSILON)
#define UNUSED(x) (void)(x);

// Source from http://www.flipcode.com/archives/FIXME_TODO_Notes_As_Warnings_In_Compiler_Output.shtml
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "

#define PRAGMA(p)  __pragma( p )
#define NOTE( x )  PRAGMA( message(x) )
#define FILE_LINE  NOTE( __FILE__LINE__ )

// THE IMPORANT BITS
#define TODO( x )  NOTE( __FILE__LINE__"\n"           \
        " --------------------------------------------------------------------------------------\n" \
        "|  TODO :   " ##x "\n" \
        " --------------------------------------------------------------------------------------\n" )

// Unimplemented (Todo comment on steroids)
#define UNIMPLEMENTED()  TODO( "IMPLEMENT: " QUOTE(__FILE__) " (" QUOTE(__LINE__) ")" ); ASSERT_RECOVERABLE(1,"")

typedef std::string					String;
typedef std::vector<std::string>	Strings;
typedef std::vector<int>			Ints;
typedef unsigned int				uint;

//TODO("Make an actual uniform buffer pls");
typedef RenderBuffer				UniformBuffer; 

//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================


//====================================================================================
// Standalone C Functions
//====================================================================================
void EngineStartUp();
void EngineShutDown();

template< typename T >
bool IsIndexValid(uint idx, std::vector<T>& theVector);

template< typename T >
bool IsIndexValid(uint idx, std::vector<T>& theVector)
{
	return (idx > 0 && idx < theVector.size());
}

//====================================================================================
// Externs
//====================================================================================
extern Blackboard g_gameConfigBlackboard;
extern SpriteSheet g_tileSpriteSheet;
extern SpriteSheet g_dialogueSpriteSheet;

//====================================================================================
// Written by Zachary Bracken : [7/4/2018]
//====================================================================================




