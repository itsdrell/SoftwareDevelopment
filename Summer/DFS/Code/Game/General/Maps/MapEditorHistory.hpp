#pragma once
#include <vector>

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
	UNDO_TYPE_GAMEOBJECT,
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
	virtual void Undo() = 0;
};

class UnitHistory : public HistoryItem
{
public:
	UnitHistory(Unit& unitToDestroy);
	virtual void Undo() override;
	
	Unit*		m_unitToDestroy;
};

class BuildingHistory : public HistoryItem
{
public:
	BuildingHistory(Building& buildingToDestroy);
	virtual void Undo() override;
	
	Building*	m_buildingToDestroy;
};
//
//class DeleteHistory : public HistoryItem
//{
//public:
//	virtual void Undo() override;
//	
//	UndoType			m_type;
//	GameObject2D*		m_gameObjectToMake;
//};
//
class TileHistory : public HistoryItem
{
public:
	TileHistory(TileDefinition* previousDef, Tile& theTile);
	virtual void Undo() override;
	
	TileDefinition*		m_previousDefinition;
	Tile*				m_theTile;
};

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