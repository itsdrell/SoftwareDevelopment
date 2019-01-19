#pragma once
#include <vector>
#include "Game\Main\GameCommon.hpp"

//====================================================================================
// Forward Declare
//====================================================================================
class Unit;
class Tile;
class Building;
class GameObject2D;
class TileDefinition;

//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================
enum UndoType
{
	UNDO_TYPE_UNIT,
	UNDO_TYPE_BUILDING,
	UNDO_TYPE_TILE
};

//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class HistoryItem
{
public:
	HistoryItem() {}
	HistoryItem(const IntVector2& tilePos) { m_position = tilePos; }
	virtual void Undo() = 0;

	// the reason we use position instead of gameobjects is because
	// when undoing a delete the gameobject is destroyed leaving 
	// behind a dangling pointer
	IntVector2	m_position;
};

//-----------------------------------------------------------------------------------------------
class UnitHistory : public HistoryItem
{
public:
	UnitHistory(const IntVector2& tilePos);
	virtual void Undo() override;
};

//-----------------------------------------------------------------------------------------------
class BuildingHistory : public HistoryItem
{
public:
	BuildingHistory(const IntVector2& tilePos);
	virtual void Undo() override;
};

//-----------------------------------------------------------------------------------------------
class DeleteHistory : public HistoryItem
{
public:
	DeleteHistory( UndoType theType, TeamName theTeam, const String& definitionName, const IntVector2& tilePos );
	virtual void Undo() override;
	
	UndoType			m_type;
	TeamName			m_team;
	String				m_definitionName;
};

//-----------------------------------------------------------------------------------------------
class TileHistory : public HistoryItem
{
public:
	TileHistory(TileDefinition* previousDef, Tile& theTile);
	virtual void Undo() override;
	
	TileDefinition*		m_previousDefinition;
	Tile*				m_theTile;
};

//-----------------------------------------------------------------------------------------------
class History
{
public:
	std::vector<HistoryItem*>		m_history;
};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [1/18/2019]
//====================================================================================