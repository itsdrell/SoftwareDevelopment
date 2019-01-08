#pragma once
#include "Game/General/Maps/Map.hpp"

//====================================================================================
// Forward Declare
//====================================================================================
class GameObject2D;
class Unit;
class HeatMap;
class HoverTile;
class GameHeatMap;
class CommandingOfficer;
class Container;
class HUD;
class BattleCutscene;

//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================
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

//====================================================================================
// Classes
//====================================================================================
class BattleMap : public Map
{
public:
	BattleMap(const String& name, const IntVector2& dimensions);
	BattleMap(const String& name, const Image& mapImage);
	void CreateTeams();

	~BattleMap();
	void DeleteOfficers();

	virtual void Update() override;
	void UpdateCurrentCO(); 
	void UpdateUI(); 

	void CreateCommandingOfficer(TeamName theTeam); // v1 for now

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

public:
	std::vector<HoverTile*>				m_hoverTiles;
	std::vector<HoverTile*>				m_attackTiles;
	std::vector<CommandingOfficer*>		m_officers;

	TurnOrder							m_turnOrder;
	GameHeatMap*						m_movementHeatMap = nullptr;
	HeatMap*							m_attackHeatMap = nullptr;

	CommandingOfficer*					m_currentOfficer = nullptr;
	Building*							m_buildingToCapture = nullptr; // for console command 

	Container*							m_currentContainer = nullptr;
	Container*							m_actionMenu = nullptr;
	Container*							m_storeMenu = nullptr;
	HUD*								m_hud = nullptr;

	BattleCutscene*						m_battleScene = nullptr;
};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================
extern BattleMap* g_theBattleMap;

//====================================================================================
// Written by Zachary Bracken : [1/8/2019]
//====================================================================================