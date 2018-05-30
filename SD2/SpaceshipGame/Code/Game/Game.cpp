#pragma warning(disable: 4100) // for commands
#include "Game/Game.hpp"
#include "Game/TheApp.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"
#include "Engine/Renderer/Image.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/GameCommon.hpp"
#include <vector>
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "ThirdParty/tinyxml/tinyxml2.h"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Command.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Core/Camera.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Lights.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/Renderer/ParticleEmitter.hpp"
#include "Engine/Renderer/TextureCube.hpp"
#include "Game/Bullet.hpp"
#include "Asteroid.hpp"
#include "Game/Explosion.hpp"

//////////////////////////////////////////////////////////////////////////

Game* g_theGame = nullptr;

//////////////////////////////////////////////////////////////////////////
// Command Tests (no console)
void QuitGame(Command& thecommand)
{
	g_theApp->m_isQuitting = true;
}

void Weeb(Command& thecommand)
{
	//g_theGame->m_sky = g_theRenderer->CreateOrGetTexture("Data/Images/myheroes.png");
}


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor so lets make what we need
Game::Game()
{
	m_console = new DevConsole(g_theRenderer);
	g_theGameClock = new Clock(g_theMasterClock);

	CommandRegister("quit","Type: quit","Exits Application", QuitGame);
	//CommandRegister("ForsethDontLook", "", "", Weeb);
}

void Game::StartUp()
{

	// Create the global sprite sheet for all the textures to use
	Texture* TileTexture = g_theRenderer->CreateOrGetTexture("Data/Images/Terrain_32x32.png");
	g_tileSpriteSheet = SpriteSheet(TileTexture,32,32);


	//////////////////////////////////////////////////////////////////////////
	//m_sky = g_theRenderer->CreateOrGetTexture("Data/Images/galaxy2.png");
	m_skyBox = new TextureCube();
	m_skyBox->make_from_image("Data/Images/galaxy2.png");
	//m_skyBox->make_from_image("Data/Images/skybox.jpg");


	//////////////////////////////////////////////////////////////////////////
	// For Test Scene
	m_scene = new Scene("Test");
	m_renderingPath = new ForwardRenderingPath();

	// Ship
	m_ship = AddShip();
	m_shootcooldown = .1f;
	m_currentShootTimer = m_shootcooldown;

	//////////////////////////////////////////////////////////////////////////
	MeshBuilder mb;
	//mb.AddSkyBox();
	mb.AddCube(Vector3::ZERO, Vector3::ONE);
	m_skyMesh = mb.CreateMesh<Vertex3D_PCU>();

	//////////////////////////////////////////////////////////////////////////
	// Cameras
	m_camera = new Camera();
	m_camera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_camera->SetDepthStencilTarget(g_theRenderer->m_defaultDepthTarget);
	
	//m_miniMapCamera = new Camera();
	//m_miniMapTexture = g_theRenderer->CreateRenderTarget((int)Window::GetInstance()->GetWidth(), (int)Window::GetInstance()->GetHeight());
	//m_miniMapCamera->SetColorTarget( m_miniMapTexture );
	

	m_scene->AddCamera(m_camera);
	//m_scene->AddCamera(m_miniMapCamera);

	g_theRenderer->SetCamera();
	//////////////////////////////////////////////////////////////////////////

	g_theRenderer->SetAmbientLight(.1f, Rgba::WHITE);
	SetUpLights();


	AddAsteroid(Vector3(0.f, 1.f, 20.f));
	AddAsteroid(Vector3(20.f, 1.f, 0.f));
	AddAsteroid(Vector3(0.f, 20.f, 20.f));
	AddAsteroid(Vector3(-20.f, 1.f, 0.f));

	m_score = 0;

	//m_testParticle = new ParticleEmitter(Vector3::ZERO);
	//m_scene->AddRenderable(m_testParticle->m_renderable);
	
	m_console->StartUp();
	
}


void Game::Update(float deltaSeconds)
{
	
	m_console->Update(deltaSeconds); // using engine clock?

	CheckKeyBoardInputs();
	
	DebugRenderGrid(0.f, Vector3(0.f,-20.f,0.f), 40.f,40.f);

	m_ship->Update();

	BulletUpdate();
	AsteroidUpdate();
	ExplosionUpdate();

	CheckForAsteroidBulletCollision();

	DebugRender2DText(0.f, Vector2(-3.f, 38.f), "Score: " + std::to_string(m_score), 10.f, .1f);

	//m_testParticle->update(m_camera, deltaSeconds);
	
}

