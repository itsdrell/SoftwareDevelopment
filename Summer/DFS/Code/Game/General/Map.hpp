#pragma once
#include "..\SystemsAndTools\Renderable2D.hpp"
#include "Tiles\Tile.hpp"
#include "Engine\Renderer\Images\Image.hpp"


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
	Map(std::string name, Image& mapImage);

	void CreateMapRenderable(bool makeDebug = false);
	void CreateMapRenderableFromImage();

	Tile* GetTile(Vector2& worldPos);
	Tile* GetTile(IntVector2& tilePos);
	IntVector2 GetTileCoords(Vector2& worldPos) { return GetTile(worldPos)->m_position; }
	
	void CreateMovementTiles(const Unit& theUnitToUse);
	bool CanPlayerMoveThere(IntVector2& posToCheck);
	bool CanUnitEnterThatTile(const Unit& theUnitToUse, IntVector2& tileToCheck);

	void ClearHoverTiles();
	void RemoveDeadGameObjects();

	//--------------------------------------------------------------------------
	void AddGameObject(GameObject2D& newObject) { m_gameObjects.push_back(&newObject) ;}
	
	
public:
	std::string							m_name;
	IntVector2							m_dimensions;
	Renderable2D*						m_mapRenderable = nullptr; // batched renderable
	Renderable2D*						m_debugRenderable = nullptr;
	Image								m_mapImage;

	std::vector<Tile>					m_tiles;
	std::vector<HoverTile*>				m_hoverTiles;
	std::vector<GameObject2D*>			m_gameObjects;


	HeatMap*							m_heatmap;

	Unit*								m_selectedUnit;
};


//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
