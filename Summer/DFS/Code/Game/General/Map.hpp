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
class HoverTile;
class HeatMap;

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
	std::vector<TeamName>		m_order;
	uint						m_current;
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

	void CreateMapRenderable(bool makeDebug = false);
	void CreateMapRenderableFromImage();

	Tile* GetTile(Vector2& worldPos);
	Tile* GetTile(const IntVector2& tilePos);
	IntVector2 GetTileCoords(Vector2& worldPos) { return GetTile(worldPos)->m_position; }
	
	bool SelectUnit(Vector2 pos);
	void PlaceUnit(Vector2 pos);
	void PutSelectedUnitBack();
	bool CheckForAction(const IntVector2& mousePos);
	void AttackUnitAt(const IntVector2& tileCoords);

	void CreateMovementTiles(const Unit& theUnitToUse);
	void CreateActionTiles(const Unit& theUnitToUse);
	void CreateAttackTiles(const Unit& theUnitToUse);

	bool CanPlayerMoveThere(IntVector2& posToCheck);
	bool CanPlayerAttackUnitOnTile(const Unit& theUnitToUse, const IntVector2& posToCheck);
	bool CanUnitEnterThatTile(const Unit& theUnitToUse, IntVector2& tileToCheck);

	void ClearHoverTiles();
	void RemoveDeadGameObjects();

	void GoToNextTurn();

	void CheckForVictory();
	bool IsATeamWithoutUnits();

	//--------------------------------------------------------------------------
	void AddGameObject(GameObject2D& newObject) { m_gameObjects.push_back(&newObject) ;}
	void AddUnit(Unit& newUnit) { m_units.push_back(&newUnit); }
	void CreateUnit(std::string name, TeamName team, IntVector2 pos);
	
public:
	std::string							m_name;
	IntVector2							m_dimensions;
	Renderable2D*						m_mapRenderable = nullptr; // batched renderable
	Renderable2D*						m_debugRenderable = nullptr;
	Image								m_mapImage;

	std::vector<Tile>					m_tiles;
	std::vector<HoverTile*>				m_hoverTiles;
	std::vector<GameObject2D*>			m_gameObjects;
	std::vector<Unit*>					m_units;

	TurnOrder							m_turnOrder;
	HeatMap*							m_heatmap;
	Unit*								m_selectedUnit;
};


//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
extern Map*	g_theCurrentMap;