void Game::BulletUpdate()
{
	
	uint bulletSize = (uint) m_bullets.size();
	
	for(uint i = 0; i < bulletSize; i++)
	{
		Bullet*& currentBullet = m_bullets.at(i);

		if(currentBullet->m_isDead == true)
		{
			// make sure they are removed from scene
			RemoveRenderable(currentBullet->m_renderable);
			RemoveLight(currentBullet->m_light);
			
			m_bullets.erase(m_bullets.begin() + i, m_bullets.begin() + (i + 1));
			bulletSize--;
			i--;
		}
		else
		{
			m_bullets.at(i)->Update();
		}
		
	}

	//DebugRenderLog(0.f, "lights" + std::to_string(m_scene->m_lights.size()));
	//DebugRenderLog(0.f, "renderables" + std::to_string(m_scene->m_renderables.size()));
	//DebugRenderLog(0.f, std::to_string(bulletSize));
}

void Game::AsteroidUpdate()
{

	uint asteroidSize =  (uint) m_asteroids.size();

	for(uint i = 0; i < asteroidSize; i++)
	{
		Asteroid*& currentAsteroid = m_asteroids.at(i);

		if(currentAsteroid->m_isDead == true)
		{
			// make sure they are removed from scene
			RemoveRenderable(currentAsteroid->m_renderable);

			m_asteroids.erase(m_asteroids.begin() + i, m_asteroids.begin() + (i + 1));
			asteroidSize--;
			i--;
		}
		else
		{
			m_asteroids.at(i)->Update();
		}

	}
}

void Game::ExplosionUpdate()
{
	uint explosionSize =  (uint) m_explosions.size();

	for(uint i = 0; i < explosionSize; i++)
	{
		Explosion*& currentExplosion = m_explosions.at(i);

		if(currentExplosion->m_isDead == true)
		{
			// make sure they are removed from scene
			RemoveRenderable(currentExplosion->m_emitter->m_renderable);

			m_explosions.erase(m_explosions.begin() + i, m_explosions.begin() + (i + 1));
			explosionSize--;
			i--;
		}
		else
		{
			m_explosions.at(i)->Update();
		}

	}

	
	if(WasKeyJustPressed(G_THE_LETTER_P))
	{
		AddEXPLOSION(Vector3::ZERO);
	}
}

void Game::Render() const
{
	//////////////////////////////////////////////////////////////////////////
	// Set up Cameras
	//m_camera->SetProjectionOrtho(10, 10, -10.0f, 20.0f);
	m_camera->SetPerspective(45.f, (16.f/9.f), .1f , 100.f);
	
	Matrix44 modelMatrix = Matrix44::LookAt(
		m_ship->m_behindTransform.GetWorldPosition(), 
		m_ship->m_frontTransform.GetWorldPosition() , 
		m_ship->m_transform.GetLocalUp()); 
	
	m_camera->m_cameraMatrix = modelMatrix;
	m_camera->m_viewMatrix = InvertFast(modelMatrix); // inverse this 


	//m_miniMapCamera->SetPerspective(45.f, (16.f / 9.f), .1, 200.f);
	//Matrix44 miniMapModel = Matrix44:: LookAt(
	//	m_ship->m_miniMapTransform.GetWorldPosition(),
	//	m_ship->m_transform.GetWorldPosition(),
	//	-Vector3::UP);
	//
	//m_miniMapCamera->m_cameraMatrix = miniMapModel;
	//m_miniMapCamera->m_viewMatrix = InvertFast(miniMapModel); // inverse this 


	// Set the camera
	g_theRenderer->SetCamera(m_camera);
	//DebugRenderSet3DCamera(m_camera);

	// sky box
	g_theRenderer->ClearDepth(1.f);
	g_theRenderer->SetShader(Shader::CreateOrGetShader("Data/Shaders/skybox.shader"));
	g_theRenderer->SetUniform("MODEL", modelMatrix);
	//g_theRenderer->SetCurrentTexture(0, m_sky);
	g_theRenderer->SetCurrentCubeMapTexture(m_skyBox, 0);
	g_theRenderer->DrawMesh(m_skyMesh);
	g_theRenderer->ClearDepth(1.f);
	g_theRenderer->SetShader();
	g_theRenderer->SetCurrentTexture();


	//////////////////////////////////////////////////////////////////////////

	m_renderingPath->Render(m_scene);



	//////////////////////////////////////////////////////////////////////////
	// MINI MAP BOIS
	
	//g_theRenderer->ClearDepth(1.f);
	//g_theRenderer->SetCamera(g_theRenderer->m_defaultUICamera);
	//g_theRenderer->SetCurrentTexture();
	//g_theRenderer->DrawAABB2(AABB2(-50.f, -50.f, -30.f, -10.f), Rgba::WHITE);
	//g_theRenderer->SetCurrentTexture();
	//g_theRenderer->SetCurrentTexture(0, m_miniMapCamera->m_output.m_color_target);
	//g_theRenderer->DrawAABB2(AABB2(-49.f, -49.f, -31.f, -11.f), Rgba::WHITE);
	//g_theRenderer->SetCamera();

	//////////////////////////////////////////////////////////////////////////
	// Show the console, we return instantly if its not open
	m_console->Render();
	
}

