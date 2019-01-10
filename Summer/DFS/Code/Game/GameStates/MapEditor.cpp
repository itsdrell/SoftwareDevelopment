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

//===============================================================================================
MapEditor::MapEditor()
{
}

//-----------------------------------------------------------------------------------------------
void MapEditor::StartUp()
{
	//m_scene = new Scene2D("Test");
	m_renderingPath = new SpriteRendering();
	
	m_currentMap = new Map("new map:)", IntVector2(10,10));

	//---------------------------------------------------------
	// Cameras
	m_camera = new Camera();
	m_camera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_camera->SetDepthStencilTarget(g_theRenderer->m_defaultDepthTarget);

	m_currentMap->m_scene->AddCamera(m_camera);

	g_theRenderer->SetCamera();
	
	m_cursor = new Cursor();
	m_cameraLocation = Vector2(-112,-112);
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
void MapEditor::Render() const
{
	Renderer::GetInstance()->DrawAABB2(AABB2(-1000,1000), Rgba(51, 102, 255));

	m_camera->SetProjectionOrthoByAspect(Window::GetInstance()->GetHeight() * .5f); // .5f makes it bigger
	Vector3 cursorPos = m_camera->ScreenToWorldCoordinate(GetMouseCurrentPosition(), 0.f);
	m_camera->m_viewMatrix = Matrix44::LookAt(Vector3(m_cameraLocation.x, m_cameraLocation.y, -10.f), 
		Vector3(m_cameraLocation.x, m_cameraLocation.y, .5f));

	m_renderingPath->Render(m_currentMap->m_scene);
}

//-----------------------------------------------------------------------------------------------
void MapEditor::CheckKeyboardInputs()
{
	if(IsDevConsoleOpen())
		return;

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
		default:
			break;
		}
	}
	
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
	m_selectedTileToChange->m_definition = GetTileDefinition("grass");
	m_currentMap->RecreateMapRenderable();
}

//-----------------------------------------------------------------------------------------------
void MapEditor::PlaceUnit()
{
	if(m_selectedTileToChange->m_unit != nullptr)
		return;

	IntVector2 pos = (m_selectedTileToChange->m_position.GetAsVector2() / TILE_SIZE).GetVector2AsInt();
	
	m_currentMap->CreateUnit("grunt", TEAM_RED, pos, 10);
}

//-----------------------------------------------------------------------------------------------
void MapEditor::PlaceBuilding()
{
	if(m_selectedTileToChange->m_building != nullptr)
		return;

	IntVector2 pos = (m_selectedTileToChange->m_position.GetAsVector2() / TILE_SIZE).GetVector2AsInt();

	m_currentMap->CreateBuilding("default", TEAM_NONE, pos);
}

//-----------------------------------------------------------------------------------------------
Tile* MapEditor::GetSelectedTile()
{
	Vector2 cmouse = GetMouseCurrentPosition();
	Vector3 mousePos = m_camera->ScreenToWorldCoordinate(cmouse, 0.f);
	return m_currentMap->GetTile(mousePos.xy());
}
