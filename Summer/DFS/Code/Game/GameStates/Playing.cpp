#include "Playing.hpp"
#include "Engine\Renderer\Images/Textures/Texture.hpp"
#include "Engine\Renderer\Images/Sprites/SpriteSheet.hpp"
#include "Engine\Core\General/Camera.hpp"
#include "Game/Main/GameCommon.hpp"
#include "Engine\Core\Tools/Clock.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Core\Tools/DevConsole.hpp"
#include "Engine/Renderer/RenderableComponents/Material.hpp"
#include "Engine/Renderer/Images/Sprites/Sprite.hpp"
#include "Game/SystemsAndTools/Scene2D.hpp"
#include "Game/SystemsAndTools/SpriteRendering.hpp"
#include "Engine/Renderer/Systems/DebugRenderSystem.hpp"
#include "../General/Tiles/TileDefinition.hpp"
#include "../General/Tiles/Tile.hpp"
#include "../General/Map.hpp"
#include "Engine/Core/Tools/Command.hpp"
#include "Engine/Input/Mouse.hpp"
#include "../General/GameObjects/Cursor.hpp"
#include "Engine/Core/General/HeatMap.hpp"
#include "../General/GameObjects/Building.hpp"
#include "../General/GameObjects/Unit.hpp"


Playing::Playing()
{
	m_showHeatmap = false;
}

void Playing::StartUp()
{

	//---------------------------------------------------------
	// For Test Scene
	m_scene = new Scene2D("Test");
	m_renderingPath = new SpriteRendering();


	//---------------------------------------------------------
	// Cameras
	m_camera = new Camera();
	m_camera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_camera->SetDepthStencilTarget(g_theRenderer->m_defaultDepthTarget);

	m_scene->AddCamera(m_camera);

	g_theRenderer->SetCamera();

	//--------------------------------------------------------------------------
	// Game specific setup

	Tile newTile = Tile();
	TileDefinition* def = GetTileDefinition("default");
	newTile.ChangeTileType(*def);

	m_currentPlayState = SELECTING;

	Image mapImage = Image("Data/Images/Maps/testMap.png");
	m_currentMap = new Map("test", mapImage);
	m_cursor = new Cursor();
	m_cameraLocation = Vector2(0,0);

	//---------------------------------------------------------
	// Creating a test scene
	m_currentMap->CreateUnit("melee", TEAM_BLUE, IntVector2::ZERO);
	m_currentMap->CreateUnit("melee", TEAM_RED, IntVector2(1,0));

	GameObject2D* newBuilding = new Building();
	newBuilding->m_transform.TranslateLocal(Vector2(16.f,16.f));
	m_currentMap->AddGameObject(*newBuilding);
}

void Playing::Update()
{
	CheckKeyBoardInputs();

	if(WasKeyJustPressed(KEYBOARD_SPACE))
		m_currentMap->GoToNextTurn();

	DebugRenderLog(0.f, m_currentMap->m_turnOrder.GetCurrentTurnString());

	//m_testUnit->Update();
	m_currentMap->Update();

	m_currentMap->RemoveDeadGameObjects();

}

void Playing::Render() const
{
	//////////////////////////////////////////////////////////////////////////
	// Set up Cameras

	m_camera->SetProjectionOrtho(750, 450, -10.0f, 100.0f);
	Vector3 cursorPos = m_camera->ScreenToWorldCoordinate(GetMouseCurrentPosition(), 0.f);
	m_camera->m_viewMatrix = Matrix44::LookAt(Vector3(m_cameraLocation.x, m_cameraLocation.y, -10.f), Vector3(m_cameraLocation.x, m_cameraLocation.y, .5f));
	//Matrix44 view = Matrix44::MakeView(Vector3(0.f, 0.f, -10.f), Vector3::ZERO );
	//view = Matrix44::MakeTranslation3D(Vector3(-100.f, -100.f, -10.f));
	//m_camera->m_viewMatrix = view;

	//////////////////////////////////////////////////////////////////////////

	m_renderingPath->Render(m_scene);

	// Debug heat map
	if(m_showHeatmap)
	{
		//g_theRenderer->SetShader(Shader::CreateOrGetShader("default"));
		g_theRenderer->BindMaterial(Material::CreateOrGetMaterial("sprite"));
		g_theRenderer->SetUniform("MODEL", Matrix44());
		m_currentMap->m_heatmap->Render(4);
	}
	
}

