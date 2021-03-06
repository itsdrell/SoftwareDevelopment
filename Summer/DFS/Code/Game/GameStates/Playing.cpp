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
#include "Engine\Renderer\Systems\Scene2D.hpp"
#include "Engine\Renderer\Systems\SpriteRendering.hpp"
#include "Engine/Renderer/Systems/DebugRenderSystem.hpp"
#include "../General/Tiles/TileDefinition.hpp"
#include "../General/Tiles/Tile.hpp"
#include "Game/General/Maps/Map.hpp"
#include "Engine/Core/Tools/Command.hpp"
#include "Engine/Input/Mouse.hpp"
#include "../General/GameObjects/Cursor.hpp"
#include "Engine/Core/General/HeatMap.hpp"
#include "../General/GameObjects/Building.hpp"
#include "../General/GameObjects/Unit.hpp"
#include "../General/UI/Container.hpp"
#include "../General/UI/UIWidget.hpp"
#include "../General/UI/HUD.hpp"
#include "Game/General/Player/CommandingOfficer.hpp"
#include "Game/General/Maps/GameHeatMap.hpp"
#include "Game/General/BattleScene/BattleCutscene.hpp"
#include "Engine/Core/Platform/Window.hpp"
#include "Game\General\Maps\BattleMap.hpp"

//====================================================================================
Tile* g_currentTile = nullptr;

//====================================================================================
Playing::Playing()
{
	m_showHeatmap = false;
}

//--------------------------------------------------------------------------
Playing::~Playing()
{
	delete m_scene;
	m_scene = nullptr;

	delete m_renderingPath;
	m_renderingPath = nullptr;

	// camera gets deleted in scene
	m_camera = nullptr;

	delete m_cursor;
	m_cursor = nullptr;

	delete m_currentMap;
	m_currentMap = nullptr;
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

	m_currentPlayState = SELECTING;

	Image mapImage = Image("Data/Images/Maps/beanIsland.png");
	m_currentMap = new BattleMap("test", mapImage);
	g_theBattleMap = m_currentMap;

	m_cursor = new Cursor();
	m_cameraLocation = Vector2(-112,-112);

	//---------------------------------------------------------
	// Creating a test scene
	CommandRunScriptFromFile("LevelScripts/beanIsland");
	m_currentMap->CreateTeams();
	//CommandRunScriptFromFile("LevelScripts/zooLevel");

}

void Playing::Update()
{
	CheckKeyBoardInputs();

	//DebugRenderLog(0.f, m_currentMap->m_turnOrder.GetCurrentTurnString());
	//DebugRender2DText(0.f, Vector2(-20.f, -40.f), m_currentMap->m_turnOrder.GetCurrentTurnString(), 10.f);

	m_currentMap->Update();

	m_currentMap->RemoveDeadGameObjects();

}

void Playing::Render() const
{
	//////////////////////////////////////////////////////////////////////////
	// Set up Cameras
	Renderer::GetInstance()->DrawAABB2(AABB2(-1000,1000), Rgba(51, 102, 255));


	//m_camera->SetProjectionOrtho(750, 450, -10.0f, 100.0f);
	m_camera->SetProjectionOrthoByAspect(Window::GetInstance()->GetHeight() * .5f); // .5f makes it bigger
	Vector3 cursorPos = m_camera->ScreenToWorldCoordinate(GetMouseCurrentPosition(), 0.f);
	m_camera->m_viewMatrix = Matrix44::LookAt(Vector3(m_cameraLocation.x, m_cameraLocation.y, -10.f), 
		Vector3(m_cameraLocation.x, m_cameraLocation.y, .5f));
	//Matrix44 view = Matrix44::MakeView(Vector3(0.f, 0.f, -10.f), Vector3::ZERO );
	//view = Matrix44::MakeTranslation3D(Vector3(-100.f, -100.f, -10.f));
	//m_camera->m_viewMatrix = view;

	//////////////////////////////////////////////////////////////////////////

	m_renderingPath->Render(m_scene);
	//m_currentMap->m_battleScene->Render();
	
	// #TODO make this a renderable once we figure out a clean way to maintain it
	m_currentMap->m_actionMenu->Render();
	m_currentMap->m_hud->Render();
	m_currentMap->m_storeMenu->Render();

	// draw the battle scene
	//g_theRenderer->SetCamera(m_currentMap->m_battleScene->GetCamera());
	//g_theRenderer->DrawTexturedAABB2(m_currentMap->m_battleScene->GetRenderedTextureOfScene(), m_currentMap->m_battleScene->GetCamera()->GetOrthoBounds());
	
	// vvvv this works but breaks mouse input regularly so it needs an if check.
	// vvvv Gonna comment it out for now
	//m_camera->m_viewMatrix = Matrix44();
	//g_theRenderer->SetCamera(m_camera);
	//g_theRenderer->DrawTexturedAABB2(m_camera->GetOrthoBounds(), 
	//	*m_currentMap->m_battleScene->GetRenderedTextureOfScene(), Vector2(0.f, 0.f), Vector2(1.f, 1.f), Rgba::WHITE);

	// Debug heat map
	if(m_showHeatmap)
	{
		//g_theRenderer->SetShader(Shader::CreateOrGetShader("default"));
		g_theRenderer->BindMaterial(Material::CreateOrGetMaterial("sprite"));
		g_theRenderer->SetUniform("MODEL", Matrix44());
		m_currentMap->m_movementHeatMap->Render(4);
	}
	
}

