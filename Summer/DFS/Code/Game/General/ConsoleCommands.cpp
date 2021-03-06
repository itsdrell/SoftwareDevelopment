#include "ConsoleCommands.hpp"
#include "Game/Main/Game.hpp"
#include "Game/GameStates/Playing.hpp"
#include "Game/General/Maps/Map.hpp"
#include "Game/General/GameObjects/Unit.hpp"
#include "Game/General/GameObjects/Building.hpp"
#include "../GameStates/Playing.hpp"
#include "Game/General/UI/Container.hpp"
#include "Engine/Core/Tools/DevConsole.hpp"
#include "Game/General/GameObjects/Unit.hpp"
#include "Game/General/Player/CommandingOfficer.hpp"
#include "Engine\Core\General\GameObject2D.hpp"
#include "Engine/Renderer/RenderableComponents/Material.hpp"
#include "GameObjects/Effect.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Game/General/BattleScene/BattleCutscene.hpp"
#include <string>
#include "Engine/Async/Threading.hpp"
#include "Game\General\Maps\BattleMap.hpp"
#include "Maps/MapDefinition.hpp"
#include "Game/GameStates/MapEditor.hpp"

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
	CommandRegister("changeTile", "", "Change tile on map", ChangeTile);
	CommandRegister("closeMenu", "", "Close current Open Menu", CloseOpenMenu);
	CommandRegister("purchase", "", "Purchase Unit in Store", PurchaseUnit);
	CommandRegister("addAllUnits", "", "", AddAllUnitTypesToMap);
	CommandRegister("usePower", "", "", UseCOPower);
	CommandRegister("addEffect", "","", AddEffect);
	CommandRegister("debugMap","Type: debugMap <bool>","Turns on debug map mode", DebugGrid);
	CommandRegister("killTeam","Type: killTeam <teamName>","Kills a team and wins the game", KillAllUnitsOfTeam);
	CommandRegister("battle", "", "create a test battle scene", CreateBattleScene);
	CommandRegister("saveMap", "", "Save the current Map", SaveMap);
	CommandRegister("loadMap", "", "Load map from file", LoadMap);
	CommandRegister("clearMap", "", "Clear map of units and buildings", ClearMap, true);
	CommandRegister("goToGameState", "", "Set state of Game", SetGameState);
	CommandRegister("changeLevelEditorMode", "", "Change Mode of Level Editor", ChangeLevelEditorMode, true);

	// NetSession stuff
	//CommandRegister("send_ping", "", "", SendPing);
	//CommandRegister("send_add", "", "", SendAdd);
	//CommandRegister("testUnreliable", "","a14", UnreliableTest);
	//CommandRegister("testReliable", "","a15", ReliableTest);
	//
	//CommandRegister("net_host", "","name port", NetHost);
	//CommandRegister("net_join", "","name address:port", NetJoin);
	//CommandRegister("net_leave","","", NetLeave);
}

void EndTurn(Command & theCommand)
{
	UNUSED(theCommand);
	
	g_theBattleMap->ClearHoverTiles();
	g_theBattleMap->GoToNextTurn();
	g_theBattleMap->m_actionMenu->ClearWidgets();
}

void HaveAUnitWait(Command& theCommand)
{
	UNUSED(theCommand);

	Unit* currentUnit = g_theBattleMap->m_selectedUnit;

	if(currentUnit == nullptr)
		return;

	currentUnit->m_usedAction = true;
	g_theBattleMap->ClearHoverTiles();
	g_theBattleMap->m_actionMenu->ClearWidgets();
	g_theGame->m_playingState->m_currentPlayState = SELECTING;
}

void CaptureBuilding(Command& theCommand)
{
	UNUSED(theCommand);

	if(g_theBattleMap->m_selectedUnit == nullptr)
	{
		DevConsole::GetInstance()->AddErrorMessage("No building or unit selected");
		return;
	}

	g_theBattleMap->m_buildingToCapture->Capture(*g_theBattleMap->m_selectedUnit); 

	// reset state
	g_theBattleMap->m_selectedUnit->m_usedAction = true;
	g_theBattleMap->m_actionMenu->ClearWidgets();
	g_theGame->m_playingState->m_currentPlayState = SELECTING;
	g_theBattleMap->ClearHoverTiles();
}

