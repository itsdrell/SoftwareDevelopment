#pragma once
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"

//====================================================================================
enum TeamName
{
	TEAM_BLUE,
	TEAM_GREEN,
	TEAM_RED,
	NUM_OF_TEAMS
};


//====================================================================================

extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_audio; // not the audio cause we could have multiple...?