void Game::SetUpLights()
{
	
	//////////////////////////////////////////////////////////////////////////
	// Set Uniforms
	//g_theRenderer->SetUniform("EYE_POSITION", m_camera->m_cameraMatrix.GetPosition());
	//g_theRenderer->SetAmbientLight(m_ambience.w, Rgba::WHITE);
	//g_theRenderer->SetSpecularConstants(1.f, 16.f);


	//////////////////////////////////////////////////////////////////////////
	// Setup all lights
	//float x = 4.f;
	//float z = 4.f;

	//// The Lights around the ship
	//Light* centerLight = new PointLight(0, Vector3(0.f,0.f,-2.f), 10.f);
	//m_lights.push_back(centerLight);

	//Light* newLight =  new PointLight(1, Vector3(0.f,0.f,2.f), 10.f); 
	//m_lights.push_back(newLight);
	//
	//newLight =  new PointLight(2, Vector3(-x,0.f,z), 10.f); 
	//m_lights.push_back(newLight);
	//
	//newLight =  new PointLight(3, Vector3(-x,0.f,0.f), 10.f); 
	//m_lights.push_back(newLight);
	//
	//// these are the ones that should be hidden
	//newLight =  new PointLight(4, Vector3(-x,0.f,-z), 0.f,  Vector3(0.f,0.f,1.f), Rgba::RED); 
	//m_lights.push_back(newLight);
	//
	//newLight =  new PointLight(5, Vector3(x,0.f,z), 0.f,  Vector3(0.f,0.f,1.f), Rgba::RED); 
	//m_lights.push_back(newLight);
	//
	//newLight =  new PointLight(6, Vector3(x,0.f,0.f), 0.f, Vector3(0.f,0.f,1.f), Rgba::RED); 
	//m_lights.push_back(newLight);
	//
	//newLight =  new PointLight(7, Vector3(x,0.f,-z), 0.f, Vector3(0.f,0.f,1.f), Rgba::RED); 
	//m_lights.push_back(newLight);
	//
	////////////////////////////////////////////////////////////////////////////
	//// The Lights away from the ship
	//float r = 8.f;
	//
	//newLight =  new DirectionalLight(8, Vector3(-r,0.f,0.f), Vector3::RIGHT, 10.f); 
	//m_lights.push_back(newLight);
	//
	//newLight =  new DirectionalLight(9, Vector3(0.f,0.f,r), -Vector3::FORWARD, 10.f); 
	//m_lights.push_back(newLight);
	//
	Light* newLight =  new DirectionalLight(10, Vector3::ZERO, Vector3::FORWARD, .3f); 
	m_lights.push_back(newLight);
	//
	//newLight =  new DirectionalLight(11, Vector3(0.f,0.f,-r), Vector3::FORWARD, 10.f); 
	//m_lights.push_back(newLight);
	

	//////////////////////////////////////////////////////////////////////////
	// Add them all to the scene
	for(uint i = 0; i < m_lights.size(); i++)
	{
		m_scene->AddLight(m_lights.at(i));
	}
}