void Playing::CheckKeyBoardInputs()
{
	
	if(IsDevConsoleOpen())
		return;
	Vector2 cmouse = GetMouseCurrentPosition();
	Vector3 mousePos = m_camera->ScreenToWorldCoordinate(cmouse, 0.f);
	//
	//IntVector2 tileCoord = (cmouse * (1/ TILE_SIZE)).GetVector2AsInt();
	//DebugRenderLog(3.f, "Mouse Pos: " + mousePos.ToString());

	//--------------------------------------------------------------------------
	// UI input (deosnt rely on if there is a tile there or not)
	if(WasMouseButtonJustReleased(LEFT_MOUSE_BUTTON))
	{
		if(m_currentMap->m_currentContainer != nullptr)
			m_currentMap->m_currentContainer->OnClick();
	}


	//--------------------------------------------------------------------------
	// Hover
	Tile* currentTile = m_currentMap->GetTile(mousePos.xy());
	if(nullptr != currentTile)
	{
		// Always update cursor so it never looks like its lagging behind
		m_cursor->SetLocalPosition(currentTile->GetCenterOfTile()+ Vector2(3.f,-3.f));
		
		// Store it off so other classes can use it easier
		g_currentTile = currentTile;

		if(m_currentPlayState == SELECTING)
		{
			m_cursor->m_renderable->m_hidden = false;
			m_cursor->SetLocalPosition(currentTile->GetCenterOfTile() + Vector2(3.f,-3.f));
			
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
				//Tile* currentTile = m_currentMap->GetTile(mousePos.xy());
				bool isUnit = (currentTile->m_unit != nullptr);
				bool isBuilding = (currentTile->m_building != nullptr);


				// make sure there is a unit there
				if(isUnit == false && isBuilding == false)
				{
					if(m_currentMap->m_currentContainer != m_currentMap->m_storeMenu)
					{
						// Pop up the general menu for now 
						m_currentMap->m_actionMenu->AddPauseMenu();
						g_theBattleMap->m_currentContainer = m_currentMap->m_actionMenu;
						return;
					}
					
				}

				if(isUnit)
				{
					m_currentMap->m_selectedUnit = currentTile->m_unit;
					
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
						g_theBattleMap->m_currentContainer = m_currentMap->m_actionMenu;
						m_currentMap->m_actionMenu->AddPauseMenu();
						return;
					}
				}
				else if(isBuilding)
				{
					// not your factory
					if(currentTile->m_building->m_team != m_currentMap->m_currentOfficer->m_team)
						return;
					
					m_currentMap->m_selectedBuilding = currentTile->m_building;
					m_currentMap->CreateStoreUI();
				}
				
				
				
			}
			else if(m_currentPlayState == MOVEMENT)
			{
				// Only do moving if it's their turn
				if(m_currentMap->m_turnOrder.GetCurrentTeamTurn() == m_currentMap->m_selectedUnit->m_team)
				{
					if(m_currentMap->CanPlayerMoveThere( m_currentMap->GetTile(mousePos.xy())->m_position))
					{
						//m_cursor->m_renderable->m_hidden = false;
						m_currentMap->ClearHoverTiles();
						
						m_currentMap->CreateActionTiles(*m_currentMap->m_selectedUnit);
						
						// if they can add ui widget
						if(m_currentMap->CanUnitCaptureBuilding(*m_currentMap->m_selectedUnit))
						{
							UIWidget* capWidget = new UIWidget(*UIWidgetDefinition::GetUIWidgetDefinition("capture"));
							m_currentMap->m_actionMenu->AddWidget(*capWidget);
						}

						UIWidget* newWidget = new UIWidget(*UIWidgetDefinition::GetUIWidgetDefinition("wait"));
						m_currentMap->m_actionMenu->AddWidget(*newWidget);
						g_theBattleMap->m_currentContainer = m_currentMap->m_actionMenu;

						m_currentMap->PlaceUnit(mousePos.xy());
						m_currentPlayState = ACTION;
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
			else if(m_currentPlayState == ACTION)
			{
				// make sure they clicked a valid action
				if(m_currentMap->CheckForAction(m_currentMap->GetTile(mousePos.xy())->m_position))
				{
					m_currentMap->ClearAttackTiles();
					m_currentMap->m_actionMenu->CloseMenu();
					m_currentPlayState = SELECTING;
				}
				

			}

		}
	
	}


	ShowAttackRange();

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

void Playing::ShowAttackRange()
{
	// show hover range
	if(m_currentMap->m_selectedUnit != nullptr)
	{
		if(WasKeyJustPressed(G_THE_LETTER_R))
		{
			m_currentMap->CreateAttackTiles(*m_currentMap->m_selectedUnit, true);
		}

		if(WasKeyJustReleased(G_THE_LETTER_R))
		{
			m_currentMap->ClearAttackTiles();
		}
	}
}


