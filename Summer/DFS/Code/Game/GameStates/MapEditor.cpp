#include "MapEditor.hpp"
#include "..\General\Maps\Map.hpp"
#include "Engine\Math\Vectors\IntVector2.hpp"
#include "Engine\Core\General\Camera.hpp"
#include "..\General\GameObjects\Cursor.hpp"
#include "Engine\Renderer\Systems\Scene2D.hpp"
#include "Engine\Renderer\Systems\SpriteRendering.hpp"
#include "Engine\Core\Platform\Window.hpp"
#include "Engine\Input\Mouse.hpp"
#include "Engine\Core\Tools\DevConsole.hpp"
#include "Game\General\Tiles\Tile.hpp"
#include "Game\General\Tiles\TileDefinition.hpp"
#include "..\General\GameObjects\Unit.hpp"
#include "Game\General\GameObjects\Building.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteSheet.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteAnimator.hpp"
#include "Engine\Renderer\Images\Sprites\Sprite.hpp"

//===============================================================================================
MapEditor::MapEditor()
{
	Renderer* r = Renderer::GetInstance();
	m_tileBounds =		GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2(.1f, .7f), Vector2(.2f, .8f));
	m_unitBounds =		GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2(.1f, .5f), Vector2(.2f, .6f));
	m_buildingBounds =	GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2(.1f, .3f), Vector2(.2f, .4f));
	m_deleteBounds =	GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2(.1f, .1f), Vector2(.2f, .2f));
}

//-----------------------------------------------------------------------------------------------
void MapEditor::StartUp()
{
	//m_scene = new Scene2D("Test");
	m_renderingPath = new SpriteRendering();
	
	m_currentMap = new Map("new map:)", IntVector2(20,16));

	//---------------------------------------------------------
	// Cameras
	m_camera = new Camera();
	m_camera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_camera->SetDepthStencilTarget(g_theRenderer->m_defaultDepthTarget);

	m_currentMap->m_scene->AddCamera(m_camera);

	g_theRenderer->SetCamera();
	
	m_cursor = new Cursor();
	m_cameraLocation = Vector2(-112,-112);

	GetDefinitions();
}

//-----------------------------------------------------------------------------------------------
void MapEditor::GetDefinitions()
{
	// By doing it in order we let the data show what units are more common/ do grouping etc
	m_currentTileDefinition =		TileDefinition::s_definitionsInOrderOfLoad.at(0);
	m_currentUnitDefinition =		UnitDefinition::s_definitionsInOrderOfLoad.at(0);
	m_currentBuildingDefinition =	BuildingDefinition::s_definitionsInOrderOfLoad.at(0);
}

//-----------------------------------------------------------------------------------------------
MapEditor::~MapEditor()
{

	delete m_renderingPath;
	m_renderingPath = nullptr;

	// scene deletes camera
	m_camera = nullptr;

	delete m_cursor;
	m_cursor = nullptr;

	delete m_currentMap;
	m_currentMap = nullptr;
}

//-----------------------------------------------------------------------------------------------
void MapEditor::Update()
{
	CheckKeyboardInputs();
}

//-----------------------------------------------------------------------------------------------
void MapEditor::SwapTeamColors()
{
	if(WasKeyJustPressed(KEYBOARD_TAB))
	{
		if(m_currentTeam == TEAM_RED)
			m_currentTeam = TEAM_BLUE;
		else
			m_currentTeam = TEAM_RED;
	}
}

//-----------------------------------------------------------------------------------------------
void MapEditor::Render() const
{
	Renderer::GetInstance()->DrawAABB2(AABB2(-1000,1000), Rgba::BLACK);

	m_camera->SetProjectionOrthoByAspect(Window::GetInstance()->GetHeight() * .5f); // .5f makes it bigger
	Vector3 cursorPos = m_camera->ScreenToWorldCoordinate(GetMouseCurrentPosition(), 0.f);
	m_camera->m_viewMatrix = Matrix44::LookAt(Vector3(m_cameraLocation.x, m_cameraLocation.y, -10.f), 
		Vector3(m_cameraLocation.x, m_cameraLocation.y, .5f));

	m_renderingPath->Render(m_currentMap->m_scene);

	RenderUI();
}

//-----------------------------------------------------------------------------------------------
void MapEditor::RenderUI() const
{
	RenderSelectionBar();
	RenderCurrentPaintMethod();
}

