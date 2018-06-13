#pragma once
#include "Engine/Math/Vectors/IntVector2.hpp"
#include "Engine/Math/Vectors/Vector2.hpp"

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
class Tile
{
public:
	Tile();
	Tile(const IntVector2& pos, TileDefinition& def, int tileCellSize = 16);

	bool IsPointInsideTile( IntVector2& pos);

	Vector2 GetCenterOfTile() { return m_position.GetAsVector2(); }

	void ChangeTileType(TileDefinition& newDef) { m_definition = &newDef; }

public:
	IntVector2			m_position;
	TileDefinition*		m_definition;
	int					m_tileSize;
	
};

//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
