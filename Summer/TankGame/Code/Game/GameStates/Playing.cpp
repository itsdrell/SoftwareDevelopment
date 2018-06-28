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
#include "../GameSpecific/OrbitCamera.hpp"
#include "Engine/Core/Platform/Window.hpp"
#include "../GameSpecific/Projectile.hpp"
#include "Engine/Renderer/Images/Fonts/BitmapFont.hpp"

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

	AddWater(1.f);

	m_hud = new HUD();

	//---------------------------------------------------------
	// Cameras
	m_camera = new OrbitCamera();
	m_camera->CreateSkyBox("Data/Images/skybox.jpg");
	m_camera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_camera->SetDepthStencilTarget(g_theRenderer->m_defaultDepthTarget);
	m_camera->SetPerspective(45.f, (16.f/9.f), .1f , 1000.f);

	m_scene->AddCamera(m_camera);
	m_uiScene->AddCamera(g_theRenderer->m_defaultUICamera);

	g_theRenderer->SetCamera();
	//---------------------------------------------------------

	g_theRenderer->SetAmbientLight(.3f, Rgba::WHITE);

	m_sun = new DirectionalLight(0, Vector3(0.f, 100.f, 0.f), Vector3::DOWN, .6f);
	m_scene->AddLight(m_sun);

}

void Playing::CleanUp()
{
	m_scene->m_renderables.clear();
	m_enemies.clear();
	m_enemySpawner.clear();
	m_projectiles.clear();

	AddRenderable(m_map->m_test);
	AddRenderable(m_hud->m_target);
}

void Playing::Enter()
{
	m_player = AddPlayer();
	m_testEnemy = AddEnemy(Vector3(20.f, 0.f, 0.f));
	m_testSpawner = AddEnemySpawner(Vector2(0.f, 20.f));
	m_cameraRotation = Vector2::ZERO;

	//PlayLoopingSound("bg");

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
	mb.AddCube(Vector3::ZERO, Vector3(1.f, 1.f, 2.f));
	mb.AddUVSphere(Vector3(0.f, 0.f, 4.f), .4f, 16, 16, Rgba::BLUE);
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
	enemyMaterial->SetTint(GetRandomColorInRainbow());

	newSpawner->m_renderable->SetMaterial( enemyMaterial );
	newSpawner->m_renderable->m_usesLight = true;

	// Add to the containers
	m_scene->AddRenderable(newSpawner->m_renderable);
	m_enemySpawner.push_back(newSpawner);

	return newSpawner;
}

void Playing::AddWater(float height)
{
	Renderable* water = new Renderable();

	Material* waterMaterial = Material::CreateOrGetMaterial("water");
	water->SetMaterial(waterMaterial);

	MeshBuilder mb;
	mb.AddFlatPlane(Vector3(0.f, height, 0.f), AABB2(-1000.f, 1000.f), Rgba(255,255,255,100), AABB2(0, 128));
	water->SetMesh(mb.CreateMesh<VertexLit>());

	m_scene->AddRenderable(water);
}

void Playing::Update()
{
	CheckKeyBoardInputs();
	
	m_player->Update();

	for(uint i = 0; i < m_enemies.size(); i++)
	{
		m_enemies.at(i)->Update();
	}

	for(uint j = 0; j < m_enemySpawner.size(); j++)
	{
		m_enemySpawner.at(j)->Update();
	}

	for(uint p = 0; p < m_projectiles.size(); p++)
	{
		Projectile*& currentProjectile = m_projectiles.at(p);

		currentProjectile->Update();
	}

	CheckCollisions();
	RemoveTheDead();

	m_hud->Update();
}

void Playing::CheckCollisions()
{
	EnemyVsBullet();
	TowerVsBullet();
	EnemyVsPlayer();
	BulletVsTerrain();
}