//-----------------------------------------------------------------------------------------------
void MapEditor::RenderSelectionBar() const
{
	Renderer* r = Renderer::GetInstance();
	r->SetCurrentTexture();

	// team color
	//r->DrawAABB2(m_teamColorBounds, GetColorFromTeamName(m_currentTeam));

	// tiles
	Texture* tileTexture = g_tileSpriteSheet.m_spriteSheetTexture;
	r->SetCurrentTexture(0, tileTexture);
	r->DrawTexturedAABB2(m_tileBounds, *tileTexture, m_currentTileDefinition->m_uvCoords.mins, m_currentTileDefinition->m_uvCoords.maxs, Rgba::WHITE);

	// units
	SpriteAnimator theAnimator = SpriteAnimator(Unit::GetAnimatorName(m_currentUnitDefinition->m_name,m_currentTeam));
	Sprite* currentUnitSprite = theAnimator.GetCurrentSprite();
	r->SetCurrentTexture(0, currentUnitSprite->m_image);
	r->DrawTexturedAABB2(m_unitBounds, *currentUnitSprite->m_image, currentUnitSprite->m_uv.mins, currentUnitSprite->m_uv.maxs, Rgba::WHITE);

	// buildings
	Texture* buildingTexture = g_buildingSpriteSheet.m_spriteSheetTexture;
	AABB2 newSpriteCoords = g_buildingSpriteSheet.GetTexCoordsForSpriteCoords(IntVector2(m_currentBuildingDefinition->m_spriteCoords.x, m_currentTeam));
	r->SetCurrentTexture(0, buildingTexture);
	r->DrawTexturedAABB2(m_buildingBounds, *buildingTexture, newSpriteCoords.mins, newSpriteCoords.maxs, Rgba::WHITE);

	// Draw delete
	Texture* trashCan = r->CreateOrGetTexture("Data/Images/Sprites/trashcan.png");
	r->SetCurrentTexture(0, trashCan);
	r->DrawTexturedAABB2(m_deleteBounds, *trashCan, Vector2::ZERO, Vector2::ONE, Rgba::WHITE);
	
	r->SetCurrentTexture();
}

//-----------------------------------------------------------------------------------------------
void MapEditor::RenderCurrentPaintMethod() const
{
	Renderer* r = Renderer::GetInstance();
	AABB2 space;

	switch (m_selectionType)
	{
	case SELECTIONTYPE_TILE:
		space = GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2(.04f,.74f), Vector2(.06f, .76f));
		break;
	case SELECTIONTYPE_BUILDING:
		space = GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2(.04f,.34f), Vector2(.06f, .36f));
		break;
	case SELECTIONTYPE_UNIT:
		space = GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2(.04f,.54f), Vector2(.06f, .56f));
		break;
	case SELECTIONTYPE_DELETE:
		space = GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2(.04f,.14f), Vector2(.06f, .16f));
		break;
	default:
		break;
	}

	r->DrawAABB2(space, Rgba::YELLOW);
}

//-----------------------------------------------------------------------------------------------
void MapEditor::CheckKeyboardInputs()
{
	if(IsDevConsoleOpen())
		return;

	SwapTeamColors();
	SwapPlacementMode();
	SwapTypeOfObject();
	PlaceObjectOrTile();
	
	
	MoveCursor();
}

//-----------------------------------------------------------------------------------------------
void MapEditor::PlaceObjectOrTile()
{
	// when the user hits mouse button, do a thing based on mode
	if(IsMouseButtonPressed(LEFT_MOUSE_BUTTON))
	{
		m_selectedTileToChange = GetSelectedTile();

		// this check is to enable dragging/painting tiles
		if(m_selectedTileToChange == nullptr)
			return;
		
		switch (m_selectionType)
		{
		case SELECTIONTYPE_TILE:
			ChangeTile();
			break;
		case SELECTIONTYPE_BUILDING:
			PlaceBuilding();
			break;
		case SELECTIONTYPE_UNIT:
			PlaceUnit();
			break;
		case SELECTIONTYPE_DELETE:
			DeleteUnitOrBuilding();
			break;
		default:
			break;
		}
	}
	
}

//-----------------------------------------------------------------------------------------------
void MapEditor::SwapPlacementMode()
{
	if(WasKeyJustPressed(G_THE_LETTER_W) || WasKeyJustPressed(KEYBOARD_UP_ARROW))
	{
		m_selectionType = (SelectionType)((m_selectionType + NUM_OF_SELECTION_TYPES - 1) % NUM_OF_SELECTION_TYPES);
	}

	if(WasKeyJustPressed(G_THE_LETTER_S) || WasKeyJustPressed(KEYBOARD_DOWN_ARROW))
	{
		m_selectionType = (SelectionType)((m_selectionType + 1) % NUM_OF_SELECTION_TYPES);
	}
	
}

//-----------------------------------------------------------------------------------------------
void MapEditor::SwapTypeOfObject()
{
	int direction = 0;
	if(WasKeyJustPressed(G_THE_LETTER_D) || WasKeyJustPressed(KEYBOARD_LEFT_ARROW))
		direction = 1;
	if(WasKeyJustPressed(G_THE_LETTER_A) || WasKeyJustPressed(KEYBOARD_RIGHT_ARROW))
		direction = -1;

	// no key was pressed ignore
	if(direction == 0)
		return;

	// change type
	switch (m_selectionType)
	{
	case SELECTIONTYPE_TILE:
		ChangeTheTileToPlace(direction);
		break;
	case SELECTIONTYPE_UNIT:
		ChangeTheUnitToPlace(direction);
		break;
	case SELECTIONTYPE_BUILDING:
		ChangeTheBuildingToPlace(direction);
		break;
	case NUM_OF_SELECTION_TYPES:
		break;
	default:
		break;
	}
}

