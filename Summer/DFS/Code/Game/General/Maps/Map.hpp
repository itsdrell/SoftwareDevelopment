#pragma once
#include "Engine\Renderer\RenderableComponents\Renderable2D.hpp"
#include "Game\General\Tiles\Tile.hpp"
#include "Engine\Renderer\Images\Image.hpp"
#include "Game\Main\GameCommon.hpp"
#include "Engine/Renderer/Systems/Scene2D.hpp"


//=============================================================
// Forward Declare
//=============================================================
class GameObject2D;
class Unit;
class Scene2D;

//====================================================================================
// Typedefs
//====================================================================================


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
	Map();
	Map(std::string name, const IntVector2& dimensions);
	Map(std::string name, const Image& mapImage);

	~Map();
	void DeleteUnits();
	void DeleteBuildings();
	

	virtual void Update();
	
	void CreateMapRenderable(bool makeDebug = false);
	void CreateMapRenderableFromImage();
	void RecreateMapRenderable();

	Tile* GetTile(const Vector2& worldPos);
	Tile* GetTile(const IntVector2& tilePos);
	IntVector2 GetTileCoords(const Vector2& worldPos) { return GetTile(worldPos)->m_position; }
	
	bool SelectUnit(Vector2 pos);
	void PlaceUnit(Vector2 pos);
	void PutSelectedUnitBack();
	
	//--------------------------------------------------------------------------
	void AddGameObject(GameObject2D& newObject) { m_gameObjects.push_back(&newObject) ;}
	void AddUnit(Unit& newUnit) { m_units.push_back(&newUnit); }
	void AddBuilding(Building& newBuilding) { m_buildings.push_back(&newBuilding); }
	
	// removes from gameobject list as well as others
	void DeleteGameObjectFromMap(GameObject2D* objectToRemove);
	void RemoveGameObject(int ID);
	void RemoveUnit(int ID);
	void RemoveBuilding(int ID);

	void AddRenderable(Renderable2D* newRenderable) { m_scene->AddRenderable(newRenderable); }
	void RemoveRenderable(Renderable2D* newRenderable) { m_scene->RemoveRenderable(newRenderable); }


	Unit* CreateUnit(std::string name, TeamName team, IntVector2 pos, int hp = 10);
	void CreateBuilding(const std::string& name, const TeamName& team, const IntVector2& pos);
	void CreateEffect(const String& name, const IntVector2& pos);
	
public:
	std::string							m_name;
	IntVector2							m_dimensions;
	Renderable2D*						m_mapRenderable = nullptr; // batched renderable
	Renderable2D*						m_debugRenderable = nullptr;
	Image								m_mapImage;

	Unit*								m_selectedUnit = nullptr;
	Building*							m_selectedBuilding = nullptr;

	Scene2D*							m_scene = nullptr;

	std::vector<Tile>					m_tiles;
	
	std::vector<GameObject2D*>			m_gameObjects;
	std::vector<Unit*>					m_units;
	std::vector<Building*>				m_buildings;
	

	
};


//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
