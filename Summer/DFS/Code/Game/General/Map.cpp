#include "Map.hpp"
#include "Tiles\TileDefinition.hpp"
#include "Engine\Renderer\Images\Sprites\Sprite.hpp"
#include "Engine\Core\General\EngineCommon.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteSheet.hpp"
#include "Engine\Renderer\Systems\MeshBuilder.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "..\Main\Game.hpp"
#include "Game\GameStates\Playing.hpp"
#include "GameObjects\Unit.hpp"
#include "..\SystemsAndTools\GameObject2D.hpp"
#include "GameObjects\Building.hpp"
#include "..\..\TankGame\Code\Game\Main\GameCommon.hpp"
#include "Engine\Core\Tools\Command.hpp"
#include "Engine\Core\Utils\StringUtils.hpp"
#include "Game\General\Tiles\Tile.hpp"
#include "Engine\Core\General\HeatMap.hpp"
#include "Game\Main\GameCommon.hpp"
#include "Engine\Core\Tools\DevConsole.hpp"
#include "Engine\Renderer\Systems\DebugRenderSystem.hpp"
#include "Game\General\Player\CommandingOfficer.hpp"
#include "UI\Container.hpp"
#include "UI\HUD.hpp"
#include "UI\UIWidget.hpp"
#include "UI\UnitWidget.hpp"

//====================================================================================
// Externs
Map* g_theCurrentMap = nullptr;


//====================================================================================
// Console commands
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
				currentUnit->m_isDead = true;
		}
	}
	
}

//====================================================================================
// TURN ORDER
void TurnOrder::GoToNextTurn()
{
	m_current++;
	
	if(m_current >= m_order.size())
	{
		m_current = 0;
		m_turnCount++;
	}

}

//====================================================================================
// This function is cool cause we don't need to worry about what the team order is
// because this will be called when we create a unit or building so we auto know
// what team will be playing
void TurnOrder::CheckIfTeamIsRegisteredAndAdd(TeamName teamToCheck)
{
	if(teamToCheck == TEAM_NONE)
		return;
	
	for(uint i = 0; i < m_order.size(); i++)
	{
		TeamName current = m_order.at(i);

		// we already know about the team
		if(current == teamToCheck)
			return;
	}

	// we don't know about the team so lets add em
	AddTeam(teamToCheck);
	g_theCurrentMap->CreateCommandingOfficer(teamToCheck);
}

//====================================================================================

Map::Map(std::string name, const IntVector2 & dimensions)
{
	m_name = name;
	m_dimensions = dimensions;

	m_heatmap = new HeatMap(dimensions);

	CreateMapRenderable();
	CreateMapRenderable(true);

	CommandRegister("debugMap","Type: debugMap <bool>","Turns on debug map mode", DebugGrid);
}

Map::Map(std::string name, Image& mapImage)
{
	m_name = name;
	m_dimensions = mapImage.m_dimensions;
	m_mapImage = mapImage;

	m_actionMenu = new Container("Main Menu", 5, Vector2(30.f, 30.f), AABB2(-10.f, 10.f));
	m_storeMenu = new Container("Purchase", 10, Vector2(25.f, 0.f), AABB2(-20.f, -40.f, 20.f, 40.f));
	m_hud = new HUD();

	m_heatmap = new HeatMap(m_dimensions);

	CreateMapRenderableFromImage();
	CreateMapRenderable(true);
	
	CommandRegister("debugMap","Type: debugMap <bool>","Turns on debug map mode", DebugGrid);
	CommandRegister("killTeam","Type: killTeam <teamName>","Kills a team and wins the game", KillAllUnitsOfTeam);

}

void Map::Update()
{
	UpdateCurrentCO();
	
	for(uint i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects.at(i)->Update();
	}

	
	UpdateUI();
	CheckForVictory();
}

void Map::UpdateCurrentCO()
{
	if(m_currentOfficer != m_officers.at(m_turnOrder.m_current))
		m_currentOfficer = m_officers.at(m_turnOrder.m_current);
}

void Map::UpdateUI()
{
	m_actionMenu->Update();
	m_storeMenu->Update();
}

