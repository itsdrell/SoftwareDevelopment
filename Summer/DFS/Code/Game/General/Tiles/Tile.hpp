#pragma once
#include "Engine/Math/Vectors/IntVector2.hpp"
#include "Engine/Math/Vectors/Vector2.hpp"
#include "Engine/Math/Geometry/AABB2.hpp"
#include "Game/SystemsAndTools/GameObject2D.hpp"
#include "Game/General/Tiles/TileDefinition.hpp"

//=============================================================
// Forward Declare
//=============================================================
class TileDefinition;
class Unit;
class Building;

//=============================================================
// ENUMS
//=============================================================


//=============================================================
// Structs
//=============================================================
enum HoverTileTypes
{
	MOVEMENT_TILE_TYPE,
	ATTACK_TILE_TYPE,
	ATTACK_RANGE_TILE_TYPE,
	NUM_OF_HOVER_TILE_TYPES
};

//=============================================================
// Classes
//=============================================================
class Tile
{
public:
	Tile();
	Tile(const IntVector2& pos, TileDefinition& def, int tileCellSize = 16);

	bool IsPointInsideTile( const IntVector2& pos);

	Vector2 GetCenterOfTile() { return m_position.GetAsVector2(); }
	AABB2 GetTileBounds();

	String GetDisplayName() { return m_definition->m_displayName; }

	void ChangeTileType(TileDefinition& newDef) { m_definition = &newDef; }

public:
	
	IntVector2			m_position;
	TileDefinition*		m_definition;
	int					m_tileSize;

	// Convenience stuff
	Unit*				m_unit;
	Building*			m_building;
	
};

//====================================================================================
class HoverTile : public GameObject2D
{
public:
	HoverTile(IntVector2& position, HoverTileTypes theType = MOVEMENT_TILE_TYPE);
	~HoverTile();

	void CreateMovementTile();
	void CreateAttackTile();
	void CreateAttackRangeTile();

public:
	HoverTileTypes		m_type;
	IntVector2			m_tileCoords;
};

//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
