#pragma once
#include "..\SystemsAndTools\Renderable2D.hpp"
#include "Tiles\Tile.hpp"


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
constexpr float TILE_SIZE = 16.f;
constexpr int TILE_SIZE_INT = (int) TILE_SIZE;

//=============================================================
// ENUMS
//=============================================================


//=============================================================
// Structs
//=============================================================


//=============================================================
// Classes
//=============================================================
class Map
{
public:
	Map(std::string name, const IntVector2& dimensions);

	void CreateMapRenderable(bool makeDebug = false);

	Tile* GetTile(Vector2& worldPos);
	IntVector2 GetTileCoords(Vector2& worldPos) { return GetTile(worldPos)->m_position; }
	
	void CreateMovementTiles(const Unit& theUnitToUse);
	bool CanPlayerMoveThere(IntVector2& posToCheck);

	void ClearHoverTiles();
	
	
public:
	std::string							m_name;
	IntVector2							m_dimensions;
	Renderable2D*						m_mapRenderable = nullptr; // batched renderable
	Renderable2D*						m_debugRenderable = nullptr;

	std::vector<Tile>					m_tiles;
	std::vector<HoverTile*>				m_hoverTiles;
	std::vector<GameObject2D*>			m_gameObjects;


	HeatMap*							m_heatmap;

	Unit*	m_selectedUnit;
};


//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