void Playing::EnemyVsBullet()
{
	for(uint enemyIndex = 0; enemyIndex < m_enemies.size(); enemyIndex++)
	{
		Enemy*& currentEnemy = m_enemies.at(enemyIndex);

		if(currentEnemy->m_isDead == false)
		{
			for(uint bulletIndex = 0; bulletIndex < m_projectiles.size(); bulletIndex++)
			{
				Projectile*& currentProjectile = m_projectiles.at(bulletIndex);

				if(currentProjectile->m_isDead == false)
				{
					float radiusRange = currentEnemy->m_radius + currentProjectile->m_radius;
					float dis = (currentEnemy->m_transform.GetWorldPosition() - currentProjectile->m_transform.GetWorldPosition()).GetLength();

					if(dis < radiusRange)
					{
						currentEnemy->m_isDead = true;
						currentProjectile->m_isDead = true;
					}
				}

			}
		}
	}
}

void Playing::TowerVsBullet()
{
	for(uint towerIndex = 0; towerIndex < m_enemySpawner.size(); towerIndex++)
	{
		EnemySpawner*& currentTower = m_enemySpawner.at(towerIndex);

		if(currentTower->m_isDead == false)
		{
			for(uint bulletIndex = 0; bulletIndex < m_projectiles.size(); bulletIndex++)
			{
				Projectile*& currentProjectile = m_projectiles.at(bulletIndex);

				if(currentProjectile->m_isDead == false)
				{
					// bounds is in local space so we have to move it to world
					AABB3 towerBounds = currentTower->m_renderable->m_mesh->m_bounds;
					towerBounds.Translate(currentTower->m_transform.GetWorldMatrix());
					Vector3 bulletPos = currentProjectile->m_transform.GetWorldPosition();

					if(towerBounds.IsPointInside(bulletPos))
					{
						currentTower->m_health--;

						if(currentTower->m_health <= 0)
						{
							currentTower->m_isDead = true;
							currentProjectile->m_isDead = true;
						}
						else
						{
							currentProjectile->m_isDead = true;
						}
					}
				}

			}
		}
	}
}

void Playing::EnemyVsPlayer()
{
	for(uint enemyIndex = 0; enemyIndex < m_enemies.size(); enemyIndex++)
	{
		Enemy*& currentEnemy = m_enemies.at(enemyIndex);

		if(currentEnemy->m_isDead == false)
		{
			float radiusRange = currentEnemy->m_radius + 1.f;
			float dis = (currentEnemy->m_transform.GetWorldPosition() - m_player->m_transform.GetWorldPosition()).GetLength();

			if(dis < radiusRange)
			{
				currentEnemy->m_isDead = true;
				m_player->TakeDamage();
			}
		}
	}
}

void Playing::BulletVsTerrain()
{
	for(uint bulletIndex = 0; bulletIndex < m_projectiles.size(); bulletIndex++)
	{
		Projectile*& currentProjectile = m_projectiles.at(bulletIndex);

		if(currentProjectile->m_isDead == false)
		{
			if(m_map->IsBelow(currentProjectile->m_transform.GetWorldPosition()))
			{
				currentProjectile->m_isDead = true;
			}
		}

	}
}

void Playing::RemoveTheDead()
{
	// this is gross cause time
	
	if(WasKeyJustPressed(G_THE_LETTER_X))
	{
		m_testSpawner->m_isDead = true;
	}
	
	//--------------------------------------------------------------------------
	uint size = m_enemies.size();
	for(uint i = 0; i < size; i++)
	{
		Enemy*& currentEnemy = m_enemies.at(i);

		if(currentEnemy->m_isDead)
		{
			RemoveRenderable(currentEnemy->m_renderable);
			RemoveRenderable(currentEnemy->m_eyes);

			size--;

			m_enemies.erase(m_enemies.begin() + i);
		}
	}

	//--------------------------------------------------------------------------
	size =  m_enemySpawner.size();
	for(uint j = 0; j < m_enemySpawner.size(); j++)
	{
		EnemySpawner*& currentSpawner = m_enemySpawner.at(j);

		if(currentSpawner->m_isDead)
		{
			RemoveRenderable(currentSpawner->m_renderable);

			size--;
			m_enemySpawner.erase(m_enemySpawner.begin() + j);

		}
	}

	//--------------------------------------------------------------------------
	size = m_projectiles.size();
	for(uint p = 0; p < m_projectiles.size(); p++)
	{
		Projectile*& currentProjectile = m_projectiles.at(p);

		if(currentProjectile->m_isDead)
		{
			RemoveRenderable(currentProjectile->m_renderable);

			size--;
			m_projectiles.erase(m_projectiles.begin() + p);

		}
	}
}

