#include "Playing.hpp"
#include "Engine\Renderer\Images/Textures/Texture.hpp"
#include "Engine\Renderer\Images/Sprites/SpriteSheet.hpp"
#include "Engine\Core\General/Camera.hpp"
#include "Game/Main/GameCommon.hpp"
#include "Engine\Core\Tools/Clock.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Core\Tools/DevConsole.hpp"
#include "Engine/Renderer/Systems/MeshBuilder.hpp"
#include "../GameSpecific/Player.hpp"
#include "Engine/Renderer/RenderableComponents/Material.hpp"
#include "Engine/Renderer/Systems/Lights.hpp"
#include "Engine/Renderer/Systems/DebugRenderSystem.hpp"
#include "Engine/Math/Ranges/FloatRange.hpp"
#include "Game/GameSpecific/GameMap.hpp"
#include "../GameSpecific/Enemy.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "../Main/Game.hpp"
#include "Engine/Core/Tools/Command.hpp"
#include "../GameSpecific/HUD.hpp"
#include "../GameSpecific/EnemySpawner.hpp"

//====================================================================================
void GameWon(Command& thecommand)
{
	bool condition = true;
	
	if(thecommand.m_commandArguements.size() > 1)
	{
		condition = ParseString(thecommand.m_commandArguements.at(1), condition);
	}

	if(condition)
	{
		g_theGame->m_playingState->m_enemySpawner.clear();
		g_theGame->m_playingState->m_enemies.clear();
	}
	else
	{
		g_theGame->m_playingState->m_player->m_currentHealth = 0;
	}

	g_theGame->m_playingState->Exit(condition);
	DevConsole::GetInstance()->Toggle();
	
}

//====================================================================================
Playing::Playing()
{
	CommandRegister("gameWon","gameWon <bool>","Win or lost game", GameWon);
}

void Playing::StartUp()
{
	// Create the global sprite sheet for all the textures to use
	Texture* TileTexture = g_theRenderer->CreateOrGetTexture("Data/Images/Terrain_32x32.png");
	g_tileSpriteSheet = SpriteSheet(TileTexture,32,32);

	
	//---------------------------------------------------------
	// For Test Scene
	m_scene = new Scene("Test");
	m_uiScene = new Scene("UI", false);
	m_renderingPath = new ForwardRenderingPath();

	m_map = new GameMap();
	m_map->LoadMap(AABB2(-128.f, 128.f), FloatRange(0.f, 6.f), IntVector2(16,16), 20.f);

	m_hud = new HUD();

	//---------------------------------------------------------
	// Cameras
	m_camera = new Camera();
	m_camera->CreateSkyBox("Data/Images/skybox.jpg");
	m_camera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_camera->SetDepthStencilTarget(g_theRenderer->m_defaultDepthTarget);

	m_scene->AddCamera(m_camera);
	m_uiScene->AddCamera(g_theRenderer->m_defaultUICamera);

	g_theRenderer->SetCamera();
	//---------------------------------------------------------

	g_theRenderer->SetAmbientLight(.3f, Rgba::WHITE);

	m_sun = new DirectionalLight(0, Vector3(0.f, 100.f, 0.f), Vector3::DOWN, .6f);
	m_scene->AddLight(m_sun);

}

void Playing::Enter()
{
	m_player = AddPlayer();
	m_testEnemy = AddEnemy(Vector3(20.f, 0.f, 0.f));
	m_testSpawner = AddEnemySpawner(Vector2(0.f, 20.f));

}

void Playing::Exit(bool removeEnemies)
{
	// Do some cleanup so we don't have leftovers
	//RemoveRenderable(m_player->m_renderable);

	if(removeEnemies)
	{
		for(uint i = 0; i < m_enemies.size(); i++)
		{
			RemoveRenderable(m_enemies.at(i)->m_renderable);
		}

		// remove spawners as well #TODO
	}
}

Player* Playing::AddPlayer()
{
	Player* newPlayer = new Player();

	MeshBuilder mb;
	//mb.AddUVSphere(Vector3::ZERO, 1.f, 16, 16);
	mb.AddCube(Vector3::ZERO, Vector3(1.f, .2f, 1.f));
	newPlayer->m_renderable->SetMesh(mb.CreateMesh<VertexLit>());

	Material* playerMaterial = Material::CreateOrGetMaterial("geo");
	playerMaterial->SetTint(Rgba::WHITE);
	//playerMaterial->SetTexture(0, g_theRenderer->m_defaultTexture);
	//playerMaterial->SetTexture(1, g_theRenderer->m_defaultTexture);
	//playerMaterial->SetTexture(2, g_theRenderer->m_defaultTexture);


	newPlayer->m_renderable->SetMaterial( playerMaterial );
	newPlayer->m_renderable->m_usesLight = true;

	m_scene->AddRenderable(newPlayer->m_renderable);

	return newPlayer;
}

