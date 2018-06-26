#include "ConsoleCommands.hpp"
#include "Game/Main/Game.hpp"
#include "Game/GameStates/Playing.hpp"
#include "Game/General/Map.hpp"
#include "Game/General/GameObjects/Unit.hpp"
#include "Game/General/GameObjects/Building.hpp"

void EndTurn(Command & theCommand)
{
	UNUSED(theCommand);

	g_theCurrentMap->GoToNextTurn();
}

void HaveAUnitWait(Command& theCommand)
{
	UNUSED(theCommand);

	Unit* currentUnit = g_theCurrentMap->m_selectedUnit;

	if(currentUnit == nullptr)
		return;

	currentUnit->m_usedAction = true;
	g_theCurrentMap->ClearHoverTiles();
	g_theGame->m_playingState->m_currentPlayState = SELECTING;
}

void CaptureBuilding(Command& theCommand)
{
	UNUSED(theCommand);

	g_theCurrentMap->m_buildingToCapture->m_team = g_theCurrentMap->m_selectedUnit->m_team;

	// reset state
	g_theCurrentMap->m_selectedUnit->m_usedAction = true;
	g_theGame->m_playingState->m_currentPlayState = SELECTING;
	g_theCurrentMap->ClearHoverTiles();
}