void Playing::Render() const
{
	m_renderingPath->Render(m_scene);
	m_renderingPath->Render(m_uiScene);
}

void Playing::CheckKeyBoardInputs()
{
	if(IsDevConsoleOpen())
		return;

	// movement
	PlayerInput();


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

	float rotationSpeed = 100.0f;

	//// Apply Rotation
	Vector2 mouse_delta = g_theInput->GetMouseDelta();
	mouse_delta = mouse_delta.GetNormalized();
	
	float xRotation = 0;
	float yRotation = 0;

	if(mouse_delta.x > 0)
	{
		xRotation = -rotationSpeed * dt; 
	}
	if(mouse_delta.x < 0)
	{
		xRotation = rotationSpeed * dt;
	}

	if(mouse_delta.y > 0)
	{
		yRotation = -rotationSpeed * dt; 
	}
	if(mouse_delta.y < 0)
	{
		yRotation = rotationSpeed * dt;
	}


	m_cameraRotation.x += xRotation;
	m_cameraRotation.y += yRotation;

	//m_cameraRotation.x = ClampFloat( m_cameraRotation.x, -90.f, 90.f );
	m_cameraRotation.y = ClampFloat(m_cameraRotation.y, -20.f, 40.f);

	m_camera->SetTarget(m_player->m_transform.GetLocalPosition() + (Vector3::UP * 4.f));
	m_camera->SetSphericalCoordinate(10.f, m_cameraRotation.x, m_cameraRotation.y); 

}

void Playing::PlayerInput()
{
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

	Vector2 newPos = newLocation.xz();
	Vector3 forward = m_player->m_transform.GetWorldMatrix().GetForward();
	Vector3 right = m_player->m_transform.GetWorldMatrix().GetRight();

	//DebugRenderLineSegment(0.f, newLocation, newLocation + forward * 1.f, DEBUG_RENDER_IGNORE_DEPTH, Rgba::BLUE);
	//DebugRenderLineSegment(0.f, newLocation, newLocation + right * 1.f, DEBUG_RENDER_IGNORE_DEPTH, Rgba::RED);
	//m_player->m_transform.SetLocalPosition(newLocation); 

	Matrix44 newPlayerModel = m_map->GetAdjustedModelMatrix(newPos, forward, right);
	//DebugRenderBasis(0.f, newPlayerModel, 5.f);
	
	m_player->m_transform.SetWorldMatrix(newPlayerModel);

}

Vector3 Playing::GetMovement()
{
	Vector3 result = Vector3::ZERO;
	float speed = 6.f;

	float dt = g_theGameClock->deltaTime;

	Matrix44 playerMatrix = m_player->m_transform.GetLocalMatrix();
	Vector3 shipPos = m_player->m_transform.GetLocalPosition();

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
	

	//--------------------------------------------------------------------------
	// Rotation

	float currentYRotation = m_player->m_transform.GetLocalEulerAngles().y;
	float amount = (200.f * dt);
	DebugRenderLog(0.f, std::to_string(currentYRotation));
	
	if(IsKeyPressed(G_THE_LETTER_E))
	{
		m_player->m_transform.RotateLocalByEuler(Vector3(0.f, -amount, 0.f));
	}

	if(IsKeyPressed(G_THE_LETTER_Q))
	{
		m_player->m_transform.RotateLocalByEuler(Vector3(0.f,amount, 0.f));
	}

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
		{
			CleanUp();
			g_theGame->GoToVictoryState();
		}
}

void Playing::RespawnPlayer()
{
	// resets hp and stats
	m_player->Reset();
	AddRenderable(m_player->m_renderable);

	// spawn in a safe area
}