void Map::CreateMapRenderable(bool makeDebug)
{
	if(makeDebug == false)
		m_mapRenderable = new Renderable2D();
	else
		m_debugRenderable = new Renderable2D();
	
	// For now, hacky
	TileDefinition* grass = GetTileDefinition("default");
	Sprite theSprite;

	float tileSize = TILE_SIZE;

	if(makeDebug == false)
		theSprite = Sprite(*g_tileSpriteSheet.m_spriteSheetTexture, Vector2::ONE, tileSize, Vector2(.5f,.5f), grass->m_uvCoords);
	else
		theSprite = Sprite(*g_theRenderer->CreateOrGetTexture("Data/Images/Sprites/highlightCell.png"), Vector2::ONE, tileSize, Vector2(.5f,.5f));


	Vector2 currentPos = Vector2::ZERO;
	float stepSize = tileSize;

	MeshBuilder mb;
	int theTileSize = TILE_SIZE_INT;
	for(int y = 0; y < m_dimensions.y; y++)
	{
		for(int x = 0; x < m_dimensions.x; x++)
		{
			Tile newTile = Tile(currentPos.GetVector2AsInt(), *grass, theTileSize);
			m_tiles.push_back(newTile);
			
			mb.AddFromSprite(currentPos, theSprite);


			currentPos.x += stepSize;
		}

		currentPos.x = 0.f;
		currentPos.y += stepSize;
	}


	//---------------------------------------------------------
	// Create the mesh
	Mesh* theMesh = mb.CreateMesh<Vertex3D_PCU>();

	if(makeDebug == false)
	{
		m_mapRenderable->SetMesh(theMesh);

		Material* mapMat = Material::CreateOrGetMaterial("default");
		mapMat->SetTexture(0, g_tileSpriteSheet.m_spriteSheetTexture);
		m_mapRenderable->SetMaterial(mapMat);

		g_theGame->m_playingState->AddRenderable(m_mapRenderable);
	}
	else
	{
		m_debugRenderable->SetMesh(theMesh);

		Material* mapMat = Material::CreateOrGetMaterial("sprite");
		mapMat->SetTexture(0,g_theRenderer->CreateOrGetTexture("Data/Images/Sprites/highlightCell.png"));
		m_debugRenderable->SetMaterial(mapMat);
		m_debugRenderable->SetLayer(-10);

		//g_theGame->m_playingState->AddRenderable(m_debugRenderable);

	}
	


}

void Map::CreateMapRenderableFromImage()
{
	m_mapRenderable = new Renderable2D();

	float tileSize = TILE_SIZE;

	Vector2 currentPos = Vector2::ZERO;
	float stepSize = tileSize;

	MeshBuilder mb;
	int theTileSize = TILE_SIZE_INT;
	for(int y = 0; y < m_dimensions.y; y++)
	{
		for(int x = 0; x < m_dimensions.x; x++)
		{
			Rgba color = m_mapImage.GetColorAt(x,y);
			TileDefinition* currentDef = GetTileDefinition(color);

			Tile newTile = Tile(currentPos.GetVector2AsInt(), *currentDef, theTileSize);
			m_tiles.push_back(newTile);

			Sprite theSprite = Sprite(*g_tileSpriteSheet.m_spriteSheetTexture, Vector2::ONE, tileSize, Vector2(.5f,.5f), currentDef->m_uvCoords);
			mb.AddFromSprite(currentPos, theSprite);


			currentPos.x += stepSize;
		}

		currentPos.x = 0.f;
		currentPos.y += stepSize;
	}


	//---------------------------------------------------------
	// Create the mesh
	Mesh* theMesh = mb.CreateMesh<Vertex3D_PCU>();

	m_mapRenderable->SetMesh(theMesh);

	Material* mapMat = Material::CreateOrGetMaterial("default");
	mapMat->SetTexture(0, g_tileSpriteSheet.m_spriteSheetTexture);
	m_mapRenderable->SetMaterial(mapMat);

	g_theGame->m_playingState->AddRenderable(m_mapRenderable);
}

void Map::CreateCommandingOfficer(TeamName theTeam)
{
	String playerName = "Player " + std::to_string(m_officers.size() + 1U);
	
	CommandingOfficer* newOfficer = new CommandingOfficer(playerName, theTeam, PLAYER_CO);

	m_officers.push_back(newOfficer);
}

Tile* Map::GetTile(const Vector2& worldPos)
{
	//int Tilesize = TILE_SIZE_INT;
	
	for(uint i = 0; i < m_tiles.size(); i++)
	{
		Tile* current = &m_tiles.at(i);

		if( current->IsPointInsideTile(worldPos.GetVector2AsInt()) )
		{
			return current;
		}
	}

	return nullptr; 
}

