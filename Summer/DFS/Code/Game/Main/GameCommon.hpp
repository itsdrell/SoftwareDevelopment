#pragma once
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/Images/Sprites/SpriteSheet.hpp"

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

// The order matters here because in the sprite sheet for buildings I get the sprite 
// coords using this since they are all in one row
enum TeamName
{
	TEAM_BLUE,
	TEAM_RED,
	TEAM_NONE,
	TEAM_GREEN,
	TEAM_YELLOW,
	NUM_OF_TEAMS
};
std::string TeamNameToString(TeamName team);
TeamName StringFromTeamName(std::string name);
Rgba GetColorFromTeamName(TeamName team);
Strings GetAllTeamNames();


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
extern Renderer*		g_theRenderer;
extern InputSystem*		g_theInput;
extern AudioSystem*		g_audio; // not the audio cause we could have multiple...?


extern SpriteSheet		g_blueUnitSpriteSheet;
extern SpriteSheet		g_redUnitSpriteSheet;
extern SpriteSheet		g_buildingSpriteSheet;

//====================================================================================
// Written by Zachary Bracken : [6/19/2018]
//====================================================================================


