#include "ConsoleCommands.hpp"
#include "Game/Main/Game.hpp"
#include "Game/GameStates/Playing.hpp"
#include "Game/General/Map.hpp"
#include "Game/General/GameObjects/Unit.hpp"
#include "Game/General/GameObjects/Building.hpp"
#include "../GameStates/Playing.hpp"
#include "Game/General/UI/Container.hpp"
#include "Engine/Core/Tools/DevConsole.hpp"
#include "Game/General/GameObjects/Unit.hpp"
#include "Game/General/Player/CommandingOfficer.hpp"
#include "Engine\Core\General\GameObject2D.hpp"
#include "Engine/Renderer/RenderableComponents/Material.hpp"

//====================================================================================
UnitDefinition* g_unitToSpawn = nullptr;

//====================================================================================
void RegisterGameCommands()
{
	CommandRegister("endTurn", "", "Ends the player turn" , EndTurn);
	CommandRegister("wait", "", "Have current unit wait", HaveAUnitWait);
	CommandRegister("capture", "", "Capture Building for selected unit", CaptureBuilding);
	CommandRegister("addUnit", "", "Add unit to map", AddUnit);
	CommandRegister("addBuilding", "", "Add a building to map", AddBuilding);
	CommandRegister("closeMenu", "", "Close current Open Menu", CloseOpenMenu);
	CommandRegister("purchase", "", "Purchase Unit in Store", PurchaseUnit);
	CommandRegister("addAllUnits", "", "", AddAllUnitTypesToMap);
}

void EndTurn(Command & theCommand)
{
	UNUSED(theCommand);
	
	g_theCurrentMap->ClearHoverTiles();
	g_theCurrentMap->GoToNextTurn();
	g_theCurrentMap->m_actionMenu->ClearWidgets();
}

void HaveAUnitWait(Command& theCommand)
{
	UNUSED(theCommand);

	Unit* currentUnit = g_theCurrentMap->m_selectedUnit;

	if(currentUnit == nullptr)
		return;

	currentUnit->m_usedAction = true;
	g_theCurrentMap->ClearHoverTiles();
	g_theCurrentMap->m_actionMenu->ClearWidgets();
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
	g_theCurrentMap->m_actionMenu->ClearWidgets();
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

void CloseOpenMenu(Command& theCommand)
{
	UNUSED(theCommand);

	g_theCurrentMap->m_currentContainer->CloseMenu();

	if(g_theCurrentMap->m_selectedUnit != nullptr)
		g_theCurrentMap->PutSelectedUnitBack();
}

void PurchaseUnit(Command& theCommand)
{
	UNUSED(theCommand);
	
	// subtract funds
	g_theCurrentMap->m_currentOfficer->m_money -= g_unitToSpawn->m_cost;
	
	Vector2 tilePos = g_theCurrentMap->m_selectedBuilding->m_tileReference->m_position.GetAsVector2();
	tilePos *= (1 / TILE_SIZE);


	// add unit
	Unit* newUnit = g_theCurrentMap->CreateUnit(
		g_unitToSpawn->m_name, 
		g_theCurrentMap->m_currentOfficer->m_team, 
		tilePos.GetVector2AsInt(), 
		10);
	
	// make unit already moved
	newUnit->m_usedAction = true;
	newUnit->m_beenMoved = true;
	newUnit->m_renderable->GetMaterial()->SetTint(Rgba(150,150,150,220));
}

//-----------------------------------------------------------------------------------------------
void AddAllUnitTypesToMap(Command& theCommand)
{
	UNUSED(theCommand);

	IntVector2 dim = g_theCurrentMap->m_dimensions;

	std::vector<UnitDefinition*> defList;
	UnitDefinition::GetAllUnitDefinitions( &defList );
	
	uint amountOfUnits = defList.size();
	
	IntVector2 currentPos = IntVector2(0,0);
	for(uint i = 0; i < amountOfUnits; i++)
	{
		g_theCurrentMap->CreateUnit(defList.at(i)->m_name, TEAM_RED,  currentPos, 10);
		g_theCurrentMap->CreateUnit(defList.at(i)->m_name, TEAM_BLUE, currentPos + IntVector2(1,0), 10);
		//g_theCurrentMap->CreateUnit(defList.at(i)->m_name, TEAM_RED,  currentPos + IntVector2(2,0), 10);
		//g_theCurrentMap->CreateUnit(defList.at(i)->m_name, TEAM_RED,  currentPos + IntVector2(3,0), 10);

		currentPos.y += 1;
	}
}
