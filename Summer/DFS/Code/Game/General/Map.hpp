#pragma once
#include "..\SystemsAndTools\Renderable2D.hpp"
#include "Tiles\Tile.hpp"


//=============================================================
// Forward Declare
//=============================================================
class GameObject2D;

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
	
	
public:
	std::string				m_name;
	IntVector2				m_dimensions;
	Renderable2D*			m_mapRenderable; // batched renderable
	Renderable2D*			m_debugRenderable;

	std::vector<Tile>					m_tiles;
	std::vector<GameObject2D*>			m_gameObjects;
};


//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
