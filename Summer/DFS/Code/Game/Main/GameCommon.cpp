#include "Game/Main/GameCommon.hpp"

std::string TeamNameToString(TeamName team)
{
	std::string name = "idk";
		
	switch (team)
	{
	case TEAM_BLUE:
		name = "BLUE";
		break;
	case TEAM_GREEN:
		name = "GREEN";
		break;
	case TEAM_RED:
		name = "RED";
		break;
	case TEAM_YELLOW:
		name = "YELLOW";
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

Renderer* g_theRenderer = nullptr;
InputSystem* g_theInput = nullptr;
AudioSystem* g_audio = nullptr;
/*Lives* g_amountOfLives = new Lives();*/