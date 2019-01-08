#include "BattleMap.hpp"
#include "Game\General\Maps\Map.hpp"
#include "Game\General\Tiles\TileDefinition.hpp"
#include "Engine\Renderer\Images\Sprites\Sprite.hpp"
#include "Engine\Core\General\EngineCommon.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteSheet.hpp"
#include "Engine\Renderer\Systems\MeshBuilder.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "Game\Main\Game.hpp"
#include "Game\GameStates\Playing.hpp"
#include "Game\General\GameObjects\Unit.hpp"
#include "Engine\Core\General\GameObject2D.hpp"
#include "Game\General\GameObjects\Building.hpp"
#include "..\..\TankGame\Code\Game\Main\GameCommon.hpp"
#include "Engine\Core\Tools\Command.hpp"
#include "Engine\Core\Utils\StringUtils.hpp"
#include "Game\General\Tiles\Tile.hpp"
#include "Engine\Core\General\HeatMap.hpp"
#include "Game\Main\GameCommon.hpp"
#include "Engine\Core\Tools\DevConsole.hpp"
#include "Engine\Renderer\Systems\DebugRenderSystem.hpp"
#include "Game\General\Player\CommandingOfficer.hpp"
#include "Game\General\UI\Container.hpp"
#include "Game\General\UI\HUD.hpp"
#include "Game\General\UI\UIWidget.hpp"
#include "Game\General\UI\UnitWidget.hpp"
#include "Game\General\Maps\GameHeatMap.hpp"
#include "Game\General\GameObjects\Effect.hpp"
#include "Game\General\BattleScene\BattleCutscene.hpp"

//===============================================================================================
BattleMap* g_theBattleMap = nullptr;

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

	// If you get an error here the cast failed 
	BattleMap* theMap = (BattleMap*) g_theBattleMap;
	theMap->CreateCommandingOfficer(teamToCheck);
}

//====================================================================================
// BattleMapz
BattleMap::BattleMap(const String & name, const IntVector2 & dimensions)
	: Map(name, dimensions)
{
	m_movementHeatMap = new GameHeatMap(dimensions);
}

//-----------------------------------------------------------------------------------------------
BattleMap::BattleMap(const String& name, const Image& mapImage)
	: Map(name, mapImage)
{
	m_actionMenu = new Container("Main Menu", 5, Vector2(30.f, 30.f), AABB2(-10.f, 10.f));
	m_storeMenu = new Container("Purchase", 10, Vector2(25.f, 0.f), AABB2(-20.f, -40.f, 20.f, 40.f));
	m_hud = new HUD();

	m_movementHeatMap = new GameHeatMap(m_dimensions);
	m_attackHeatMap = new HeatMap(m_dimensions);

	m_battleScene = new BattleCutscene();
	
}

//-----------------------------------------------------------------------------------------------
void BattleMap::CreateTeams()
{
	for(uint i = 0; i < m_units.size(); i++)
	{
		TeamName unitsTeam = m_units.at(i)->m_team;
		m_turnOrder.CheckIfTeamIsRegisteredAndAdd(unitsTeam);
	}

	for(uint j = 0; j < m_buildings.size(); j++)
	{
		TeamName buildingsTeam = m_buildings.at(j)->m_team;

		if(buildingsTeam != TEAM_NONE)
			m_turnOrder.CheckIfTeamIsRegisteredAndAdd(buildingsTeam);
	}
}

BattleMap::~BattleMap()
{
	ClearHoverTiles(); //sanity check
	DeleteOfficers();

	delete m_movementHeatMap;
	m_movementHeatMap = nullptr;

	delete m_attackHeatMap;
	m_attackHeatMap = nullptr;

	delete m_actionMenu;
	m_actionMenu = nullptr;

	delete m_storeMenu;
	m_storeMenu = nullptr;

	delete m_hud;
	m_hud = nullptr;
}

//-----------------------------------------------------------------------------------------------
void BattleMap::DeleteOfficers()
{
	for(uint i = 0; i < m_officers.size(); i++)
	{
		CommandingOfficer* current = m_officers.at(i);

		delete current;
		current = nullptr;
	}

	m_officers.clear();
}

//-----------------------------------------------------------------------------------------------
void BattleMap::Update()
{
	UpdateCurrentCO();

	Map::Update(); // might be a bad idea

	UpdateUI();
	CheckForVictory();
}

void BattleMap::UpdateCurrentCO()
{
	if(m_currentOfficer != m_officers.at(m_turnOrder.m_current))
		m_currentOfficer = m_officers.at(m_turnOrder.m_current);
}

void BattleMap::UpdateUI()
{
	m_actionMenu->Update();
	m_storeMenu->Update();
}

void BattleMap::CreateCommandingOfficer(TeamName theTeam)
{
	//String playerName = "Player " + std::to_string(m_officers.size() + 1U);

	CommandingOfficer* newOfficer = new CommandingOfficer("Andy", theTeam, PLAYER_CO);

	m_officers.push_back(newOfficer);
}


bool BattleMap::CheckForAction(const IntVector2& mousePos)
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
			case ATTACK_TILE_TYPE:
				AttackUnitAt(m_attackTiles.at(i)->m_tileCoords);
				break;
			default:
				break;
			}

		}
	}


	return check;
}