void AddUnit(Command& theCommand)
{	
	DevConsole* dc = DevConsole::GetInstance();
	
	// addUnit name team pos health 
	std::string unitName = "grunt";
	TeamName teamName = TEAM_RED;
	IntVector2 pos = IntVector2(0,0);
	int hp = 100;

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
		g_theGame->GetCurrentMap()->CreateUnit(unitName, teamName, pos, hp);
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
		g_theGame->GetCurrentMap()->CreateBuilding(buildingName, teamName, pos);
	}

}

//-----------------------------------------------------------------------------------------------
void ChangeTile(Command& theCommand)
{
	DevConsole* dc = DevConsole::GetInstance();

	// addUnit name team pos health 
	std::string tileName = "default";
	IntVector2 pos = IntVector2(0,0);

	if(IsIndexValid(1, theCommand.m_commandArguements))
		tileName = theCommand.m_commandArguements.at(1);
	if(IsIndexValid(2, theCommand.m_commandArguements))
		pos = ParseString(theCommand.m_commandArguements.at(2), pos);


	if(tileName == "help")
	{
		dc->AddHeader("Params: tileName pos", Rgba::WHITE, 2);
		dc->AddHeader("Possible Tiles to Create", Rgba::WHITE);

		Strings tileNames = TileDefinition::GetAllTileDefinitionNames();
		for(uint i = 0; i < tileNames.size(); i++)
		{
			dc->AddConsoleDialogue(tileNames.at(i), GetRainbowColor((int) i , (int) tileNames.size()));
		}

		dc->AddFooter();
	}
	else
	{
		g_theGame->GetCurrentMap()->ChangeTile(tileName, pos);
	}
}

//-----------------------------------------------------------------------------------------------
void AddEffect(Command& theCommand)
{
	DevConsole* dc = DevConsole::GetInstance();

	// addUnit name team pos health 
	std::string effectName = "explosion";
	IntVector2 pos = IntVector2(0,0);

	if(IsIndexValid(1, theCommand.m_commandArguements))
		effectName = theCommand.m_commandArguements.at(1);
	if(IsIndexValid(2, theCommand.m_commandArguements))
		pos = ParseString(theCommand.m_commandArguements.at(2), pos);
	

	if(effectName == "help")
	{
		dc->AddHeader("Params: effectName pos", Rgba::WHITE, 2);
		dc->AddHeader("Possible Effects to spawn", Rgba::WHITE);

		Strings nameOfEffects = EffectDefinition::GetAllEffectDefinitionNames();
		for(uint i = 0; i < nameOfEffects.size(); i++)
		{
			dc->AddConsoleDialogue(nameOfEffects.at(i), GetRainbowColor((int) i , (int) nameOfEffects.size()));
		}

		dc->AddFooter();

	}
	else
	{
		g_theBattleMap->CreateEffect(effectName, pos);
	}
}

void CloseOpenMenu(Command& theCommand)
{
	UNUSED(theCommand);

	g_theBattleMap->m_currentContainer->CloseMenu();

	if(g_theBattleMap->m_selectedUnit != nullptr)
		g_theBattleMap->PutSelectedUnitBack();
}

