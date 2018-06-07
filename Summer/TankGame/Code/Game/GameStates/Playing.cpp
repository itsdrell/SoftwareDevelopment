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

Playing::Playing()
{
}

void Playing::StartUp()
{
	// Create the global sprite sheet for all the textures to use
	Texture* TileTexture = g_theRenderer->CreateOrGetTexture("Data/Images/Terrain_32x32.png");
	g_tileSpriteSheet = SpriteSheet(TileTexture,32,32);


	//////////////////////////////////////////////////////////////////////////
	// For Test Scene
	m_scene = new Scene("Test");
	m_renderingPath = new ForwardRenderingPath();


	m_player = AddPlayer();

	m_map = new GameMap();
	m_map->LoadMap(AABB2(-128.f, 128.f), FloatRange(0.f, 6.f), IntVector2(16,16), 20.f);


	//////////////////////////////////////////////////////////////////////////
	// Cameras
	m_camera = new Camera();
	m_camera->CreateSkyBox("Data/Images/skybox.jpg");
	m_camera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_camera->SetDepthStencilTarget(g_theRenderer->m_defaultDepthTarget);


	m_scene->AddCamera(m_camera);


	g_theRenderer->SetCamera();
	//////////////////////////////////////////////////////////////////////////

	g_theRenderer->SetAmbientLight(.3f, Rgba::WHITE);

	m_sun = new DirectionalLight(0, Vector3(0.f, 100.f, 0.f), Vector3::DOWN, .6f);
	m_scene->AddLight(m_sun);

	DebugRender2DText(20.f, Vector2(20.f, 20.f), "GO", 10.f);
}

Player* Playing::AddPlayer()
{
	Player* newPlayer = new Player();

	MeshBuilder mb;
	mb.AddUVSphere(Vector3::ZERO, 1.f, 16, 16);
	newPlayer->m_renderable->SetMesh(mb.CreateMesh<VertexLit>());

	Material* playerMaterial = Material::CreateOrGetMaterial("ship");
	playerMaterial->SetTexture(0, g_theRenderer->m_defaultTexture);
	//playerMaterial->SetTexture(1, g_theRenderer->m_defaultTexture);
	//playerMaterial->SetTexture(2, g_theRenderer->m_defaultTexture);


	newPlayer->m_renderable->SetMaterial( playerMaterial );
	newPlayer->m_renderable->m_usesLight = true;

	m_scene->AddRenderable(newPlayer->m_renderable);

	return newPlayer;
}

void Playing::Update()
{
	m_player->Update();
	CheckKeyBoardInputs();

	//DebugRenderGrid(0.f, Vector3::ZERO, 20.f, 20.f);
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
}

void Playing::CheckKeyBoardInputs()
{
	if(IsDevConsoleOpen())
		return;

	CameraInput();
}

void Playing::CameraInput()
{
//	float dt = g_theGameClock->deltaTime; // this needs to be after keyboard because we might fuck with ds for go to next frames

	//float rotationSpeed = 2.0f;
	//
	//// Apply Rotation
	//Vector2 mouse_delta = g_theInput->GetMouseDelta();
	////mouse_delta = mouse_delta.GetNormalized();
	//
	//// m_current_cam_euler; // defined, starts at zero
	//Vector3 local_euler = Vector3::ZERO; 
	//local_euler.y = mouse_delta.x * rotationSpeed * dt; 
	//local_euler.x = mouse_delta.y * rotationSpeed * dt; 
	//
	//Vector3 currentRotation = m_player->m_transform.GetLocalEulerAngles();
	//currentRotation.x += local_euler.x; 
	//currentRotation.y += local_euler.y; 

	//currentRotation.x = ClampFloat( currentRotation.x, -90.f, 90.f );
	//currentRotation.y = fmod(currentRotation.y, 360.f ); 

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

	m_player->m_transform.SetLocalPosition(newLocation); 


}

Vector3 Playing::GetMovement()
{
	Vector3 result = Vector3::ZERO;
	float speed = 2.f;

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
