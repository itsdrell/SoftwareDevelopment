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


Playing::Playing()
{
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

	m_currentMap = new Map("test", IntVector2(8,8));
	m_cursor = new Cursor();
}

void Playing::Update()
{
	CheckKeyBoardInputs();

	

}

void Playing::Render() const
{
	//////////////////////////////////////////////////////////////////////////
	// Set up Cameras
	//m_camera->SetProjectionOrtho(10, 10, -10.0f, 20.0f);
	//m_camera->SetPerspective(45.f, (16.f/9.f), .1f , 100.f);


	//Matrix44 cameraPos = Matrix44::MakeTranslation3D(Vector3(0.f, 0.f, -10.f));
	//m_camera->m_cameraMatrix = cameraPos;//modelMatrix;
	//m_camera->m_viewMatrix = InvertFast(cameraPos); // model); // inverse this 
	m_camera->SetProjectionOrtho(375, 225, -10.0f, 100.0f);
	Matrix44 view = Matrix44::MakeView(Vector3(0.f,0.f,-10.f), Vector3::ZERO );
	m_camera->m_viewMatrix = view;
	//m_camera->m_cameraMatrix = InvertFast(view); // maybe wrong

	// Set the camera
	//g_theRenderer->SetCamera(m_camera);

	//////////////////////////////////////////////////////////////////////////

	m_renderingPath->Render(m_scene);

	// Debug heat map
	//g_theRenderer->SetShader(Shader::CreateOrGetShader("default"));
	g_theRenderer->BindMaterial(Material::CreateOrGetMaterial("sprite"));
	g_theRenderer->SetUniform("MODEL", Matrix44());
	m_currentMap->m_heatmap->Render(4);
}

void Playing::CheckKeyBoardInputs()
{
	if(IsDevConsoleOpen())
		return;

	Vector3 mousePos = m_camera->ScreenToWorldCoordinate(GetMouseCurrentPosition(), 0.f);
	DebugRenderLog(0.f, "Mouse Pos: " + mousePos.ToString());


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
				GameObject2D player = *m_currentMap->m_gameObjects.at(0);

				m_currentMap->m_gameObjects.at(0)->m_transform.SetLocalPosition(currentTile->GetCenterOfTile());
			}
			
			
		}


		if(WasMouseButtonJustPressed(LEFT_MOUSE_BUTTON))
		{

			if(m_currentPlayState == SELECTING)
			{
				m_cursor->m_renderable->m_hidden = true;
				Unit* player = (Unit*) m_currentMap->m_gameObjects.at(0);
				m_currentMap->CreateMovementTiles(*player);
				m_currentPlayState = MOVEMENT;
			}
			else if(m_currentPlayState == MOVEMENT)
			{
				if(m_currentMap->CanPlayerMoveThere( m_currentMap->GetTile(mousePos.xy())->m_position))
				{
					m_cursor->m_renderable->m_hidden = false;
					m_currentMap->ClearHoverTiles();
					m_currentPlayState = SELECTING;
				}
				
			}
		
				
			
		}
	
	}
}




