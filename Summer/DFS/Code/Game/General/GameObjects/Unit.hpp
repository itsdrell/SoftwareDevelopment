#pragma once
#include "Game\SystemsAndTools\GameObject2D.hpp"
#include "Game\SystemsAndTools\Tags.hpp"
#include "Game\Main\GameCommon.hpp"
#include "Engine\Math\Ranges\IntRange.hpp"
#include <string>
#include "Engine\Math\Geometry\AABB2.hpp"
#include "Engine\Math\Vectors\IntVector2.hpp"


//=============================================================
// Forward Declare
//=============================================================
class Tile;

//=============================================================
// ENUMS
//=============================================================


//=============================================================
// Structs
//=============================================================


//=============================================================
// Classes
//=============================================================
class UnitDefinition
{
public:
	UnitDefinition(tinyxml2::XMLElement& node);


	static UnitDefinition* GetUnitDefinition(std::string name);
	static void GetAllUnitDefinitionsWithStoreTag(String tag, std::vector<UnitDefinition*>* list);
	static Strings GetAllUnitNames();
	static void DeleteAllDefinitions();

	static std::map<std::string, UnitDefinition*>	s_definitions;

	float GetMovementCost(const String& tileName);

public:
	std::string			m_name;
	String				m_displayName;

	uint				m_cost;
	String				m_factoryTag;
	
	bool				m_canCapture;
	int					m_movement;

	IntVector2			m_spriteCoords;
	AABB2				m_uvCoords;
	
	Tags				m_movementTags;
	Tags				m_attackTags;

	IntRange			m_attackRange;

	std::map<String, float>		m_movementCosts;
};

//--------------------------------------------------------------------------
class Unit : public GameObject2D
{
public:
	Unit(TeamName team);
	Unit(TeamName team, UnitDefinition& def);
	
	void Update();

	static SpriteSheet GetTeamTexture(TeamName name);

	float GetCostForTileType(const String& tileType);
	IntRange GetAttackRange() const { return m_definition->m_attackRange;}
	Tags GetMovementTags() const { return m_definition->m_movementTags; }
	Tags GetAttackTags() const { return m_definition->m_attackTags; }
	std::string GetName() const { return m_definition->m_name; }
	String GetDisplayName() const { return m_definition->m_displayName; }
	float GetMovement() const { return (float) m_definition->m_movement; }

public:
	UnitDefinition*		m_definition;

	TeamName			m_team;
	uint				m_health;

	bool				m_beenMoved;
	bool				m_usedAction;

	Tile*				m_tileIAmOn;
};

//=============================================================
// Standalone C Functions
//=============================================================
float CalculateWinChance(const Unit& attacking, const Unit& defending);

//=============================================================
// Externs
//=============================================================
