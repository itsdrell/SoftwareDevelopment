#pragma once
#include "..\SystemsAndTools\Renderable2D.hpp"
#include "Tiles\Tile.hpp"
#include "Engine\Renderer\Images\Image.hpp"
#include "..\Main\GameCommon.hpp"


//=============================================================
// Forward Declare
//=============================================================
class GameObject2D;
class Unit;
class HeatMap;
class HoverTile;
class GameHeatMap;
class CommandingOfficer;
class Container;
class HUD;

//====================================================================================
// Typedefs
//====================================================================================


//=============================================================
// ENUMS
//=============================================================


//=============================================================
// Structs
//=============================================================
struct TurnOrder
{
	TurnOrder()
	: m_current (0U) {}

	void GoToNextTurn();
	void CheckIfTeamIsRegisteredAndAdd(TeamName teamToCheck);
	
	void AddTeam(TeamName team) { m_order.push_back(team); }
	TeamName GetCurrentTeamTurn() const { return m_order.at(m_current); }
	std::string GetCurrentTurnString() const { return TeamNameToString(m_order.at(m_current)); }

	//--------------------------------------------------------------------------
	uint						m_current = 0U;
	uint						m_turnCount = 1U;
	std::vector<TeamName>		m_order;
};

//=============================================================
// Classes
//=============================================================
class Map
{
public:
	Map(std::string name, const IntVector2& dimensions);
	Map(std::string name, Image& mapImage);

	void Update();
	void UpdateCurrentCO();
	void UpdateUI();

	void CreateMapRenderable(bool makeDebug = false);
	void CreateMapRenderableFromImage();

	void CreateCommandingOfficer(TeamName theTeam); // v1 for now

	Tile* GetTile(const Vector2& worldPos);
	Tile* GetTile(const IntVector2& tilePos);
	IntVector2 GetTileCoords(const Vector2& worldPos) { return GetTile(worldPos)->m_position; }
	
	bool SelectUnit(Vector2 pos);
	void PlaceUnit(Vector2 pos);
	void PutSelectedUnitBack();
	bool CheckForAction(const IntVector2& mousePos);
	void AttackUnitAt(const IntVector2& tileCoords);

	void CreateMovementTiles(const Unit& theUnitToUse);
	void CreateActionTiles(const Unit& theUnitToUse);
	void CreateAttackTiles(const Unit& theUnitToUse, bool showRange = false);

	void CreateStoreUI();

	bool CanUnitCaptureBuilding(const Unit& theUnitToUse);
	bool CanPlayerMoveThere(IntVector2& posToCheck);
	bool CanPlayerAttackUnitOnTile(const Unit& theUnitToUse, const IntVector2& posToCheck);
	bool CanUnitEnterThatTile(const Unit& theUnitToUse, IntVector2& tileToCheck);

	void ClearHoverTiles();
	void ClearAttackTiles();
	void RemoveDeadGameObjects();

	void GoToNextTurn();
	void GenerateIncome();

	void CheckForVictory();
	bool IsATeamWithoutUnits();

	//--------------------------------------------------------------------------
	void AddGameObject(GameObject2D& newObject) { m_gameObjects.push_back(&newObject) ;}
	void AddUnit(Unit& newUnit) { m_units.push_back(&newUnit); }
	void AddBuilding(Building& newBuilding) { m_buildings.push_back(&newBuilding); }
	
	Unit* CreateUnit(std::string name, TeamName team, IntVector2 pos, int hp = 10);
	void CreateBuilding(const std::string& name, const TeamName& team, const IntVector2& pos);
	
public:
	std::string							m_name;
	IntVector2							m_dimensions;
	Renderable2D*						m_mapRenderable = nullptr; // batched renderable
	Renderable2D*						m_debugRenderable = nullptr;
	Image								m_mapImage;

	std::vector<Tile>					m_tiles;
	std::vector<HoverTile*>				m_hoverTiles;
	std::vector<HoverTile*>				m_attackTiles;
	std::vector<GameObject2D*>			m_gameObjects;
	std::vector<Unit*>					m_units;
	std::vector<Building*>				m_buildings;
	std::vector<CommandingOfficer*>		m_officers;

	TurnOrder							m_turnOrder;
	GameHeatMap*						m_movementHeatMap = nullptr;
	HeatMap*							m_attackHeatMap = nullptr;
	
	CommandingOfficer*					m_currentOfficer = nullptr;
	Unit*								m_selectedUnit = nullptr;
	Building*							m_selectedBuilding = nullptr;
	Building*							m_buildingToCapture = nullptr; // for console command 

	Container*							m_currentContainer = nullptr;
	Container*							m_actionMenu = nullptr;
	Container*							m_storeMenu = nullptr;
	HUD*								m_hud = nullptr;
};


//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
extern Map*	g_theCurrentMap;