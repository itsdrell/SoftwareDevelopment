#pragma warning(disable: 4100) // for commands
#include "Game/Game.hpp"
#include "Game/TheApp.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/GameCommon.hpp"
#include <vector>
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Command.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Core/Camera.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Lights.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/Renderer/TextureCube.hpp"


//////////////////////////////////////////////////////////////////////////

Game* g_theGame = nullptr;

//////////////////////////////////////////////////////////////////////////
// Command Tests (no console)
void QuitGame(Command& thecommand)
{
	g_theApp->m_isQuitting = true;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor so lets make what we need
Game::Game()
{
	m_console = new DevConsole(g_theRenderer);
	g_theGameClock = new Clock(g_theMasterClock);

	CommandRegister("quit","Type: quit","Exits Application", QuitGame);
}

void Game::StartUp()
{

	// Create the global sprite sheet for all the textures to use
	Texture* TileTexture = g_theRenderer->CreateOrGetTexture("Data/Images/Terrain_32x32.png");
	g_tileSpriteSheet = SpriteSheet(TileTexture,32,32);


	//////////////////////////////////////////////////////////////////////////
	m_skyBox = new TextureCube();
	m_skyBox->make_from_image("Data/Images/galaxy2.png");


	//////////////////////////////////////////////////////////////////////////
	// For Test Scene
	m_scene = new Scene("Test");
	m_renderingPath = new ForwardRenderingPath();


	//////////////////////////////////////////////////////////////////////////
	MeshBuilder mb;
	mb.AddCube(Vector3::ZERO, Vector3::ONE);
	m_skyMesh = mb.CreateMesh<Vertex3D_PCU>();

	//////////////////////////////////////////////////////////////////////////
	// Cameras
	m_camera = new Camera();
	m_camera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_camera->SetDepthStencilTarget(g_theRenderer->m_defaultDepthTarget);
	

	m_scene->AddCamera(m_camera);


	g_theRenderer->SetCamera();
	//////////////////////////////////////////////////////////////////////////

	g_theRenderer->SetAmbientLight(.1f, Rgba::WHITE);

	
	m_console->StartUp();
	
}


void Game::Update(float deltaSeconds)
{
	
	m_console->Update(deltaSeconds); // using engine clock?

	CheckKeyBoardInputs();
	
	DebugRenderGrid(0.f, Vector3(0.f,-20.f,0.f), 40.f,40.f);


	
}

void Game::ClockDebug()
{
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

void Game::Render() const
{
	//////////////////////////////////////////////////////////////////////////
	// Set up Cameras
	//m_camera->SetProjectionOrtho(10, 10, -10.0f, 20.0f);
	m_camera->SetPerspective(45.f, (16.f/9.f), .1f , 100.f);
	
	//Matrix44 modelMatrix = Matrix44::LookAt(
	//	m_ship->m_behindTransform.GetWorldPosition(), 
	//	m_ship->m_frontTransform.GetWorldPosition() , 
	//	m_ship->m_transform.GetLocalUp()); 
	
	m_camera->m_cameraMatrix = Matrix44();//modelMatrix;
	m_camera->m_viewMatrix = InvertFast(Matrix44()); // model); // inverse this 


	// Set the camera
	g_theRenderer->SetCamera(m_camera);

	// sky box
	g_theRenderer->ClearDepth(1.f);
	g_theRenderer->SetShader(Shader::CreateOrGetShader("Data/Shaders/skybox.shader"));
	g_theRenderer->SetUniform("MODEL", Matrix44());//modelMatrix);
	//g_theRenderer->SetCurrentTexture(0, m_sky);
	g_theRenderer->SetCurrentCubeMapTexture(m_skyBox, 0);
	g_theRenderer->DrawMesh(m_skyMesh);
	g_theRenderer->ClearDepth(1.f);
	g_theRenderer->SetShader();
	g_theRenderer->SetCurrentTexture();


	//////////////////////////////////////////////////////////////////////////

	m_renderingPath->Render(m_scene);


	//////////////////////////////////////////////////////////////////////////
	// Show the console, we return instantly if its not open
	m_console->Render();
	
}


void Game::CheckKeyBoardInputs()
{
	// dont check for input unless its not open
	if(IsDevConsoleOpen()) return;
	
	if(WasKeyJustPressed(G_TILDE))
	{
		m_console->Toggle(); // Turns off or on the Console
	}


	//////////////////////////////////////////////////////////////////////

	CameraInput();
	ClockDebug();
	
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

	Vector3 currentRotation = Vector3::ZERO; //->m_transform.GetLocalEulerAngles();
	currentRotation.x += local_euler.x; 
	currentRotation.y += local_euler.y; 

	currentRotation.x = ClampFloat( currentRotation.x, -90.f, 90.f );
	currentRotation.y = fmod(currentRotation.y, 360.f ); 

	//m_ship->m_transform.SetLocalRotationEuler( currentRotation);

	Vector3 movement = GetMovement();


	//////////////////////////////////////////////////////////////////////////
	// Trail
	//Vector3 previousPosition = m_ship->m_transform.GetLocalPosition();
	//Vector3 newPosition = m_camera->transform.GetLocalPosition() + movement;
	//DebugRenderLineSegment(5.f,previousPosition,newPosition,DEBUG_RENDER_USE_DEPTH);
	//////////////////////////////////////////////////////////////////////////

	// Apply world offset (Method 1 & Method 2)
	//m_camera->transform.position += world_offset; 
	//m_ship->m_transform.SetLocalPosition(movement + previousPosition); 



}

Vector3 Game::GetMovement()
{
	Vector3 result = Vector3::ZERO;
	//float speed = 2.f;
	
	//float dt = g_theGameClock->deltaTime;

	//Camera& currentCamera = *m_camera;
	//Matrix44 shipMatrix = m_ship->m_transform.GetLocalMatrix();
	//Vector3 shipPos = m_ship->m_transform.GetLocalPosition();
	
	//DebuggerPrintf("Cam Location: %f , %f \n", camPos.x, camPos.y);
	
	// Forward - Backwards
	if(IsKeyPressed(G_THE_LETTER_W))
	{
		//result += (shipMatrix.GetForward() * speed * dt);
		//m_ship->CreateParticleTrail();
	}

	if(IsKeyPressed(G_THE_LETTER_S))
	{
		//result -= (shipMatrix.GetForward() * speed * dt);
		//m_ship->CreateParticleTrail();


	}

	// Left and Right
	if(IsKeyPressed(G_THE_LETTER_D))
	{
		//result += (shipMatrix.GetRight() * speed * dt);
		//m_ship->CreateParticleTrail();

	}

	if(IsKeyPressed(G_THE_LETTER_A))
	{
		//result -= (shipMatrix.GetRight() * speed * dt);
		//m_ship->CreateParticleTrail();

	}

	// Up and Down
	if(IsKeyPressed(KEYBOARD_SPACE)) // up
	{
		//result += (shipMatrix.GetUp() * speed * dt);
		//m_ship->CreateParticleTrail();

	}

	if(IsKeyPressed(KEYBOARD_SHIFT))
	{
		//result -= (shipMatrix.GetUp() * speed * dt);
		//m_ship->CreateParticleTrail();

	}

	// reset rotation
	if(IsKeyPressed(G_THE_LETTER_E))
	{
// 		if(m_currentShootTimer <= 0)
// 		{
// 			Shoot();
// 			m_currentShootTimer = m_shootcooldown;
// 		}
// 		else
// 		{
// 			m_currentShootTimer -= g_theGameClock->deltaTime;
// 		}
	}

	return result;
}