Tile* Map::GetTile(const IntVector2& tilePos)
{
	return &m_tiles.at(tilePos.y * m_dimensions.x + tilePos.x);
}

bool Map::SelectUnit(Vector2 pos)
{
	Tile* selectedTile = GetTile(pos);

	if(selectedTile->m_unit == nullptr)
		return false;

	m_selectedUnit = selectedTile->m_unit;
	m_selectedUnit->m_tileIAmOn = selectedTile; // keep this so we know where to go back to
	selectedTile->m_unit = nullptr;

	return true;
}

void Map::PlaceUnit(Vector2 pos)
{
	Tile* selectedTile = GetTile(pos);

	selectedTile->m_unit = m_selectedUnit;
	m_selectedUnit->m_tileIAmOn = selectedTile;
	m_selectedUnit->m_beenMoved = true;
}

void Map::PutSelectedUnitBack()
{
	Tile* tileToGoBackTo = m_selectedUnit->m_tileIAmOn;

	tileToGoBackTo->m_unit = m_selectedUnit;
	m_selectedUnit->m_transform.SetLocalPosition(tileToGoBackTo->GetCenterOfTile());

}

bool Map::CheckForAction(const IntVector2& mousePos)
{
	int tileSize = TILE_SIZE_INT;

	bool check = false;

	// We just need to see if the tile pos we have is in our cached off list
	for(uint i = 0; i < m_attackTiles.size(); i++)
	{
		HoverTile* currentTile = m_attackTiles.at(i);
		
		IntVector2 current = currentTile->m_tileCoords * tileSize;

		if(mousePos == current)
		{
			check = true;
			
			// do something based off the action
			switch (currentTile->m_type)
			{
			case ATTACK_RANGE_TILE_TYPE:
				AttackUnitAt(m_attackTiles.at(i)->m_tileCoords);
				break;
			default:
				break;
			}
			
		}
	}


	return check;
}

void Map::AttackUnitAt(const IntVector2& tileCoords)
{
	Tile* theTile = GetTile(tileCoords);

	PlayOneShot("default");

	// right now, one shot
	Unit* target = theTile->m_unit;
	m_selectedUnit->m_usedAction = true;
	target->m_isDead = true;
	theTile->m_unit = nullptr; // important for ui
}

void Map::CreateMovementTiles(const Unit& theUnitToUse)
{
	m_heatmap->ResetHeatMap();
	
	// Gotta translate from world coords to tileCoords
	IntVector2 worldCoords = GetTileCoords(theUnitToUse.m_transform.GetLocalPosition());
	IntVector2 tileCoords = IntVector2(worldCoords.x / TILE_SIZE_INT, worldCoords.y / TILE_SIZE_INT);

	m_heatmap->AddHeat(tileCoords);

	std::vector<IntVector2> tilePos = m_heatmap->GetAllTileCoordsWithHeatLessOrEqual(theUnitToUse.GetMovement());
	
	for(uint i = 0; i < tilePos.size(); i++)
	{
		if(CanUnitEnterThatTile(theUnitToUse, tilePos.at(i)))
		{
			HoverTile* newTile = new HoverTile(tilePos.at(i));

			m_hoverTiles.push_back(newTile);
		}

		//HoverTile* newTile = new HoverTile(tilePos.at(i));
		//
		//m_hoverTiles.push_back(newTile);
	}
	
	
}

void Map::CreateActionTiles(const Unit& theUnitToUse)
{
	CreateAttackTiles(theUnitToUse);
}

void Map::CreateAttackTiles(const Unit& theUnitToUse, bool showRange)
{
	m_heatmap->ResetHeatMap();

	// Gotta translate from world coords to tileCoords
	IntVector2 worldCoords = GetTileCoords(theUnitToUse.m_transform.GetLocalPosition());
	IntVector2 tileCoords = IntVector2(worldCoords.x / TILE_SIZE_INT, worldCoords.y / TILE_SIZE_INT);

	m_heatmap->AddHeat(tileCoords);

	std::vector<IntVector2> tilePos = m_heatmap->GetAllTileCoordsWithHeatInRangeOf(theUnitToUse.GetAttackRange());

	for(uint i = 0; i < tilePos.size(); i++)
	{
		
		// see if we want to get the attack range of the unit, or just show what we can attack
		if(showRange == false)
		{
			if(CanPlayerAttackUnitOnTile(theUnitToUse, tilePos.at(i)))
			{
				HoverTile* newTile = new HoverTile(tilePos.at(i), ATTACK_RANGE_TILE_TYPE);

				m_attackTiles.push_back(newTile);
			}
		}
		else
		{
			HoverTile* newTile = new HoverTile(tilePos.at(i), ATTACK_RANGE_TILE_TYPE);

			m_attackTiles.push_back(newTile);
		}

	}
}

