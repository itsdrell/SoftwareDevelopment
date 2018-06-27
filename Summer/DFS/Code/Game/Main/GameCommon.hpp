#pragma once
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"

//====================================================================================
// Forward Declare
//====================================================================================


//====================================================================================
// Type Defs + Defines
//====================================================================================
constexpr float		TILE_SIZE = 16.f;
constexpr int		TILE_SIZE_INT = (int) TILE_SIZE;
constexpr float		HALF_TILE_SIZE = TILE_SIZE * .5f;

// In advanced wars they do 29 and 19 cause they start at 1 in the ui instead of 0
// so its actually one less from a good number
#define MAX_MAP_WIDTH	(30)
#define MAX_MAP_HEIGHT	(30)

//====================================================================================
// ENUMS
//====================================================================================
enum TeamName
{
	TEAM_NONE = -1,
	TEAM_BLUE,
	TEAM_GREEN,
	TEAM_RED,
	TEAM_YELLOW,
	NUM_OF_TEAMS
};
std::string TeamNameToString(TeamName team);
TeamName StringFromTeamName(std::string name);


enum SORTING_LAYER
{
	DEFAULT,
	HOVER_TILES,
	BUILDINGS,
	UNITS, 
	UI
};

//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================


//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================
extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_audio; // not the audio cause we could have multiple...?

//====================================================================================
// Written by Zachary Bracken : [6/19/2018]
//====================================================================================
//====================================================================================