void Game::CheckKeyBoardInputs()
{
	// dont check for input unless its not open
	if(IsDevConsoleOpen()) return;
	
	if(WasKeyJustPressed(G_TILDE))
	{
		m_console->Toggle(); // Turns off or on the Console
	}

	if(WasKeyJustPressed(G_THE_LETTER_T))
	{
		DebugRenderLog(1.f);
	}

	//////////////////////////////////////////////////////////////////////

	CameraInput();

	//////////////////////////////////////////////////////////////////////////
	// this is gonna be used for clock debug
	if(g_theMasterClock->m_debugMode == true)
	{
		if(WasKeyJustPressed(G_THE_LETTER_L))
		{
			g_theMasterClock->IncreaseScale();
		}

		if(WasKeyJustPressed(G_THE_LETTER_K))
		{
			g_theMasterClock->DescreaseScale();
		}

		if(WasKeyJustPressed(G_THE_LETTER_P))
		{
			g_theMasterClock->TogglePaused();
			g_theMasterClock->SetScale(0.f);
			DevConsole::AddConsoleDialogue(ConsoleDialogue("TIME HAS BEEN TOGGLED AND SCALE RESET", Rgba::YELLOW));
		}

		if(WasKeyJustPressed(KEYBOARD_BACKSPACE))
		{
			g_theMasterClock->m_debugMode = false;
			DevConsole::AddConsoleDialogue(ConsoleDialogue("DEBUG MODE DISABLED", Rgba::YELLOW));
		}

		if(WasKeyJustPressed(G_THE_LETTER_O))
		{
			g_theMasterClock->StepForwardAFrame();
		}
	}

	
}

void Game::CameraInput()
{
	float dt = g_theGameClock->deltaTime; // this needs to be after keyboard because we might fuck with ds for go to next frames

	float rotationSpeed = 2.0f;

	// Apply Rotation
	Vector2 mouse_delta = g_theInput->GetMouseDelta();
	//mouse_delta = mouse_delta.GetNormalized();

	// m_current_cam_euler; // defined, starts at zero
	Vector3 local_euler = Vector3::ZERO; 
	local_euler.y = mouse_delta.x * rotationSpeed * dt; 
	local_euler.x = mouse_delta.y * rotationSpeed * dt; 

	Vector3 currentRotation = m_ship->m_transform.GetLocalEulerAngles();
	currentRotation.x += local_euler.x; 
	currentRotation.y += local_euler.y; 

	currentRotation.x = ClampFloat( currentRotation.x, -90.f, 90.f );
	currentRotation.y = fmod(currentRotation.y, 360.f ); 

	m_ship->m_transform.SetLocalRotationEuler( currentRotation);

	Vector3 movement = GetMovement();
	

	// Method 1
	//Vector3 cam_forward = m_camera->GetForward(); 
	//Vector3 cam_up = m_camera->GetUp(); 
	//Vector3 cam_right = m_camera->GetRight(); 

	//Vector3 world_offset = GetWorldPositionFromACamerasDirections(movement,cam_up,cam_right, cam_forward);


	//////////////////////////////////////////////////////////////////////////
	// Trail
	Vector3 previousPosition = m_ship->m_transform.GetLocalPosition();
	//Vector3 newPosition = m_camera->transform.GetLocalPosition() + movement;
	//DebugRenderLineSegment(5.f,previousPosition,newPosition,DEBUG_RENDER_USE_DEPTH);
	//////////////////////////////////////////////////////////////////////////

	// Apply world offset (Method 1 & Method 2)
	//m_camera->transform.position += world_offset; 
	m_ship->m_transform.SetLocalPosition(movement + previousPosition); 



}

Vector3 Game::GetMovement()
{
	Vector3 result = Vector3::ZERO;
	float speed = 2.f;
	
	float dt = g_theGameClock->deltaTime;

	//Camera& currentCamera = *m_camera;
	Matrix44 shipMatrix = m_ship->m_transform.GetLocalMatrix();
	Vector3 shipPos = m_ship->m_transform.GetLocalPosition();
	
	//DebuggerPrintf("Cam Location: %f , %f \n", camPos.x, camPos.y);
	
	// Forward - Backwards
	if(IsKeyPressed(G_THE_LETTER_W))
	{
		result += (shipMatrix.GetForward() * speed * dt);
		m_ship->CreateParticleTrail();
	}

	if(IsKeyPressed(G_THE_LETTER_S))
	{
		result -= (shipMatrix.GetForward() * speed * dt);
		m_ship->CreateParticleTrail();


	}

	// Left and Right
	if(IsKeyPressed(G_THE_LETTER_D))
	{
		result += (shipMatrix.GetRight() * speed * dt);
		m_ship->CreateParticleTrail();

	}

	if(IsKeyPressed(G_THE_LETTER_A))
	{
		result -= (shipMatrix.GetRight() * speed * dt);
		m_ship->CreateParticleTrail();

	}

	// Up and Down
	if(IsKeyPressed(KEYBOARD_SPACE)) // up
	{
		result += (shipMatrix.GetUp() * speed * dt);
		m_ship->CreateParticleTrail();

	}

	if(IsKeyPressed(KEYBOARD_SHIFT))
	{
		result -= (shipMatrix.GetUp() * speed * dt);
		m_ship->CreateParticleTrail();

	}

	// reset rotation
	if(IsKeyPressed(G_THE_LETTER_E))
	{
		if(m_currentShootTimer <= 0)
		{
			Shoot();
			m_currentShootTimer = m_shootcooldown;
		}
		else
		{
			m_currentShootTimer -= g_theGameClock->deltaTime;
		}
	}

	return result;
}

