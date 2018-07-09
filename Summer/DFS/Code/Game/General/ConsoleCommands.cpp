#include "ConsoleCommands.hpp"
#include "Game/Main/Game.hpp"
#include "Game/GameStates/Playing.hpp"
#include "Game/General/Map.hpp"
#include "Game/General/GameObjects/Unit.hpp"
#include "Game/General/GameObjects/Building.hpp"
#include "../GameStates/Playing.hpp"
#include "Game/General/UI/Container.hpp"
#include "Engine/Core/Tools/DevConsole.hpp"

void EndTurn(Command & theCommand)
{
	UNUSED(theCommand);
	
	g_theCurrentMap->ClearHoverTiles();
	g_theCurrentMap->GoToNextTurn();
	g_theGame->m_playingState->m_actionMenu->ClearWidgets();
}

void HaveAUnitWait(Command& theCommand)
{
	UNUSED(theCommand);

	Unit* currentUnit = g_theCurrentMap->m_selectedUnit;

	if(currentUnit == nullptr)
		return;

	currentUnit->m_usedAction = true;
	g_theCurrentMap->ClearHoverTiles();
	g_theGame->m_playingState->m_actionMenu->ClearWidgets();
	g_theGame->m_playingState->m_currentPlayState = SELECTING;
}

void CaptureBuilding(Command& theCommand)
{
	UNUSED(theCommand);

	if(g_theCurrentMap->m_selectedUnit == nullptr)
	{
		DevConsole::GetInstance()->AddErrorMessage("No building or unit selected");
		return;
	}
	g_theCurrentMap->m_buildingToCapture->m_team = g_theCurrentMap->m_selectedUnit->m_team;

	// reset state
	g_theCurrentMap->m_selectedUnit->m_usedAction = true;
	g_theGame->m_playingState->m_actionMenu->ClearWidgets();
	g_theGame->m_playingState->m_currentPlayState = SELECTING;
	g_theCurrentMap->ClearHoverTiles();
}

void AddUnit(Command& theCommand)
{
	DevConsole* dc = DevConsole::GetInstance();
	
	// addUnit name team pos health 
	std::string unitName = "grunt";
	TeamName teamName = TEAM_RED;
	IntVector2 pos = IntVector2(0,0);
	int hp = 10;

	if(IsIndexValid(1, theCommand.m_commandArguements))
		unitName = theCommand.m_commandArguements.at(1);
	if(IsIndexValid(2, theCommand.m_commandArguements))
		teamName = StringFromTeamName(theCommand.m_commandArguements.at(2));
	if(IsIndexValid(3, theCommand.m_commandArguements))
		pos = ParseString(theCommand.m_commandArguements.at(3), pos);
	if(IsIndexValid(4, theCommand.m_commandArguements))
		hp = ParseString(theCommand.m_commandArguements.at(4), hp);


	g_theCurrentMap->CreateUnit(unitName, teamName, pos, hp);


}
