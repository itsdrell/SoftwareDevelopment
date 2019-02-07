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
#include "Game\General\UI\UIWidget.hpp"

//===============================================================================================
MapEditor::MapEditor()
{
	Renderer* r = Renderer::GetInstance();
	m_tileBounds =		GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2(.1f, .7f), Vector2(.2f, .87f));
	m_unitBounds =		GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2(.1f, .5f), Vector2(.2f, .67f));
	m_buildingBounds =	GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2(.1f, .3f), Vector2(.2f, .47f));
	m_deleteBounds =	GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2(.1f, .1f), Vector2(.2f, .27f));
}

//-----------------------------------------------------------------------------------------------
void MapEditor::StartUp()
{
	//m_scene = new Scene2D("Test");
	m_renderingPath = new SpriteRendering();
	
	CreateNewMap();

	AddWidgets();

	GetDefinitions();
}

//-----------------------------------------------------------------------------------------------
void MapEditor::CreateNewMap()
{
	if(m_currentMap != nullptr)
		delete m_currentMap;

	m_currentMap = new Map("new map:)", IntVector2(30,20));

	m_camera = new Camera();
	m_camera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_camera->SetDepthStencilTarget(g_theRenderer->m_defaultDepthTarget);
	m_currentMap->m_scene->AddCamera(m_camera);

	g_theRenderer->SetCamera();

	m_cursor = new Cursor();
	m_cameraLocation = Vector2(-112,-160);

}

//-----------------------------------------------------------------------------------------------
void MapEditor::AddWidgets()
{
	Renderer* r = Renderer::GetInstance();
	
	UIWidget* exitWidget = new UIWidget(*UIWidgetDefinition::GetUIWidgetDefinition("exitLevelEditor"));
	AABB2 exitBounds = GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2(.8f, .01f), Vector2(.99f, .08f));
	exitWidget->GenerateBounds(exitBounds);
	m_widgets.push_back(exitWidget);

	UIWidget* clearMapWidget = new UIWidget(*UIWidgetDefinition::GetUIWidgetDefinition("clearMap"));
	AABB2 clearBounds = GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2(.31f, .01f), Vector2(.49f, .08f));
	clearMapWidget->GenerateBounds(clearBounds);
	m_widgets.push_back(clearMapWidget);

	UIWidget* selectUnitWidget = new UIWidget(*UIWidgetDefinition::GetUIWidgetDefinition("changeLevelEditorUnit"));
	selectUnitWidget->GenerateBounds(m_unitBounds);
	m_widgets.push_back(selectUnitWidget);

	UIWidget* selectBuildingWidget = new UIWidget(*UIWidgetDefinition::GetUIWidgetDefinition("changeLevelEditorBuilding"));
	selectBuildingWidget->GenerateBounds(m_buildingBounds);
	m_widgets.push_back(selectBuildingWidget);

	UIWidget* selectTileWidget = new UIWidget(*UIWidgetDefinition::GetUIWidgetDefinition("changeLevelEditorTile"));
	selectTileWidget->GenerateBounds(m_tileBounds);
	m_widgets.push_back(selectTileWidget);

	UIWidget* selectDeleteWidget = new UIWidget(*UIWidgetDefinition::GetUIWidgetDefinition("changeLevelEditorDelete"));
	selectDeleteWidget->GenerateBounds(m_deleteBounds);
	m_widgets.push_back(selectDeleteWidget);

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

	DeleteWidgets();
}

//-----------------------------------------------------------------------------------------------
void MapEditor::Exit()
{

}

//-----------------------------------------------------------------------------------------------
void MapEditor::DeleteWidgets()
{
	for(uint i = 0; i < m_widgets.size(); i++)
	{
		UIWidget* current = m_widgets.at(i);
		delete current;
		current = nullptr;
	}

	m_widgets.clear();
}

//-----------------------------------------------------------------------------------------------
void MapEditor::Update()
{
	m_currentMap->Update();

	if(m_widgets.size() != 0)
	{
		for(uint i = 0; i < m_widgets.size(); i++)
		{
			UIWidget*& current = m_widgets.at(i);

			current->Update();
		}
	}
	
	
	CheckKeyboardInputs();
}

