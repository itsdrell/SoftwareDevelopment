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
class UIWidget;

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
	SELECTIONTYPE_DELETE,
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
	void CreateNewMap();
	void AddWidgets();
	void GetDefinitions();

	~MapEditor();
	void Exit();
	void DeleteWidgets();

	void Update();
	void SwapTeamColors();

	void Render() const;
	void RenderUI() const;
	void RenderSelectionBar() const;
	void RenderCurrentPaintMethod() const;
	void RenderBackgroundUI() const;
	void RenderCurrentSelectionBar() const;

	void CheckKeyboardInputs();
	void PlaceObjectOrTile();
	void SwapPlacementMode();
	void SwapTypeOfObject();
	void ChangeTheUnitToPlace(int direction);
	void ChangeTheTileToPlace(int direction);
	void ChangeTheBuildingToPlace(int direction);
	void MoveCursor();
	void MoveCamera();

	void ChangeTile();
	void PlaceUnit();
	void PlaceBuilding();
	void DeleteUnitOrBuilding();
	void RemoveUnit();
	void RemoveBuilding();

	Tile* GetSelectedTile();
	String GetCurrentSelectionText() const;

	void AddRenderable(Renderable2D* newRenderable) { m_currentMap->AddRenderable(newRenderable); }
	void RemoveRenderable(Renderable2D* toRemove) { m_currentMap->RemoveRenderable(toRemove); }

public:
	
	SpriteRendering*					m_renderingPath = nullptr;

	Camera*								m_camera = nullptr;

	Map*								m_currentMap = nullptr;

	Cursor*								m_cursor = nullptr;
	Vector2								m_cameraLocation;

	SelectionType						m_selectionType = SELECTIONTYPE_TILE;

	Tile*								m_selectedTileToChange = nullptr; // convienence pointer

	TeamName							m_currentUnitTeam = TEAM_RED;
	TeamName							m_currentBuildingTeam = TEAM_NONE;
	TileDefinition*						m_currentTileDefinition = nullptr;
	int									m_tileDefinitionIndex = 0;
	UnitDefinition*						m_currentUnitDefinition = nullptr;
	int									m_unitDefinitionIndex = 0;
	BuildingDefinition*					m_currentBuildingDefinition = nullptr;
	int									m_buildingDefinitionIndex = 0;

private:
	AABB2 m_deleteBounds;
	AABB2 m_tileBounds;	
	AABB2 m_unitBounds;	
	AABB2 m_buildingBounds;

	std::vector<UIWidget*> m_widgets;
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