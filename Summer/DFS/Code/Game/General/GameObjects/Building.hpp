#pragma once
#include "Game\SystemsAndTools\GameObject2D.hpp"
#include "Game\Main\GameCommon.hpp"
#include "Engine\Core\General\EngineCommon.hpp"


//=============================================================
// Forward Declare
//=============================================================
class TileDefinition;

//=============================================================
// ENUMS
//=============================================================


//=============================================================
// Structs
//=============================================================


//=============================================================
// Classes
//=============================================================
class BuildingDefinition
{

public:
	BuildingDefinition(tinyxml2::XMLElement& node);

	static BuildingDefinition* GetDefinition(std::string name);
	static Strings GetAllBuildingNames();

	static std::map<std::string, BuildingDefinition*>	s_definitions;

public:
	std::string			m_name;

	bool				m_isFactory;
	bool				m_isHQ;

	// this is for naval bases since ground units and naval can go to it
	// it will need a special water tile (shore)
	TileDefinition*		m_tileToSpawnBeneath; 

	IntVector2			m_spriteCoords;
	AABB2				m_uvCoords;
};


class Building : public GameObject2D
{
public:
	Building(TeamName theTeam = TEAM_NONE);
	Building(const TeamName& theTeam, BuildingDefinition& def);
	void SetSpriteFromTeam(const TeamName& theTeam);


	void Update();
	void Captured(const TeamName& theTeam);

public:
	TeamName				m_team;

	uint					m_health;

	BuildingDefinition*		m_definition;
};

//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