void PurchaseUnit(Command& theCommand)
{
	UNUSED(theCommand);
	
	// subtract funds
	g_theBattleMap->m_currentOfficer->m_money -= g_unitToSpawn->m_cost;
	
	Vector2 tilePos = g_theBattleMap->m_selectedBuilding->m_tileReference->m_position.GetAsVector2();
	tilePos *= (1 / TILE_SIZE);


	// add unit
	Unit* newUnit = g_theBattleMap->CreateUnit(
		g_unitToSpawn->m_name, 
		g_theBattleMap->m_currentOfficer->m_team, 
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

	IntVector2 dim = g_theBattleMap->m_dimensions;

	std::vector<UnitDefinition*> defList;
	UnitDefinition::GetAllUnitDefinitions( &defList );
	
	uint amountOfUnits = (uint) defList.size();
	
	IntVector2 currentPos = IntVector2(0,0);
	for(uint i = 0; i < amountOfUnits; i++)
	{
		g_theBattleMap->CreateUnit(defList.at(i)->m_name, TEAM_RED,  currentPos, 10);
		g_theBattleMap->CreateUnit(defList.at(i)->m_name, TEAM_BLUE, currentPos + IntVector2(1,0), 10);
		//g_theBattleMap->CreateUnit(defList.at(i)->m_name, TEAM_RED,  currentPos + IntVector2(2,0), 10);
		//g_theBattleMap->CreateUnit(defList.at(i)->m_name, TEAM_RED,  currentPos + IntVector2(3,0), 10);

		currentPos.y += 1;
	}
}

//-----------------------------------------------------------------------------------------------
void UseCOPower(Command& theCommand)
{
	UNUSED(theCommand);

	g_theBattleMap->m_currentOfficer->m_definition->m_power->UsePower();
}


//-----------------------------------------------------------------------------------------------
void KillAllUnitsOfTeam(Command& theCommand)
{
	std::string input;
	DevConsole* dc; 

	if(theCommand.m_commandArguements.size() == 1)
		input = "help";
	else
		input = theCommand.m_commandArguements.at(1);

	if(input == "help")
	{
		dc->AddConsoleDialogue(ConsoleDialogue("Teams to kill >:D ", Rgba::WHITE));

		TurnOrder t = g_theGame->m_playingState->m_currentMap->m_turnOrder;
		dc->AddSpace(1);

		for(uint i = 0; i < t.m_order.size(); i++)
		{
			std::string teamName = TeamNameToString(t.m_order.at(i));

			dc->AddConsoleDialogue(ConsoleDialogue(teamName, GetRainbowColor(i, (uint)t.m_order.size())));
		}

		dc->AddSpace(1);
	}
	else
	{
		TeamName tm = StringFromTeamName(input);

		for(uint i = 0; i < g_theGame->m_playingState->m_currentMap->m_units.size(); i++)
		{
			Unit*& currentUnit = g_theGame->m_playingState->m_currentMap->m_units.at(i);

			if(currentUnit->m_team == tm)
				currentUnit->Die();
		}
	}

}

//-----------------------------------------------------------------------------------------------
void CreateBattleScene(Command& theCommand)
{
	// battle attackUnitName team startHP endHP defUnitName team startHP endHP 
	// battle help
	// battle (gives random battle)
	DevConsole* dc = DevConsole::GetInstance();

	// attacker
	std::string attackerUnitName = UnitDefinition::GetRandomUnitName();
	TeamName attackerTeamName = TEAM_RED;
	int attackerStartHP = GetRandomIntRange(10, 100);
	int attackerendHP = GetRandomIntRange(0, attackerStartHP);

	// defender
	std::string defUnitName = UnitDefinition::GetRandomUnitName();
	TeamName defTeamName = TEAM_BLUE;
	int defStartHP = GetRandomIntRange(10, 100);
	int defEndHP = GetRandomIntRange(0, defStartHP);;

	// checks and sets
	if(IsIndexValid(1, theCommand.m_commandArguements))
		attackerUnitName = theCommand.m_commandArguements.at(1);
	if(IsIndexValid(2, theCommand.m_commandArguements))
		attackerTeamName = StringFromTeamName(theCommand.m_commandArguements.at(2));
	if(IsIndexValid(3, theCommand.m_commandArguements))
		attackerStartHP = ParseString(theCommand.m_commandArguements.at(3), attackerStartHP);
	if(IsIndexValid(4, theCommand.m_commandArguements))
		attackerendHP = ParseString(theCommand.m_commandArguements.at(4), attackerendHP);

	if(IsIndexValid(5, theCommand.m_commandArguements))
		defUnitName = theCommand.m_commandArguements.at(5);
	if(IsIndexValid(6, theCommand.m_commandArguements))
		defTeamName = StringFromTeamName(theCommand.m_commandArguements.at(6));
	if(IsIndexValid(7, theCommand.m_commandArguements))
		defStartHP = ParseString(theCommand.m_commandArguements.at(7), defStartHP);
	if(IsIndexValid(8, theCommand.m_commandArguements))
		defEndHP = ParseString(theCommand.m_commandArguements.at(8), defEndHP);


	if(attackerUnitName == "help")
	{
		dc->AddHeader("Params: attackUnitName team startHP endHP defUnitName team startHP endHP", Rgba::WHITE, 2);
		dc->AddFooter();

	}
	else
	{
		// create attacker unit
		Unit attacker = Unit(attackerUnitName, attackerTeamName, attackerendHP);

		// create defender unit
		Unit defender = Unit(defUnitName, defTeamName, defEndHP);

		// create battle result
		BattleResults br = BattleResults(attacker, defender, attackerStartHP, defStartHP);

		// set it on the battle scene
		g_theBattleMap->m_battleScene->SetBattleResults(br);

		// this is just to make sure we got what we wanted, or if we do random make sure what we see is what we put in
		dc->AddConsoleDialogue(Stringf("Created a battle between %s on team %s with start hp: %i endHp: %i",
			attackerUnitName.c_str(), TeamNameToString(attackerTeamName).c_str(), attackerStartHP, attackerendHP), Rgba::WHITE);

		dc->AddConsoleDialogue(Stringf("VS a %s on team %s with start hp: %i endHp: %i",
			defUnitName.c_str(), TeamNameToString(defTeamName).c_str(), defStartHP, defEndHP), Rgba::WHITE);

	}
}

//-----------------------------------------------------------------------------------------------
void SaveMap(Command& theCommand)
{
	String mapName = "idk man";
	
	if(IsIndexValid(1, theCommand.m_commandArguements))
		mapName = theCommand.m_commandArguements.at(1);

	g_theGame->GetCurrentMap()->Save(mapName);
}

//-----------------------------------------------------------------------------------------------
void LoadMap(Command& theCommand)
{
	String mapName = "idk man";

	if(IsIndexValid(1, theCommand.m_commandArguements))
		mapName = theCommand.m_commandArguements.at(1);

	g_theGame->GetCurrentMap()->ClearMap();
	
	MapDefinition* theDef = MapDefinition::GetDefinition(mapName);
	CommandRunScriptFromFile(theDef->m_scriptPath.c_str());

	g_theGame->GetCurrentMap()->RecreateMapRenderable();
}

//-----------------------------------------------------------------------------------------------
void ClearMap(Command& theCommand)
{
	//Map* currentMap = g_theGame->GetCurrentMap();
	//currentMap->ClearMap();

	g_theGame->m_mapEditorState->CreateNewMap();
}

//-----------------------------------------------------------------------------------------------
void SetGameState(Command& theCommand)
{
	std::string input;
	std::string stateFrom;
	DevConsole* dc; 

	if(theCommand.m_commandArguements.size() == 1)
		input = "help";
	else
	{
		input = theCommand.m_commandArguements.at(1);

		if(IsIndexValid(2, theCommand.m_commandArguements))
			stateFrom = theCommand.m_commandArguements.at(2);
		else
			stateFrom = GetGameStateAsString(g_theGame->m_currentState);
	}

	if(input == "help")
	{
		dc->AddConsoleDialogue(ConsoleDialogue("States to go to: Syntax is <State to go to> <State you are in> ", Rgba::WHITE));

		for(int i = 0; i < NUM_OF_GAME_STATES; i++)
		{
			String theState = GetGameStateAsString((GameStates) i);
			dc->AddConsoleDialogue(ConsoleDialogue(theState, Rgba::WHITE));
		}

		dc->AddSpace(1);
	}
	else
	{
		g_theGame->GoToGameStateFrom(GetGameStateFromString(input), GetGameStateFromString(stateFrom));
	}
}

//-----------------------------------------------------------------------------------------------
void ChangeLevelEditorMode(Command& theCommand)
{
	String modeToGoTo = theCommand.GetNextString();
	DevConsole* dc = DevConsole::GetInstance();

	if(modeToGoTo != "help")
	{
		if(modeToGoTo == "unit") { g_theGame->m_mapEditorState->m_selectionType = SELECTIONTYPE_UNIT; }
		if(modeToGoTo == "building") { g_theGame->m_mapEditorState->m_selectionType = SELECTIONTYPE_BUILDING; }
		if(modeToGoTo == "tile") { g_theGame->m_mapEditorState->m_selectionType = SELECTIONTYPE_TILE; }
		if(modeToGoTo == "delete") { g_theGame->m_mapEditorState->m_selectionType = SELECTIONTYPE_DELETE; }
	}
	else
	{
		dc->AddConsoleDialogue(ConsoleDialogue("States to go to: Syntax is <State to go to> <State you are in> ", Rgba::WHITE));

		dc->AddConsoleDialogue("Enter one of these: <unit> <building> <tile> <delete>");

		dc->AddSpace(1);
	}
	
}

//-----------------------------------------------------------------------------------------------
void DebugGrid(Command& theCommand)
{
	std::string input;

	if(theCommand.m_commandArguements.size() == 1)
		input = "true";
	else
		input = theCommand.m_commandArguements.at(1);

	bool check = ParseString(input, false);


	if(check)
	{
		// Make sure there are only one of them
		g_theGame->m_playingState->RemoveRenderable(g_theGame->m_playingState->m_currentMap->m_debugRenderable);
		g_theGame->m_playingState->AddRenderable(g_theGame->m_playingState->m_currentMap->m_debugRenderable);
		g_theGame->m_playingState->m_showHeatmap = true;
	}
	else
	{
		g_theGame->m_playingState->RemoveRenderable(g_theGame->m_playingState->m_currentMap->m_debugRenderable);
		g_theGame->m_playingState->m_showHeatmap = false;

	}
}

//-----------------------------------------------------------------------------------------------
// void AddConnection(Command & theCommand)
// {
// 	uint8_t idx = 0U;
// 	NetAddress addr;
// 
// 	if(IsIndexValid(1, theCommand.m_commandArguements))
// 		idx = (uint8_t) stoi(theCommand.m_commandArguements.at(1));
// 	if(IsIndexValid(2, theCommand.m_commandArguements))
// 		addr = NetAddress(theCommand.m_commandArguements.at(2).c_str());
// 
// 	// notice this can't fail - we do no validation that that
// 	// address is reachable.   UDP can't tell; 
// 	NetSession *session = Game::GetNetSession(); 
// 	NetConnection *cp = session->AddConnection( idx, addr ); 
// 	if (cp == nullptr) 
// 	{
// 		DevConsole::AddErrorMessage( "Failed to add connection." ); 
// 	} 
// 	else 
// 	{
// 		DevConsole::AddErrorMessage( Stringf("Connection added at index [%u]", idx )); 
// 	}
// }

//===============================================================================================
// NET STUFF
//===============================================================================================
//-----------------------------------------------------------------------------------------------
void SendPing(Command & theCommand)
{
	uint8_t idx = 0U; 
	if(IsIndexValid(1, theCommand.m_commandArguements))
		idx = (uint8_t) stoi(theCommand.m_commandArguements.at(1));

	NetSession *session = Game::GetNetSession(); 
	NetConnection *cp = session->GetConnection( idx ); 
	if (nullptr == cp) 
	{
		DevConsole::AddErrorMessage( Stringf("No connection at index %u", idx )); 
		return; 
	}

	NetMessage* msg = new NetMessage("ping"); 

	String str = "ping";
	if(IsIndexValid(2, theCommand.m_commandArguements))
		str = theCommand.m_commandArguements.at(2); 

	msg->WriteString( str.c_str() ); 

	// messages are sent to connections (not sessions)
	cp->Send( *msg ); 
}

//-----------------------------------------------------------------------------------------------
void SendAdd(Command & theCommand)
{
	uint8_t idx = 0U; 
	float val0 = 0.f;
	float val1 = 0.f;

	if(IsIndexValid(1, theCommand.m_commandArguements))
		idx = (uint8_t) stoi(theCommand.m_commandArguements.at(1));
	if(IsIndexValid(2, theCommand.m_commandArguements))
		val0 = stof(theCommand.m_commandArguements.at(2));
	if(IsIndexValid(3, theCommand.m_commandArguements))
		val1 = stof(theCommand.m_commandArguements.at(3));

	NetSession *sp = Game::GetNetSession(); 
	NetConnection *cp = sp->GetConnection( idx ); 
	if (cp == nullptr) 
	{
		DevConsole::AddErrorMessage( Stringf("Unknown connection: %u", idx )); 
		return; 
	}

	NetMessage* msg = new NetMessage("add"); 
	msg->WriteBytes( sizeof(float), &val0 ); 
	msg->WriteBytes( sizeof(float), &val1 ); 
	cp->Send( *msg );
}

//-----------------------------------------------------------------------------------------------
void UnreliableTest(Command& theCommand)
{
	uint idx = 0U; 
	uint count = 1U;


	if(IsIndexValid(1, theCommand.m_commandArguements))
		idx = (uint) stoi(theCommand.m_commandArguements.at(1));
	if(IsIndexValid(2, theCommand.m_commandArguements))
		count = (uint) stoi(theCommand.m_commandArguements.at(2));

	NetSession *sp = Game::GetNetSession(); 
	sp->m_idx = idx;
	sp->m_totalAmount = count;
	sp->m_currentAmount = count;
}

//-----------------------------------------------------------------------------------------------
void ReliableTest(Command& theCommand)
{
	uint idx = 0U; 
	uint count = 1U;


	if(IsIndexValid(1, theCommand.m_commandArguements))
		idx = (uint) stoi(theCommand.m_commandArguements.at(1));
	if(IsIndexValid(2, theCommand.m_commandArguements))
		count = (uint) stoi(theCommand.m_commandArguements.at(2));

	NetSession *sp = Game::GetNetSession(); 
	sp->m_reliableIdx = idx;
	sp->m_reliableTotalAmount = count;
	sp->m_reliableCurrentAmount = count;
}

//-----------------------------------------------------------------------------------------------
void NetHost(Command & theCommand)
{
	NetSession* theSession = Game::GetNetSession();

	if(theSession->IsRunning())
	{
		// error
		DevConsole::AddErrorMessage("Could not Host because already running!");
	}

	String myName = "default host";
	const char* port = GAME_PORT;

	if(IsIndexValid(1, theCommand.m_commandArguements))
		myName = theCommand.m_commandArguements.at(1);
	if(IsIndexValid(2, theCommand.m_commandArguements))
		port = theCommand.m_commandArguements.at(2).c_str();

	theSession->Host(myName.c_str(), port);
}

//-----------------------------------------------------------------------------------------------
void NetJoin(Command & theCommand)
{
	NetSession* theSession = Game::GetNetSession();

	if(theSession->IsRunning())
	{
		// error
		DevConsole::AddErrorMessage("Could not Join because already running!");
	}

	String hostName = "idk man"; // host ???
	String AddressAsString = "";
	
	if(IsIndexValid(1, theCommand.m_commandArguements))
		hostName = theCommand.m_commandArguements.at(1);
	if(IsIndexValid(2, theCommand.m_commandArguements))
		AddressAsString = theCommand.m_commandArguements.at(2);

	NetAddress hostAddress = NetAddress(AddressAsString.c_str());

	theSession->Join(hostName.c_str(), hostAddress);
}

//-----------------------------------------------------------------------------------------------
void NetLeave(Command& theCommand)
{
	NetSession* theSession = Game::GetNetSession();

	theSession->Disconnect();
}