Ship* Game::AddShip()
{
	Ship* ship = new Ship();

	// Mesh
	MeshBuilder mb;
	mb.AddMeshFromObjFile("Data/Model/scifi_fighter_mk6/scifi_fighter_mk6.obj");
	Mesh* shipMesh =  mb.CreateMesh<VertexLit>();
	ship->m_renderable->SetMesh(shipMesh);

	Material* shipMaterial = Material::CreateOrGetMaterial("ship");
	
	ship->m_renderable->m_usesLight = true;
	ship->m_renderable->SetMaterial(shipMaterial);

	// Add to scene
	m_scene->AddRenderable(ship->m_renderable);

	// Create particle trail
	ship->m_leftTrail = new ParticleEmitter(ship->m_transform.GetLocalPosition() + Vector3::LEFT);
	ship->m_rightTrail = new ParticleEmitter(ship->m_transform.GetLocalPosition() + Vector3::RIGHT);

	ship->m_leftTrail->m_randomVelocity = false;
	ship->m_leftTrail->m_force = Vector3(0.f,0.f, -9.8f);

	ship->m_rightTrail->m_randomVelocity = false;
	ship->m_rightTrail->m_force = Vector3(0.f,0.f, -9.8f);

	//////////////////////////////////////////////////////////////////////////
	m_scene->AddLight(ship->m_spotLight);
	m_scene->AddRenderable(ship->m_leftTrail->m_renderable);
	m_scene->AddRenderable(ship->m_rightTrail->m_renderable);

	return ship;
	
}

void Game::AddEXPLOSION(const Vector3& position)
{
	Explosion* newExplosion = new Explosion(position);

	m_scene->AddRenderable(newExplosion->m_emitter->m_renderable);

	m_explosions.push_back(newExplosion);

}

void Game::AddAsteroid(const Vector3& position)
{
	Asteroid* newAsteroid = new Asteroid(position, GetRandomFloat(4.f, 6.f));

	m_scene->AddRenderable(newAsteroid->m_renderable);

	m_asteroids.push_back(newAsteroid);
}

void Game::Shoot()
{
	//////////////////////////////////////////////////////////////////////////
	// Create a bullets
	
	//////////////////////////////////////////////////////////////////////////
	// Left
	Bullet* newBullet = new Bullet(m_ship->m_leftTransform.GetWorldMatrix());

	m_scene->AddLight(newBullet->m_light);
	m_scene->AddRenderable(newBullet->m_renderable);

	m_bullets.push_back(newBullet);

	//////////////////////////////////////////////////////////////////////////
	// Right
	newBullet = new Bullet(m_ship->m_rightTransform.GetWorldMatrix());

	m_scene->AddLight(newBullet->m_light);
	m_scene->AddRenderable(newBullet->m_renderable);

	m_bullets.push_back(newBullet);
}

void Game::CheckForAsteroidBulletCollision()
{
	// For spawning new one
	bool spawnNewOne = false;
	Vector3 newPos;
	
	for(uint a = 0; a < m_asteroids.size(); a++)
	{

		Asteroid*& currentAsteroid = m_asteroids.at(a);

		for(uint b = 0; b < m_bullets.size(); b++)
		{
			Bullet*& currentBullet = m_bullets.at(b);

			// check for collisions
			Vector3 bulletPos = currentBullet->m_transform.GetWorldPosition();
			Vector3 currentPos = currentAsteroid->m_transform.GetWorldPosition();
			float distance = GetDistance( bulletPos, currentPos);

			if(distance <= (currentAsteroid->m_radius))
			{
				AddEXPLOSION(currentBullet->m_transform.GetWorldPosition());

				m_score++;

				newPos = currentAsteroid->m_transform.GetLocalPosition() + Vector3(GetRandomFloat(-currentAsteroid->m_radius, currentAsteroid->m_radius));
				spawnNewOne = true;
				currentBullet->m_isDead = true;
				currentAsteroid->m_isDead = true;
			}
		}
	}


	if(spawnNewOne)
	{
		AddAsteroid(newPos);

	}
}