//-----------------------------------------------------------------------------------------------
void MapEditor::ChangeTheUnitToPlace(int direction)
{
	m_unitDefinitionIndex += direction;

	if(m_unitDefinitionIndex >= (int) UnitDefinition::s_definitionsInOrderOfLoad.size())
		m_unitDefinitionIndex = 0;
	if(m_unitDefinitionIndex < 0)
		m_unitDefinitionIndex = (int) UnitDefinition::s_definitionsInOrderOfLoad.size() - 1;

	m_currentUnitDefinition = UnitDefinition::s_definitionsInOrderOfLoad.at(m_unitDefinitionIndex);
}

//-----------------------------------------------------------------------------------------------
void MapEditor::ChangeTheTileToPlace(int direction)
{
	m_tileDefinitionIndex += direction;

	if(m_tileDefinitionIndex >= (int) TileDefinition::s_definitionsInOrderOfLoad.size())
		m_tileDefinitionIndex = 0;
	if(m_tileDefinitionIndex < 0)
		m_tileDefinitionIndex = (int) TileDefinition::s_definitionsInOrderOfLoad.size() - 1;

	m_currentTileDefinition = TileDefinition::s_definitionsInOrderOfLoad.at(m_tileDefinitionIndex);
}

//-----------------------------------------------------------------------------------------------
void MapEditor::ChangeTheBuildingToPlace(int direction)
{
	m_buildingDefinitionIndex += direction;

	if(m_buildingDefinitionIndex >= (int) BuildingDefinition::s_definitionsInOrderOfLoad.size())
		m_buildingDefinitionIndex = 0;
	if(m_buildingDefinitionIndex < 0)
		m_buildingDefinitionIndex = (int) BuildingDefinition::s_definitionsInOrderOfLoad.size() - 1;

	m_currentBuildingDefinition = BuildingDefinition::s_definitionsInOrderOfLoad.at(m_buildingDefinitionIndex);
}

//-----------------------------------------------------------------------------------------------
void MapEditor::MoveCursor()
{
	Vector2 cmouse = GetMouseCurrentPosition();
	Vector3 mousePos = m_camera->ScreenToWorldCoordinate(cmouse, 0.f);
	//
	//IntVector2 tileCoord = (cmouse * (1/ TILE_SIZE)).GetVector2AsInt();
	//DebugRenderLog(3.f, "Mouse Pos: " + mousePos.ToString());

	//--------------------------------------------------------------------------
	// Hover
	Tile* currentTile = m_currentMap->GetTile(mousePos.xy());
	if(nullptr != currentTile)
	{
		// Always update cursor so it never looks like its lagging behind
		m_cursor->SetLocalPosition(currentTile->GetCenterOfTile()+ Vector2(3.f,-3.f));
	}
}

//-----------------------------------------------------------------------------------------------
void MapEditor::MoveCamera()
{
}

//-----------------------------------------------------------------------------------------------
void MapEditor::ChangeTile()
{
	m_selectedTileToChange->m_definition = m_currentTileDefinition;
	m_currentMap->RecreateMapRenderable();
}

//-----------------------------------------------------------------------------------------------
void MapEditor::PlaceUnit()
{
	if(m_selectedTileToChange->m_unit != nullptr)
		return;

	IntVector2 pos = (m_selectedTileToChange->m_position.GetAsVector2() / TILE_SIZE).GetVector2AsInt();
	
	m_currentMap->CreateUnit(m_currentUnitDefinition->m_name, m_currentTeam, pos, 10);
}

//-----------------------------------------------------------------------------------------------
void MapEditor::PlaceBuilding()
{
	if(m_selectedTileToChange->m_building != nullptr)
		return;

	IntVector2 pos = (m_selectedTileToChange->m_position.GetAsVector2() / TILE_SIZE).GetVector2AsInt();

	m_currentMap->CreateBuilding(m_currentBuildingDefinition->m_name, m_currentTeam, pos);
}

//-----------------------------------------------------------------------------------------------
void MapEditor::DeleteUnitOrBuilding()
{
	if(m_selectedTileToChange->m_unit != nullptr)
	{
		RemoveUnit();
		m_selectedTileToChange->m_unit = nullptr;
	}

	if(m_selectedTileToChange->m_building != nullptr)
	{
		RemoveBuilding();
		m_selectedTileToChange->m_building = nullptr;
	}
}

//-----------------------------------------------------------------------------------------------
Tile* MapEditor::GetSelectedTile()
{
	Vector2 cmouse = GetMouseCurrentPosition();
	Vector3 mousePos = m_camera->ScreenToWorldCoordinate(cmouse, 0.f);
	return m_currentMap->GetTile(mousePos.xy());
}

//-----------------------------------------------------------------------------------------------
void MapEditor::RemoveUnit()
{
	PlayOneShot("default");

	Unit* unitToDestroy = m_selectedTileToChange->m_unit;

	m_currentMap->DeleteGameObjectFromMap(unitToDestroy);

}

//-----------------------------------------------------------------------------------------------
void MapEditor::RemoveBuilding()
{
	PlayOneShot("default");

	Building* buildingToDestroy = m_selectedTileToChange->m_building;

	m_currentMap->DeleteGameObjectFromMap(buildingToDestroy);
}