void BattleMap::AttackUnitAt(const IntVector2& tileCoords)
{
	Tile* theTile = GetTile(tileCoords);

	Unit* target = theTile->m_unit;

	// store off the start health
	int aStartHealth = m_selectedUnit->m_health;
	int dStartHealth = target->m_health;

	// do the attack
	Unit::Attack(*m_selectedUnit, *target);

	// Create the battle result
	BattleResults results = BattleResults( *m_selectedUnit, *target, aStartHealth, dStartHealth );

	// give the battle scene the results
	m_battleScene->SetBattleResults(results);

	// clean up
	m_selectedUnit->m_usedAction = true;

	if(m_selectedUnit->m_isDead == true)
		m_selectedUnit->m_tileIAmOn->m_unit = nullptr;

	if(target->m_isDead == true)
		theTile->m_unit = nullptr;

}

void BattleMap::CreateMovementTiles(const Unit& theUnitToUse)
{
	m_movementHeatMap->ResetHeatMap();

	// Gotta translate from world coords to tileCoords
	IntVector2 worldCoords = GetTileCoords(theUnitToUse.m_transform.GetLocalPosition());
	IntVector2 tileCoords = IntVector2(worldCoords.x / TILE_SIZE_INT, worldCoords.y / TILE_SIZE_INT);

	m_movementHeatMap->AddHeat(tileCoords);

	std::vector<IntVector2> tilePos = m_movementHeatMap->GetAllTileCoordsWithHeatLessOrEqual(theUnitToUse.GetMovement());

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

void BattleMap::CreateActionTiles(const Unit& theUnitToUse)
{
	CreateAttackTiles(theUnitToUse);
}

void BattleMap::CreateAttackTiles(const Unit& theUnitToUse, bool showRange)
{
	m_attackHeatMap->ResetHeatMap();

	// Gotta translate from world coords to tileCoords
	IntVector2 worldCoords = GetTileCoords(theUnitToUse.m_transform.GetLocalPosition());
	IntVector2 tileCoords = IntVector2(worldCoords.x / TILE_SIZE_INT, worldCoords.y / TILE_SIZE_INT);

	m_attackHeatMap->AddHeat(tileCoords);

	std::vector<IntVector2> tilePos = m_attackHeatMap->GetAllTileCoordsWithHeatInRangeOf(theUnitToUse.GetAttackRange());

	for(uint i = 0; i < tilePos.size(); i++)
	{

		// see if we want to get the attack range of the unit, or just show what we can attack
		if(showRange == false)
		{
			if(CanPlayerAttackUnitOnTile(theUnitToUse, tilePos.at(i)))
			{
				HoverTile* newTile = new HoverTile(tilePos.at(i), ATTACK_TILE_TYPE);

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

void BattleMap::CreateStoreUI()
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
	m_currentContainer->AddCloseWidget();
}

bool BattleMap::CanUnitCaptureBuilding(const Unit& theUnitToUse)
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

bool BattleMap::CanPlayerMoveThere(IntVector2& posToCheck)
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

bool BattleMap::CanPlayerAttackUnitOnTile(const Unit& theUnitToUse, const IntVector2& posToCheck)
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

bool BattleMap::CanUnitEnterThatTile(const Unit& theUnitToUse, IntVector2& tileToCheck)
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

void BattleMap::ClearHoverTiles()
{
	for(uint i = 0; i < m_hoverTiles.size(); i++)
	{
		g_theGame->m_playingState->RemoveRenderable(m_hoverTiles.at(i)->m_renderable);

		delete m_hoverTiles.at(i);
	}

	m_hoverTiles.clear();
}

void BattleMap::ClearAttackTiles()
{
	for(uint i = 0; i < m_attackTiles.size(); i++)
	{
		g_theGame->m_playingState->RemoveRenderable(m_attackTiles.at(i)->m_renderable);

		delete m_attackTiles.at(i);
	}

	m_attackTiles.clear();
}

void BattleMap::RemoveDeadGameObjects()
{

	for(uint i = 0; i < m_gameObjects.size(); i++)
	{
		GameObject2D* current = m_gameObjects.at(i);

		if(current->m_isDead)
		{
			g_theGame->m_playingState->RemoveRenderable(m_gameObjects.at(i)->m_renderable);
			RemoveFast(i,m_gameObjects);
		}
	}

	// Units as well
	for(uint i = 0; i < m_units.size(); i++)
	{
		Unit* current = m_units.at(i);

		if(current->m_isDead)
		{
			RemoveFast(i, m_units);
		}
	}

}

void BattleMap::GoToNextTurn()
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

void BattleMap::GenerateIncome()
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

void BattleMap::CheckForVictory()
{
	// Later this could have other objectives based off the map (enum)

	bool victory = IsATeamWithoutUnits();

	if(victory)
		DebugRenderLog(10.f, "VICTORY");
}

bool BattleMap::IsATeamWithoutUnits()
{
	std::vector<TeamName> teams = m_turnOrder.m_order;

	for(uint teamNameIndex = 0; teamNameIndex < teams.size(); teamNameIndex++)
	{
		TeamName currentTeam = teams.at(teamNameIndex);
		bool foundOneAlive = false;

		for(uint unitIndex = 0; unitIndex < m_units.size(); unitIndex++)
		{
			Unit* currentUnit = m_units.at(unitIndex);

			if(currentUnit->m_team == currentTeam)
				foundOneAlive = true;
		}

		// they have no units 
		if(foundOneAlive == false)
			return true;
	}

	return false;
}