//-----------------------------------------------------------------------------------------------
void MapEditor::SwapTeamColors()
{
	if(WasKeyJustPressed(KEYBOARD_TAB))
	{
		if(m_selectionType == SELECTIONTYPE_UNIT)
		{
			if(m_currentUnitTeam == TEAM_RED)
				m_currentUnitTeam = TEAM_BLUE;
			else
				m_currentUnitTeam = TEAM_RED;
		}
		

		if(m_selectionType == SELECTIONTYPE_BUILDING)
		{
			if(m_currentBuildingTeam == TEAM_NONE) { m_currentBuildingTeam = TEAM_RED; return; }
			if(m_currentBuildingTeam == TEAM_RED) { m_currentBuildingTeam = TEAM_BLUE; return; }
			if(m_currentBuildingTeam == TEAM_BLUE) { m_currentBuildingTeam = TEAM_NONE; return; }

		}
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
	RenderBackgroundUI();
	RenderCurrentSelectionBar();
	RenderCurrentPaintMethod();
	RenderSelectionBar();

	if(m_widgets.size() != 0)
	{
		for(UIWidget* currentWidget : m_widgets)
		{
			currentWidget->Render();
		}
	}
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
	SpriteAnimator theAnimator = SpriteAnimator(Unit::GetAnimatorName(m_currentUnitDefinition->m_name, m_currentUnitTeam));
	Sprite* currentUnitSprite = theAnimator.GetCurrentSprite();
	r->SetCurrentTexture(0, currentUnitSprite->m_image);
	r->DrawTexturedAABB2(m_unitBounds, *currentUnitSprite->m_image, currentUnitSprite->m_uv.mins, currentUnitSprite->m_uv.maxs, Rgba::WHITE);

	// buildings
	Texture* buildingTexture = g_buildingSpriteSheet.m_spriteSheetTexture;
	AABB2 newSpriteCoords = g_buildingSpriteSheet.GetTexCoordsForSpriteCoords(IntVector2(m_currentBuildingDefinition->m_spriteCoords.x, m_currentBuildingTeam));
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
		space = GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2(.0f,.76f), Vector2(.3f, .81f));
		break;
	case SELECTIONTYPE_BUILDING:
		space = GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2(.0f,.34f), Vector2(.3f, .39f));
		break;
	case SELECTIONTYPE_UNIT:
		space = GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2(.0f,.54f), Vector2(.3f, .59f));
		break;
	case SELECTIONTYPE_DELETE:
		space = GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2(.0f,.14f), Vector2(.3f, .19f));
		break;
	default:
		break;
	}

	r->DrawAABB2(space, Rgba::YELLOW);
}

//-----------------------------------------------------------------------------------------------
void MapEditor::RenderBackgroundUI() const
{
	Renderer* r = Renderer::GetInstance();
	r->SetCurrentTexture();

	//AABB2 bottomBar = GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2::ZERO, Vector2(1.f, .2f));
	//AABB2 bottomOutline = GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2::ZERO, Vector2(1.f, .21f));
	//r->DrawAABB2(bottomOutline, Rgba::RED);
	//r->DrawAABB2(bottomBar, Rgba::RAINBOW_VIOLET);

	AABB2 leftBar = GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2::ZERO, Vector2(.3f, 1.f));
	r->DrawAABB2(leftBar, Rgba::RAINBOW_VIOLET);
}

//-----------------------------------------------------------------------------------------------
void MapEditor::RenderCurrentSelectionBar() const
{
	Renderer* r = Renderer::GetInstance();
	AABB2 topBar = GetBounds(r->m_defaultUICamera->GetOrthoBounds(), Vector2(.32f, .9f), Vector2(.98f, .98f));

	if(m_selectionType == SELECTIONTYPE_UNIT)
		r->DrawAABB2(topBar, GetColorFromTeamName(m_currentUnitTeam));
	else 
		r->DrawAABB2(topBar, GetColorFromTeamName(m_currentBuildingTeam));


	String textToShow = GetCurrentSelectionText();
	r->DrawFittedTextInBox(topBar, textToShow, 3.f, 1.f);
}