Enemy* Playing::AddEnemy(const Vector3& pos)
{
	Enemy* newEnemy = new Enemy(pos);

	MeshBuilder mb;
	//mb.AddMeshFromObjFile("Data/Model/Mech/leo.obj");
	mb.AddUVSphere(Vector3::ZERO, 1.f, 16, 16, Rgba::WHITE);
	mb.AddUVSphere(Vector3::ONE, 1.f, 16, 16, Rgba::WHITE);
	newEnemy->m_renderable->SetMesh(mb.CreateMesh<VertexLit>());

	Material* enemyMaterial = Material::CreateOrGetMaterial("geo");
	enemyMaterial->SetTexture(0, g_theRenderer->m_defaultTexture);
	Rgba tint = Rgba::BLACK;
	enemyMaterial->SetTint(tint);

	newEnemy->m_renderable->SetMaterial( enemyMaterial );
	newEnemy->m_renderable->m_usesLight = true;

	// Add to the containers
	m_scene->AddRenderable(newEnemy->m_renderable);
	m_enemies.push_back(newEnemy);

	if(m_testEnemy == nullptr)
		m_testEnemy = newEnemy;

	return newEnemy;
}

EnemySpawner* Playing::AddEnemySpawner(const Vector2& pos)
{
	EnemySpawner* newSpawner = new EnemySpawner(pos);

	MeshBuilder mb;
	//mb.AddMeshFromObjFile("Data/Model/Mech/leo.obj");
	mb.AddCube(Vector3::ZERO, Vector3(1.f,10.f, 1.f));
	newSpawner->m_renderable->SetMesh(mb.CreateMesh<VertexLit>());

	Material* enemyMaterial = Material::CreateOrGetMaterial("geo");
	enemyMaterial->SetTexture(0, g_theRenderer->m_defaultTexture);
	enemyMaterial->SetTint(Rgba::BLACK);

	newSpawner->m_renderable->SetMaterial( enemyMaterial );
	newSpawner->m_renderable->m_usesLight = true;

	// Add to the containers
	m_scene->AddRenderable(newSpawner->m_renderable);
	m_enemySpawner.push_back(newSpawner);

	return newSpawner;
}

void Playing::Update()
{
	m_player->Update();
	m_testSpawner->Update();

	m_testEnemy->m_renderable->m_material->SetTint(Rgba::BLUE);

	for(uint i = 0; i < m_enemies.size(); i++)
	{
		m_enemies.at(i)->Update();
	}

	for(uint j = 0; j < m_enemySpawner.size(); j++)
	{
		m_enemySpawner.at(j)->Update();
	}

	CheckKeyBoardInputs();
	

	//--------------------------------------------------------------------------
	//m_testEnemy->m_transform.RotateLocalByEuler(Vector3(0.f, 90.f, 0.f));
	//DebugRenderLog(0.f, "Enemy:" + m_testEnemy->m_transform.GetLocalPosition().ToString());
	DebugRenderBasis(0.f, m_testEnemy->m_transform.GetWorldMatrix());
	
	//DebugRenderLog(0.f, "Player: " + m_player->m_transform.GetWorldPosition().ToString());
	DebugRenderBasis(0.f, m_player->m_transform.GetWorldMatrix());

	//--------------------------------------------------------------------------


	m_hud->Update();
}

void Playing::Render() const
{
	//////////////////////////////////////////////////////////////////////////
	// Set up Cameras
	//m_camera->SetProjectionOrtho(10, 10, -10.0f, 20.0f);
	m_camera->SetPerspective(45.f, (16.f/9.f), .1f , 100.f);

	Matrix44 modelMatrix = Matrix44::LookAt(
		m_player->m_cameraLocation.GetWorldPosition(), 
		m_player->m_transform.GetWorldPosition() , 
		m_player->m_transform.GetLocalUp()); 

	m_camera->m_cameraMatrix = modelMatrix;
	m_camera->m_viewMatrix = InvertFast(modelMatrix); // inverse this 


	// Set the camera
	g_theRenderer->SetCamera(m_camera);

	//////////////////////////////////////////////////////////////////////////

	m_renderingPath->Render(m_scene);
	m_renderingPath->Render(m_uiScene);
}

void Playing::CheckKeyBoardInputs()
{
	if(IsDevConsoleOpen())
		return;

	//PlayLoopingSound("bg");

	if(WasKeyJustPressed(KEYBOARD_SPACE))
	{
		m_player->Shoot();
	}

	if(WasKeyJustPressed(G_THE_LETTER_R))
		m_testEnemy->m_transform.RotateLocalByEuler(Vector3(90.f, 0.f, 0.f));
	if(WasKeyJustPressed(G_THE_LETTER_T))
		m_player->m_transform.RotateLocalByEuler(Vector3(0.f, 90.f, 0.f));
	if(WasKeyJustPressed(KEYBOARD_BACKSPACE))
		m_player->TakeDamage();


	CameraInput();

	CheckWinLossStates();
}