void Playing::CheckKeyBoardInputs()
{
	if(IsDevConsoleOpen())
		return;

	Vector3 mousePos = m_camera->ScreenToWorldCoordinate(GetMouseCurrentPosition(), 0.f);
	DebugRenderLog(0.f, "Mouse Pos: " + mousePos.ToString());

	//--------------------------------------------------------------------------
	// Hover
	Tile* currentTile = m_currentMap->GetTile(mousePos.xy());
	if(nullptr != currentTile)
	{
		// Always update cursor so it never looks like its lagging behind
		m_cursor->SetLocalPosition(currentTile->GetCenterOfTile());


		if(m_currentPlayState == SELECTING)
		{
			m_cursor->SetLocalPosition(currentTile->GetCenterOfTile());
			
		}
		if(m_currentPlayState == MOVEMENT)
		{
			if(m_currentMap->CanPlayerMoveThere( m_currentMap->GetTile(mousePos.xy())->m_position))
			{
				m_currentMap->m_selectedUnit->m_transform.SetLocalPosition(currentTile->GetCenterOfTile());
			}
			
			
		}

		//--------------------------------------------------------------------------
		// Mouse click
		if(WasMouseButtonJustPressed(LEFT_MOUSE_BUTTON))
		{

			if(m_currentPlayState == SELECTING)
			{	
				// make sure there is a unit there
				if(m_currentMap->SelectUnit(mousePos.xy()) == false)
				{
					// Pop up the general menu for now 

					return;
				}

				// there is a unit there so do something with it
				if(m_currentMap->m_selectedUnit->m_beenMoved == false)
				{
					// We don't check for if its their turn cause you should be able to 
					// see where the enemy can go during your turn

					m_currentMap->CreateMovementTiles(*m_currentMap->m_selectedUnit);
					m_cursor->m_renderable->m_hidden = true;
					m_currentPlayState = MOVEMENT;
				}
				else if(m_currentMap->m_selectedUnit->m_beenMoved)
				{
					// Do an action
				}
				else
				{
					// Show general menu

				}
				
				
			}
			else if(m_currentPlayState == MOVEMENT)
			{
				// Only do moving if it's their turn
				if(m_currentMap->m_turnOrder.GetCurrentTeamTurn() == m_currentMap->m_selectedUnit->m_team)
				{
					if(m_currentMap->CanPlayerMoveThere( m_currentMap->GetTile(mousePos.xy())->m_position))
					{
						m_cursor->m_renderable->m_hidden = false;
						m_currentMap->ClearHoverTiles();
						m_currentPlayState = SELECTING;

						m_currentMap->PlaceUnit(mousePos.xy());
					}
				}
				else
				{
					// put them back
					m_currentMap->PutSelectedUnitBack();

					m_cursor->m_renderable->m_hidden = false;
					m_currentMap->ClearHoverTiles();
					m_currentPlayState = SELECTING;

				}
				
				
				
			}
		
				
			
		}
	
	}

	// do this last cause itll move the mouse 
	MoveCamera();
}

void Playing::MoveCamera()
{
	// #TODO clamp this pls
	
	if(WasKeyJustPressed(KEYBOARD_UP_ARROW))
		m_cameraLocation += Vector2::SOUTH * TILE_SIZE;
	if(WasKeyJustPressed(KEYBOARD_DOWN_ARROW))
		m_cameraLocation += Vector2::NORTH * TILE_SIZE;
	if(WasKeyJustPressed(KEYBOARD_LEFT_ARROW))
		m_cameraLocation += Vector2::EAST * TILE_SIZE;
	if(WasKeyJustPressed(KEYBOARD_RIGHT_ARROW))
		m_cameraLocation += Vector2::WEST * TILE_SIZE;
}