//-----------------------------------------------------------------------------------------------
void MapEditor::CheckKeyboardInputs()
{
	if(IsDevConsoleOpen())
		return;

	if(WasMouseButtonJustReleased(LEFT_MOUSE_BUTTON))
	{
		if(m_currentHistory != nullptr && m_currentHistory->m_history.size() > 0)
		{
			m_history.push_front(m_currentHistory);

			// the queue will handle deleting, this is just so we know to make
			// a new one on click
			m_currentHistory = nullptr;
		}
	}

	SwapTeamColors();
	SwapPlacementMode();
	SwapTypeOfObject();
	PlaceObjectOrTile();
	
	
	MoveCursor();

	if(WasMouseButtonJustPressed(LEFT_MOUSE_BUTTON))
	{
		for(uint i = 0; i < m_widgets.size(); i++)
		{
			UIWidget* current = m_widgets.at(i);
			if(current != nullptr)
			{
				if(current->m_isHoveredOver)
				{
					current->OnClick();
					PlayOneShot("default");
				}
			}
		}
	}

	if(WasKeyJustPressed(KEYBOARD_BACKSPACE))
	{
		Undo();
	}


	// check if we need to remake the renderable
	if(m_isDirty)
	{
		m_currentMap->RecreateMapRenderable();
		m_isDirty = false;
	}
	
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

		if(m_currentHistory == nullptr)
			m_currentHistory = new History();
		
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
	if(WasKeyJustPressed(G_THE_LETTER_W) || WasKeyJustPressed(KEYBOARD_UP_ARROW) /*|| DidMouseWheelScrollUp()*/)
	{
		m_selectionType = (SelectionType)((m_selectionType + NUM_OF_SELECTION_TYPES - 1) % NUM_OF_SELECTION_TYPES);
	}

	if(WasKeyJustPressed(G_THE_LETTER_S) || WasKeyJustPressed(KEYBOARD_DOWN_ARROW) /*|| DidMouseWheelScrollDown()*/)
	{
		m_selectionType = (SelectionType)((m_selectionType + 1) % NUM_OF_SELECTION_TYPES);
	}
	
}

