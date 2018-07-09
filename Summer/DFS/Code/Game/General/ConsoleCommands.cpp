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

	g_theCurrentMap->m_buildingToCapture->Captured(g_theCurrentMap->m_selectedUnit->m_team); 

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


	if(unitName == "help")
	{
		dc->AddHeader("Params: unitName team pos hp", Rgba::WHITE, 2);
		dc->AddHeader("Possible Units to spawn", Rgba::WHITE);
		
		Strings unitNames = UnitDefinition::GetAllUnitNames();
		for(uint i = 0; i < unitNames.size(); i++)
		{
			dc->AddConsoleDialogue(unitNames.at(i), GetRainbowColor((int) i , (int) unitNames.size()));
		}
		
		dc->AddSpace(1);
		dc->AddConsoleDialogue("Possible Team to spawn from", Rgba::WHITE);

		Strings teamNames = GetAllTeamNames();
		for(uint i = 0; i < teamNames.size(); i++)
		{
			dc->AddConsoleDialogue(teamNames.at(i), GetColorFromTeamName(StringFromTeamName(teamNames.at(i))));
		}

		dc->AddFooter();
		
	}
	else
	{
		g_theCurrentMap->CreateUnit(unitName, teamName, pos, hp);
	}

}

void AddBuilding(Command& theCommand)
{
	DevConsole* dc = DevConsole::GetInstance();
	
	// addUnit name team pos health 
	std::string buildingName = "default";
	TeamName teamName = TEAM_RED;
	IntVector2 pos = IntVector2(0,0);

	if(IsIndexValid(1, theCommand.m_commandArguements))
		buildingName = theCommand.m_commandArguements.at(1);
	if(IsIndexValid(2, theCommand.m_commandArguements))
		teamName = StringFromTeamName(theCommand.m_commandArguements.at(2));
	if(IsIndexValid(3, theCommand.m_commandArguements))
		pos = ParseString(theCommand.m_commandArguements.at(3), pos);


	if(buildingName == "help")
	{
		dc->AddHeader("Params: buildingName team pos", Rgba::WHITE, 2);
		dc->AddHeader("Possible Buildings to spawn", Rgba::WHITE);

		Strings buildingNames = BuildingDefinition::GetAllBuildingNames();
		for(uint i = 0; i < buildingNames.size(); i++)
		{
			dc->AddConsoleDialogue(buildingNames.at(i), GetRainbowColor((int) i , (int) buildingNames.size()));
		}

		dc->AddSpace(1);
		dc->AddConsoleDialogue("Possible Team to spawn from", Rgba::WHITE);

		Strings teamNames = GetAllTeamNames();
		for(uint i = 0; i < teamNames.size(); i++)
		{
			dc->AddConsoleDialogue(teamNames.at(i), GetColorFromTeamName(StringFromTeamName(teamNames.at(i))));
		}

		dc->AddFooter();

	}
	else
	{
		g_theCurrentMap->CreateBuilding(buildingName, teamName, pos);
	}

}
