#pragma once
#include "Engine/Math/Vectors/IntVector2.hpp"
#include "Engine/Math/Vectors/Vector2.hpp"
#include "Engine/Math/Geometry/AABB2.hpp"
#include "Game/SystemsAndTools/GameObject2D.hpp"

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
enum HoverTileTypes
{
	MOVEMENT_TILE_TYPE,
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

	bool IsPointInsideTile( IntVector2& pos);

	Vector2 GetCenterOfTile() { return m_position.GetAsVector2(); }
	AABB2 GetTileBounds();

	void ChangeTileType(TileDefinition& newDef) { m_definition = &newDef; }

public:
	IntVector2			m_position;
	TileDefinition*		m_definition;
	int					m_tileSize;
	
};

//--------------------------------------------------------------------------
class HoverTile : public GameObject2D
{
public:
	HoverTile(HoverTileTypes theType = MOVEMENT_TILE_TYPE);
	~HoverTile();

public:
	HoverTileTypes		m_type;
};

//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