//-----------------------------------------------------------------------------------------------
void MapEditor::SwapTypeOfObject()
{
	int direction = 0;
	if(WasKeyJustPressed(G_THE_LETTER_D) || WasKeyJustPressed(KEYBOARD_LEFT_ARROW) || DidMouseWheelScrollDown())
		direction = 1;
	if(WasKeyJustPressed(G_THE_LETTER_A) || WasKeyJustPressed(KEYBOARD_RIGHT_ARROW) || DidMouseWheelScrollUp())
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
void MapEditor::Undo()
{
	if(m_history.size() == 0)
		return;

	History* previous = m_history.front();

	for(uint i = 0; i < previous->m_history.size(); i++)
	{
		HistoryItem* current = previous->m_history.at(i);

		current->Undo();	

		delete current;
		current = nullptr;
	}

	m_history.pop_front();

	delete previous;
	previous = nullptr;
}

//-----------------------------------------------------------------------------------------------
void MapEditor::ChangeTile()
{
	TileDefinition* previousDef = m_selectedTileToChange->m_definition;
	
	// prevent duplicates
	if(previousDef == m_currentTileDefinition)
		return;

	m_selectedTileToChange->m_definition = m_currentTileDefinition;
	m_currentMap->RecreateMapRenderable();

	TileHistory* newHistory = new TileHistory(previousDef, *m_selectedTileToChange);
	m_currentHistory->m_history.push_back(newHistory);
}

//-----------------------------------------------------------------------------------------------
void MapEditor::PlaceUnit()
{
	if(m_selectedTileToChange->m_unit != nullptr)
		return;

	IntVector2 pos = (m_selectedTileToChange->m_position.GetAsVector2() / TILE_SIZE).GetVector2AsInt();
	Unit* newUnit = m_currentMap->CreateUnit(m_currentUnitDefinition->m_name, m_currentUnitTeam, pos, 10);

	// I am creating a tile for the unit if it needs it, I could also just not spawn the unit it the tile
	// underneath is not the right type but idk which i prefer atm
	Tile* tileWeAreOn = newUnit->m_tileIAmOn;
	if(newUnit->m_definition->m_tileNeededUnderneath != nullptr 
		&& newUnit->m_definition->m_tileNeededUnderneath != tileWeAreOn->m_definition)
	{
		TileHistory* newHistory = new TileHistory(tileWeAreOn->m_definition, *tileWeAreOn);
		m_currentHistory->m_history.push_back(newHistory);
		
		tileWeAreOn->m_definition = newUnit->m_definition->m_tileNeededUnderneath;
		MarkRenderableAsDirty();
	}
	
	UnitHistory* newHistory = new UnitHistory(pos);
	m_currentHistory->m_history.push_back(newHistory);
}

//-----------------------------------------------------------------------------------------------
void MapEditor::PlaceBuilding()
{
	if(m_selectedTileToChange->m_building != nullptr)
		return;

	IntVector2 pos = (m_selectedTileToChange->m_position.GetAsVector2() / TILE_SIZE).GetVector2AsInt();

	Building* newBuilding = m_currentMap->CreateBuilding(m_currentBuildingDefinition->m_name, m_currentBuildingTeam, pos);

	// I am creating a tile for the unit if it needs it, I could also just not spawn the unit it the tile
	// underneath is not the right type but idk which i prefer atm
	Tile* tileWeAreOn = newBuilding->m_tileReference;
	if(newBuilding->m_definition->m_tileToSpawnBeneath != nullptr 
		&& newBuilding->m_definition->m_tileToSpawnBeneath != tileWeAreOn->m_definition)
	{
		TileHistory* newHistory = new TileHistory(tileWeAreOn->m_definition, *tileWeAreOn);
		m_currentHistory->m_history.push_back(newHistory);

		tileWeAreOn->m_definition = newBuilding->m_definition->m_tileToSpawnBeneath;
		MarkRenderableAsDirty();
	}

	BuildingHistory* newHistory = new BuildingHistory(pos);
	m_currentHistory->m_history.push_back(newHistory);
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
String MapEditor::GetCurrentSelectionText() const
{
	String result = "idk what I am doing, man";
	switch (m_selectionType)
	{
	case SELECTIONTYPE_TILE:
		result = "Painting: " + m_currentTileDefinition->m_name + " tile :)";
		break;
	case SELECTIONTYPE_UNIT:
		result = "Creating: " + m_currentUnitDefinition->m_displayName + " for team: " + TeamNameToString(m_currentUnitTeam);
		break;
	case SELECTIONTYPE_BUILDING:
		result = "Creating: " + m_currentBuildingDefinition->m_displayName + " for team: " + TeamNameToString(m_currentBuildingTeam);
		break;
	case SELECTIONTYPE_DELETE:
		result = "D E L E T I N G	>:D";
		break;
	case NUM_OF_SELECTION_TYPES:
		break;
	default:
		break;
	}

	return result;
}

//-----------------------------------------------------------------------------------------------
void MapEditor::RemoveUnit()
{
	PlayOneShot("default");

	Unit* unitToDestroy = m_selectedTileToChange->m_unit;

	DeleteHistory* newHistory = new DeleteHistory(
		UNDO_TYPE_UNIT, 
		unitToDestroy->m_team, 
		unitToDestroy->m_definition->m_name, 
		unitToDestroy->GetTilePosition(TILE_SIZE_INT));
	m_currentHistory->m_history.push_back(newHistory);

	unitToDestroy->Die();
}

//-----------------------------------------------------------------------------------------------
void MapEditor::RemoveBuilding()
{
	PlayOneShot("default");

	Building* buildingToDestroy = m_selectedTileToChange->m_building;

	m_currentMap->CreateEffect("explosion", buildingToDestroy->GetTilePosition(TILE_SIZE_INT));

	DeleteHistory* newHistory = new DeleteHistory(
		UNDO_TYPE_BUILDING, 
		buildingToDestroy->m_team, 
		buildingToDestroy->m_definition->m_name, 
		buildingToDestroy->GetTilePosition(TILE_SIZE_INT));
	m_currentHistory->m_history.push_back(newHistory);

	m_currentMap->DeleteGameObjectFromMap(buildingToDestroy);
}