void Playing::CameraInput()
{
	float dt = g_theGameClock->deltaTime; // this needs to be after keyboard because we might fuck with ds for go to next frames

	float rotationSpeed = 2.0f;
	//
	//// Apply Rotation
	Vector2 mouse_delta = g_theInput->GetMouseDelta();
	//mouse_delta = mouse_delta.GetNormalized();
	
	// m_current_cam_euler; // defined, starts at zero
	Vector3 local_euler = Vector3::ZERO; 
	local_euler.y = mouse_delta.x * rotationSpeed * dt; 
	local_euler.x = mouse_delta.y * rotationSpeed * dt; 
	
	Vector3 currentRotation = m_player->m_transform.GetLocalEulerAngles();
	currentRotation.x += local_euler.x; 
	currentRotation.y += local_euler.y; 

	currentRotation.x = ClampFloat( currentRotation.x, -90.f, 90.f );
	currentRotation.y = fmod(currentRotation.y, 360.f ); 

	//m_player->m_transform.SetLocalRotationEuler( currentRotation);

	Vector3 movement = GetMovement();


	//////////////////////////////////////////////////////////////////////////
	// Trail
	Vector3 previousPosition = m_player->m_transform.GetLocalPosition();
	//Vector3 newPosition = m_camera->transform.GetLocalPosition() + movement;
	//DebugRenderLineSegment(5.f,previousPosition,newPosition,DEBUG_RENDER_USE_DEPTH);
	//////////////////////////////////////////////////////////////////////////

	// Apply world offset (Method 1 & Method 2)
	//m_camera->transform.position += world_offset; 

	// Check height of the map at that location
	Vector3 translation = movement + previousPosition;
	float height = m_map->GetHeight(translation.xz()) + 1.f;
	Vector3 newLocation = Vector3(translation.x, height , translation.z);

	Vector2 newPos = translation.xz();
	Vector3 forward = m_player->m_transform.GetWorldMatrix().GetForward();
	Vector3 right = m_player->m_transform.GetWorldMatrix().GetRight();

	DebugRenderLineSegment(0.f, newLocation, newLocation + forward * 5.f, DEBUG_RENDER_IGNORE_DEPTH, Rgba::YELLOW);
	DebugRenderLineSegment(0.f, newLocation, newLocation + right * 1.f, DEBUG_RENDER_IGNORE_DEPTH, Rgba::BLACK);

	Matrix44 newPlayerModel = m_map->GetAdjustedModelMatrix(newPos, forward, right);
	m_player->m_transform.SetLocalPosition(newLocation); 
	//m_player->m_transform.SetWorldMatrix(newPlayerModel);

}

Vector3 Playing::GetMovement()
{
	Vector3 result = Vector3::ZERO;
	float speed = 6.f;

	float dt = g_theGameClock->deltaTime;

	//Camera& currentCamera = *m_camera;
	Matrix44 playerMatrix = m_player->m_transform.GetLocalMatrix();
	Vector3 shipPos = m_player->m_transform.GetLocalPosition();

	//DebuggerPrintf("Cam Location: %f , %f \n", camPos.x, camPos.y);

	// Forward - Backwards
	if(IsKeyPressed(G_THE_LETTER_W))
	{
		result += (playerMatrix.GetForward() * speed * dt);
	
	}

	if(IsKeyPressed(G_THE_LETTER_S))
	{
		result -= (playerMatrix.GetForward() * speed * dt);

	}

	// Left and Right
	if(IsKeyPressed(G_THE_LETTER_D))
	{
		result += (playerMatrix.GetRight() * speed * dt);
	

	}

	if(IsKeyPressed(G_THE_LETTER_A))
	{
		result -= (playerMatrix.GetRight() * speed * dt);

	}
	

	// reset rotation
	//if(IsKeyPressed(G_THE_LETTER_E))
	//{
	//	if(m_currentShootTimer <= 0)
	//	{
	//		Shoot();
	//		m_currentShootTimer = m_shootcooldown;
	//	}
	//	else
	//	{
	//		m_currentShootTimer -= g_theGameClock->deltaTime;
	//	}
	//}

	return result;
}

void Playing::CheckWinLossStates()
{
	//--------------------------------------------------------------------------
	// Game over
	if(m_player->m_currentHealth <= 0)
		g_theGame->GoToDefeatState();

	//--------------------------------------------------------------------------
	// Game won
	if(m_enemies.size() == 0)
		if(m_enemySpawner.size() == 0)
			g_theGame->GoToVictoryState();
}

void Playing::RespawnPlayer()
{
	// resets hp and stats
	m_player->Reset();
	AddRenderable(m_player->m_renderable);

	// spawn in a safe area
}