void Map::CreateStoreUI()
{
	String typeOfUnit = m_selectedBuilding->m_definition->m_typeOfUnitToSpawn;

	std::vector<UnitDefinition*> units;
	UnitDefinition::GetAllUnitDefinitionsWithStoreTag(typeOfUnit, &units);
	
	for(uint i = 0; i < units.size(); i++)
	{
		UIWidget* unitWidget = new UnitWidget(m_currentOfficer->m_team, *units.at(i), *UIWidgetDefinition::GetUIWidgetDefinition("unit"));
		m_storeMenu->AddWidget(*unitWidget);
	}
	
	m_currentContainer = m_storeMenu;
}

bool Map::CanUnitCaptureBuilding(const Unit& theUnitToUse)
{
	if(theUnitToUse.m_definition->m_canCapture == false)
		return false;
	
	IntVector2 worldCoords = GetTileCoords(theUnitToUse.m_transform.GetLocalPosition());
	IntVector2 tileCoords = IntVector2(worldCoords.x / TILE_SIZE_INT, worldCoords.y / TILE_SIZE_INT);

	Tile* theTile = GetTile(tileCoords);
	
	
	Building* theBuilding = theTile->m_building;
	
	if(theBuilding != nullptr)
	{
		if(theBuilding->m_team != theUnitToUse.m_team)
		{
			m_buildingToCapture = theBuilding;
			return true;
		}
	}

	return false;
}

bool Map::CanPlayerMoveThere(IntVector2& posToCheck)
{
	int tileSize = TILE_SIZE_INT;

	bool check = false;

	// We just need to see if the tile pos we have is in our cached off list
	for(uint i = 0; i < m_hoverTiles.size(); i++)
	{
		IntVector2 current = m_hoverTiles.at(i)->m_tileCoords * tileSize;

		if(posToCheck == current)
			check = true;
	}


	return check;
}

bool Map::CanPlayerAttackUnitOnTile(const Unit& theUnitToUse, const IntVector2& posToCheck)
{
	Tile* currentTile = GetTile(posToCheck);

	if(currentTile->m_unit != nullptr)
	{
		Unit* theUnit = currentTile->m_unit;

		if(theUnit->m_team != theUnitToUse.m_team)
		{
			if(DoTagsShareATag(theUnitToUse.m_definition->m_attackTags, theUnit->m_definition->m_movementTags))
				return true;
		}
	}

	return false;
}

bool Map::CanUnitEnterThatTile(const Unit& theUnitToUse, IntVector2& tileToCheck)
{
	Tags unitTags = theUnitToUse.GetMovementTags();

	Tile* currentTile = GetTile(tileToCheck);

	if(currentTile == nullptr)
		return false;

	// make sure the unit isn't me
	if(currentTile->m_unit != nullptr)
	{
		if(currentTile->m_unit != &theUnitToUse)
			return false;
	}

	Tags tileTags = currentTile->m_definition->m_movementTags;
	
	return DoTagsShareATag(unitTags, tileTags);
}

void Map::ClearHoverTiles()
{
	for(uint i = 0; i < m_hoverTiles.size(); i++)
	{
		g_theGame->m_playingState->RemoveRenderable(m_hoverTiles.at(i)->m_renderable);

		delete m_hoverTiles.at(i);
	}

	m_hoverTiles.clear();
}

void Map::ClearAttackTiles()
{
	for(uint i = 0; i < m_attackTiles.size(); i++)
	{
		g_theGame->m_playingState->RemoveRenderable(m_attackTiles.at(i)->m_renderable);

		delete m_attackTiles.at(i);
	}

	m_attackTiles.clear();
}

