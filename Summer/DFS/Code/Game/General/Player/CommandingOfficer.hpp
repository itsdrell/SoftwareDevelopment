#pragma once
#include "Engine\Core\General\EngineCommon.hpp"
#include "Game\General\CommandingOfficerStats.hpp"
#include "Game\General\COPower.hpp"
#include "Game\Main\GameCommon.hpp"

//====================================================================================
// Forward Declare
//====================================================================================
class Controller;

//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================
enum CO_TYPES
{
	PLAYER_CO,
	AI_CO
};

//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class CommandingOfficerDefinition
{
public:
	CommandingOfficerDefinition(tinyxml2::XMLElement& node);
	~CommandingOfficerDefinition();

	static CommandingOfficerDefinition* GetDefinition(const String& name);
	static void DeleteAllDefinitions();

public:
	String							m_name;

	COPower*						m_power;
	CommandingOfficerStat*			m_stats;

	static std::map<String, CommandingOfficerDefinition*>		s_definitions;
};


//-----------------------------------------------------------------------------------------------
class CommandingOfficer
{
public:
	CommandingOfficer(const String nameOfDefinition, const TeamName& team, CO_TYPES typeOfController = AI_CO);
	CommandingOfficer( CommandingOfficerDefinition& definition, const TeamName& team, CO_TYPES typeOfController = AI_CO);

	~CommandingOfficer();

public:
	String								m_name;
	TeamName							m_team;
	uint								m_money;

	CommandingOfficerDefinition*		m_definition = nullptr;
	Controller*							m_controller = nullptr;
};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [7/14/2018]
//====================================================================================