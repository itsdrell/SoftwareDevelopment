#pragma once
#include "Engine/Math/Vectors/Vector2.hpp"
#include "Game/General/Maps/Map.hpp"
#include "../Main/GameCommon.hpp"

//====================================================================================
// Forward Declare
//====================================================================================
class SpriteRendering;
class Scene2D;
class Camera;
class Cursor;
class Renderable2D;
class Tile;
class UnitDefinition;
class BuildingDefinition;
class TileDefinition;

//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================
enum SelectionType
{
	SELECTIONTYPE_TILE,
	SELECTIONTYPE_UNIT,
	SELECTIONTYPE_BUILDING,
	NUM_OF_SELECTION_TYPES
};

//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class MapEditor
{
public:
	MapEditor();
	void StartUp();

	~MapEditor();

	void Update();
	void SwapTeamColors();


	void Render() const;
	void RenderUI() const;
	void RenderSelectionBar() const;
	void RenderCurrentPaintMethod() const;

	void CheckKeyboardInputs();
	void PlaceObjectOrTile();
	void SwapPlacementMode();
	void SwapTypeOfObject();
	void MoveCursor();
	void MoveCamera();

	void ChangeTile();
	void PlaceUnit();
	void PlaceBuilding();

	Tile* GetSelectedTile();

	void AddRenderable(Renderable2D* newRenderable) { m_currentMap->AddRenderable(newRenderable); }
	void RemoveRenderable(Renderable2D* toRemove) { m_currentMap->RemoveRenderable(toRemove); }

public:
	
	SpriteRendering*		m_renderingPath = nullptr;

	Camera*					m_camera = nullptr;

	Map*					m_currentMap = nullptr;

	Cursor*					m_cursor = nullptr;
	Vector2					m_cameraLocation;

	SelectionType			m_selectionType = SELECTIONTYPE_TILE;

	Tile*					m_selectedTileToChange = nullptr; // convienence pointer

	TeamName				m_currentTeam = TEAM_RED;
	TileDefinition*			m_currentTileDefinition = nullptr;
	UnitDefinition*			m_currentUnitDefinition = nullptr;
	BuildingDefinition*		m_currentBuildingDefinition = nullptr;

private:
	AABB2 m_teamColorBounds;
	AABB2 m_tileBounds;	
	AABB2 m_unitBounds;	
	AABB2 m_buildingBounds;

};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [1/8/2019]
//====================================================================================