void Map::RemoveDeadGameObjects()
{
	std::vector<uint> deadindices;
	
	for(uint i = 0; i < m_gameObjects.size(); i++)
	{
		GameObject2D* current = m_gameObjects.at(i);

		if(current->m_isDead)
			deadindices.push_back(i);
	}

	for(uint j = 0; j < deadindices.size(); j++)
	{
		uint idx = deadindices.at(j);
		g_theGame->m_playingState->RemoveRenderable(m_gameObjects.at(idx)->m_renderable);
		m_gameObjects.erase(m_gameObjects.begin() + deadindices.at(j));
	}

	deadindices.clear();

	// Units as well
	for(uint i = 0; i < m_units.size(); i++)
	{
		Unit* current = m_units.at(i);

		if(current->m_isDead)
			deadindices.push_back(i);
	}

	for(uint j = 0; j < deadindices.size(); j++)
	{
		uint idx = deadindices.at(j);
		g_theGame->m_playingState->RemoveRenderable(m_units.at(idx)->m_renderable);
		m_units.erase(m_units.begin() + deadindices.at(j));
	}
}

void Map::GoToNextTurn()
{
	// This needs to reset all the states of the units
	m_turnOrder.GoToNextTurn();

	// #TODO This could later be optimized by storing off units that were played and 
	// only looping through those instead of all units
	for(uint i = 0; i < m_units.size(); i++)
	{
		Unit*& current = m_units.at(i);
		
		current->m_beenMoved = false;
		current->m_usedAction = false;
	}

	GenerateIncome();
}

void Map::GenerateIncome()
{
	uint allowance = 0U;

	for(uint i = 0; i < m_buildings.size(); i ++)
	{
		Building& current = *m_buildings.at(i);

		if(current.m_team == m_currentOfficer->m_team)
			allowance += MONEY_PER_BUILDING;
	}

	m_currentOfficer->m_money += allowance;
}

void Map::CheckForVictory()
{
	// Later this could have other objectives based off the map (enum)
	
	bool victory = IsATeamWithoutUnits();

	if(victory)
		DebugRenderLog(10.f, "VICTORY");
}

bool Map::IsATeamWithoutUnits()
{
	std::vector<TeamName> teams = m_turnOrder.m_order;

	for(uint teamNameIndex = 0; teamNameIndex < teams.size(); teamNameIndex++)
	{
		TeamName currentTeam = teams.at(teamNameIndex);
		bool foundOneAlive = false;

		for(uint unitIndex = 0; unitIndex < m_units.size(); unitIndex++)
		{
			Unit currentUnit = *m_units.at(unitIndex);

			if(currentUnit.m_team == currentTeam)
				foundOneAlive = true;
		}

		// they have no units 
		if(foundOneAlive == false)
			return true;
	}
	
	return false;
}

Unit* Map::CreateUnit(std::string name, TeamName team, IntVector2 pos, int hp)
{
	Unit* newUnit = new Unit(team, *UnitDefinition::GetUnitDefinition(name));
	Vector2 position = pos.GetAsVector2() * TILE_SIZE;
	newUnit->SetLocalPosition(position);

	newUnit->m_health = hp;

	// Put the unit on the tile
	Tile* tilePlacedOn = GetTile(position);
	tilePlacedOn->m_unit = newUnit;

	// make sure we know about their team
	m_turnOrder.CheckIfTeamIsRegisteredAndAdd(team);

	AddGameObject(*newUnit);
	AddUnit(*newUnit);

	return newUnit;
}

void Map::CreateBuilding(const std::string& name, const TeamName& team, const IntVector2& pos)
{
	Building* newBuilding = new Building(team, *BuildingDefinition::GetDefinition(name));
	Vector2 position = pos.GetAsVector2() * TILE_SIZE;
	newBuilding->SetLocalPosition(position);

	Tile* tilePlacedOn = GetTile(position);
	tilePlacedOn->m_building = newBuilding;

	newBuilding->m_tileReference = tilePlacedOn;

	if(tilePlacedOn->m_definition != newBuilding->m_definition->m_tileToSpawnBeneath)
		tilePlacedOn->m_definition = newBuilding->m_definition->m_tileToSpawnBeneath;

	if(team != TEAM_NONE)
		m_turnOrder.CheckIfTeamIsRegisteredAndAdd(team);

	AddGameObject(*newBuilding);
	AddBuilding(*newBuilding);
}

