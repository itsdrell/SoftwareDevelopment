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
#include "Engine/Core/Tools/Command.hpp"
#include "Engine/Input/Mouse.hpp"
#include "Engine/Core/General/HeatMap.hpp"
#include "Engine/Renderer/Systems/Scene.hpp"
#include "Engine/Renderer/Systems/ForwardRenderingPath.hpp"
#include "Engine/Renderer/Systems/MeshBuilder.hpp"
#include "Engine/Renderer/Images/Textures/TextureCube.hpp"


//====================================================================================

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

}

void Playing::StartUp()
{

	//---------------------------------------------------------
	// For Test Scene
	m_scene = new Scene("Test");
	m_renderingPath = new ForwardRenderingPath();

	DebugRenderWireAABB3(1000.f, AABB3(Vector3(-4.f, -4.f, 8.f), Vector3(4.f, 4.f, 8.f)));

	m_skyBox = new TextureCube();
	m_skyBox->make_from_image("Data/Images/galaxy2.png");
	//m_skyBox->make_from_image("Data/Images/skybox.jpg");


	MeshBuilder mb;
	mb.AddCube(Vector3::ZERO, Vector3::ONE);
	m_skyMesh = mb.CreateMesh<Vertex3D_PCU>();

	//---------------------------------------------------------
	// Cameras
	m_camera = new Camera();
	m_camera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_camera->SetDepthStencilTarget(g_theRenderer->m_defaultDepthTarget);

	m_camera->LookAt(Vector3::ZERO, Vector3(0.f, 0.f, -10.f));

	m_scene->AddCamera(m_camera);

	g_theRenderer->SetCamera();

	//--------------------------------------------------------------------------
	// Game specific setup

	m_currentPlayState = SELECTING;

	m_cameraLocation = Vector2(-112,-112);

}

void Playing::Update()
{
	CheckKeyBoardInputs();

	MoveCamera();
}

void Playing::Render() const
{
	//////////////////////////////////////////////////////////////////////////
	// Set up Cameras
	m_camera->SetPerspective(45.f, (16.f/9.f), .1f , 100.f);

	//Matrix44 modelMatrix = Matrix44::LookAt(
	//	m_ship->m_behindTransform.GetWorldPosition(), 
	//	m_ship->m_frontTransform.GetWorldPosition() , 
	//	m_ship->m_transform.GetLocalUp()); 

	m_camera->m_cameraMatrix = m_camera->transform.GetLocalMatrix();//Matrix44(); //modelMatrix;
	m_camera->m_viewMatrix = InvertFast(m_camera->m_cameraMatrix/*modelMatrix*/); // inverse this 

	//////////////////////////////////////////////////////////////////////////

	RenderSkyBox();

	m_renderingPath->Render(m_scene);	
	
}

void Playing::RenderSkyBox() const
{
	g_theRenderer->SetCamera(m_camera);
	
	// sky box
	g_theRenderer->ClearDepth(1.f);
	g_theRenderer->SetShader(Shader::CreateOrGetShader("Data/Shaders/skybox.shader"));
	g_theRenderer->SetUniform("MODEL", m_camera->m_cameraMatrix);
	//g_theRenderer->SetCurrentTexture(0, m_sky);
	g_theRenderer->SetCurrentCubeMapTexture(m_skyBox, 0);
	g_theRenderer->DrawMesh(m_skyMesh);
	g_theRenderer->ClearDepth(1.f);
	g_theRenderer->SetShader();
	g_theRenderer->SetCurrentTexture();
}

void Playing::CheckKeyBoardInputs()
{
	// do this last cause itll move the mouse 
	MoveCamera();
}

void Playing::MoveCamera()
{
	//float dt = g_theGameClock->deltaTime; // this needs to be after keyboard because we might fuck with ds for go to next frames
	//
	//float rotationSpeed = .20f;
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
	//Vector3 currentRotation = m_camera->transform.GetLocalEulerAngles();
	//currentRotation.x += local_euler.x; 
	//currentRotation.y += local_euler.y; 
	//
	//currentRotation.x = ClampFloat( currentRotation.x, -90.f, 90.f );
	//currentRotation.y = fmod(currentRotation.y, 360.f ); 
	//
	//m_camera->transform.SetLocalRotationEuler(currentRotation);
	
	
	
	// movement
	
	Vector3 amountToMove = Vector3::ZERO;
	float ds = g_theGameClock->deltaTime;

	if(IsKeyPressed(G_THE_LETTER_W))
		amountToMove = Vector3::FORWARD;
	if(IsKeyPressed(G_THE_LETTER_S))
		amountToMove = -Vector3::FORWARD;
	if(IsKeyPressed(G_THE_LETTER_A))
		amountToMove = -Vector3::RIGHT;
	if(IsKeyPressed(G_THE_LETTER_D))
		amountToMove = Vector3::RIGHT;
	if(IsKeyPressed(KEYBOARD_SPACE))
		amountToMove = Vector3::UP;
	if(IsKeyPressed(KEYBOARD_SHIFT))
		amountToMove = Vector3::DOWN;

	if(amountToMove != Vector3::ZERO)
		m_camera->transform.TranslateLocal(amountToMove * ds * 10.f);
	//m_camera->transform.RotateLocalByEuler(Vector3::RIGHT * 10.f * g_theGameClock->deltaTime);
}



