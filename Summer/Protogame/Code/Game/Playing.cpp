#include "Playing.hpp"
#include "Engine\Renderer\Images/Textures/Texture.hpp"
#include "Engine\Renderer\Images/Sprites/SpriteSheet.hpp"
#include "Engine\Core\General/Camera.hpp"
#include "GameCommon.hpp"
#include "Engine\Core\Tools/Clock.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Core\Tools/DevConsole.hpp"

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


	//////////////////////////////////////////////////////////////////////////
	// Cameras
	m_camera = new Camera();
	m_camera->CreateSkyBox("Data/Images/galaxy2.png");
	m_camera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_camera->SetDepthStencilTarget(g_theRenderer->m_defaultDepthTarget);


	m_scene->AddCamera(m_camera);


	g_theRenderer->SetCamera();
	//////////////////////////////////////////////////////////////////////////

	g_theRenderer->SetAmbientLight(.1f, Rgba::WHITE);
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
	m_camera->SetPerspective(45.f, (16.f/9.f), .1f , 100.f);

	//Matrix44 modelMatrix = Matrix44::LookAt(
	//	m_ship->m_behindTransform.GetWorldPosition(), 
	//	m_ship->m_frontTransform.GetWorldPosition() , 
	//	m_ship->m_transform.GetLocalUp()); 

	m_camera->m_cameraMatrix = Matrix44();//modelMatrix;
	m_camera->m_viewMatrix = InvertFast(Matrix44()); // model); // inverse this 


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

Vector3 Playing::GetMovement()
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
