#include "Game/Main/GameCommon.hpp"

std::string TeamNameToString(TeamName team)
{
	std::string name = "idk";
		
	switch (team)
	{
	case TEAM_BLUE:
		name = "blue";
		break;
	case TEAM_GREEN:
		name = "green";
		break;
	case TEAM_RED:
		name = "red";
		break;
	case TEAM_YELLOW:
		name = "yellow";
		break;
	case TEAM_NONE:
		name = "none";
		break;
	default:
		name = "idk";
		break;
	}

	return name;
}

TeamName StringFromTeamName(std::string name)
{
	
	if(name == "blue")
		return TEAM_BLUE;
	if(name == "green")
		return TEAM_GREEN;
	if(name == "red")
		return TEAM_RED;
	if(name == "yellow")
		return TEAM_YELLOW;
	if(name == "none")
		return TEAM_NONE;

	return TEAM_RED;
}

Rgba GetColorFromTeamName(TeamName team)
{
	Rgba color;
	
	switch (team)
	{
	case TEAM_BLUE:
		color = Rgba::BLUE;
		break;
	case TEAM_GREEN:
		color = Rgba::GREEN;
		break;
	case TEAM_RED:
		color = Rgba::RED;
		break;
	case TEAM_YELLOW:
		color = Rgba::YELLOW;
		break;
	default:
		color = Rgba::WHITE;
		break;
	}

	return color;
}

Strings GetAllTeamNames()
{
	Strings result;

	for(int i = 0; i < NUM_OF_TEAMS; i++)
	{
		result.push_back(TeamNameToString((TeamName) i));
	}

	return result;
}

Renderer* g_theRenderer = nullptr;
InputSystem* g_theInput = nullptr;
AudioSystem* g_audio = nullptr;


SpriteSheet		g_blueUnitSpriteSheet;
SpriteSheet		g_redUnitSpriteSheet;
SpriteSheet		g_buildingSpriteSheet;
/*Lives* g_amountOfLives = new Lives();*/