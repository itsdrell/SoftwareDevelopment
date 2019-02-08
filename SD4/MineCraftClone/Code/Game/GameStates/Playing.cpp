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
#include "Engine/Renderer/RenderableComponents/Renderable.hpp"
#include "Game/General/GameCamera.hpp"
#include "Game/General/World/Block.hpp"
#include "Game/General/World/BlockDefinition.hpp"
#include "Game/General/World/Chunk.hpp"


//====================================================================================

//====================================================================================
Playing::Playing()
{
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

	DebugRenderBasis(1000.f, Matrix44());

	m_skyBox = new TextureCube();
	m_skyBox->make_from_image("Data/Images/galaxy2.png");
	//m_skyBox->make_from_image("Data/Images/skybox.jpg");

	//-----------------------------------------------------------------------------------------------
	// Create some block defs
	new BlockDefinition("air", BLOCK_TYPE_AIR, IntVector2(), IntVector2(), IntVector2());
	new BlockDefinition("grass", BLOCK_TYPE_GRASS, IntVector2(2,0), IntVector2(3,3), IntVector2(4,3));
	new BlockDefinition("snow", BLOCK_TYPE_SNOW,  IntVector2(1,3), IntVector2(2,3), IntVector2(4,3));
	new BlockDefinition("stone", BLOCK_TYPE_STONE, IntVector2(7,4), IntVector2(7,4), IntVector2(7,4));
	new BlockDefinition("test", BLOCK_TYPE_TEST, IntVector2(31,31), IntVector2(31,31), IntVector2(31,31));


	m_testChunk = new Chunk(IntVector2(1,1));

	MeshBuilder mb;
	mb.AddCube(Vector3::ZERO, Vector3::ONE);
	m_skyMesh = mb.CreateMesh<Vertex3D_PCU>();

	//RenderTestCube();

	//---------------------------------------------------------
	// Cameras
	m_camera = new Camera();
	m_camera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_camera->SetDepthStencilTarget(g_theRenderer->m_defaultDepthTarget);

	m_camera->LookAt(Vector3::ZERO, Vector3(0.f, 0.f, -10.f));

	m_scene->AddCamera(m_camera);

	g_theRenderer->SetCamera();

	m_gameCamera = new GameCamera();

}

void Playing::Update()
{
	CheckKeyBoardInputs();
}

void Playing::Render() const
{
	Renderer* r = Renderer::GetInstance();
	
	//////////////////////////////////////////////////////////////////////////
	// Set up Cameras
	m_camera->SetPerspective(45.f, (16.f/9.f), .1f , 100.f);

	//m_camera->m_cameraMatrix = m_camera->transform.GetLocalMatrix();//Matrix44(); //modelMatrix;
	Matrix44 theModel = m_gameCamera->GetModelMatrix();
	Matrix44 theView = m_gameCamera->GetViewMatrix();
	m_camera->m_cameraMatrix = theModel; 
	m_camera->m_viewMatrix = theView;


	//////////////////////////////////////////////////////////////////////////

	RenderSkyBox();

	//m_renderingPath->Render(m_scene);	

	//RenderTestCube();

	r->SetShader(Shader::CreateOrGetShader("default"));
	r->SetCurrentTexture(0 , g_blockSpriteSheet.m_spriteSheetTexture);
	r->SetCamera(m_camera);
	r->DrawMesh(m_testChunk->m_gpuMesh);
	
}

//-----------------------------------------------------------------------------------------------
void Playing::RenderTestCube() const
{
	MeshBuilder mb;
	Vector3 center = Vector3(5.f, 0.f, 0.f);
	Vector3 dimensions = Vector3::ONE;
	Renderer* r = Renderer::GetInstance();

	BlockDefinition* theDefinition = BlockDefinition::GetDefinitionByName("snow");

	mb.Begin(PRIMITIVE_TRIANGLES, true);
	mb.SetColor(Rgba::WHITE);

	//-----------------------------------------------------------------------------------------------
	// Top
	// br
	AABB2 uvs = theDefinition->m_topUVs;
	
	mb.SetUV(uvs.maxs.x, uvs.mins.y);
	uint idx = mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y - dimensions.y),	(center.z + dimensions.z)));

	//bl
	mb.SetUV(uvs.mins.x, uvs.mins.y);
	mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y + dimensions.y),	(center.z + dimensions.z)));

	// tl
	mb.SetUV(uvs.mins.x,uvs.maxs.y);
	mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y + dimensions.y),	(center.z + dimensions.z)));

	// tr
	mb.SetUV(uvs.maxs.x, uvs.maxs.y);
	mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y - dimensions.y),	(center.z + dimensions.z)));

	mb.AddFace(idx + 0, idx + 1, idx + 2);
	mb.AddFace(idx + 2, idx + 3, idx + 0);

	//-----------------------------------------------------------------------------------------------
	// Front face
	uvs = theDefinition->m_sideUVs;

	mb.SetUV(uvs.maxs.x, uvs.mins.y);
	idx = mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y - dimensions.y),	(center.z - dimensions.z)));

	//bl
	mb.SetUV(uvs.mins.x, uvs.mins.y);
	mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y + dimensions.y),	(center.z - dimensions.z)));

	// tl
	mb.SetUV(uvs.mins.x,uvs.maxs.y);
	mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y + dimensions.y),	(center.z + dimensions.z)));

	// tr
	mb.SetUV(uvs.maxs.x, uvs.maxs.y);
	mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y - dimensions.y),	(center.z + dimensions.z)));

	mb.AddFace(idx + 0, idx + 1, idx + 2);
	mb.AddFace(idx + 2, idx + 3, idx + 0);

	//-----------------------------------------------------------------------------------------------
	// left
	mb.SetUV(uvs.maxs.x, uvs.mins.y);
	idx = mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y + dimensions.y),	(center.z - dimensions.z)));

	//bl
	mb.SetUV(uvs.mins.x, uvs.mins.y);
	mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y + dimensions.y),	(center.z - dimensions.z)));

	// tl
	mb.SetUV(uvs.mins.x,uvs.maxs.y);
	mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y + dimensions.y),	(center.z + dimensions.z)));

	// tr
	mb.SetUV(uvs.maxs.x, uvs.maxs.y);
	mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y + dimensions.y),	(center.z + dimensions.z)));

	mb.AddFace(idx + 0, idx + 1, idx + 2);
	mb.AddFace(idx + 2, idx + 3, idx + 0);

	//-----------------------------------------------------------------------------------------------
	// back
	mb.SetUV(uvs.maxs.x, uvs.mins.y);
	idx = mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y - dimensions.y),	(center.z - dimensions.z)));

	//bl
	mb.SetUV(uvs.mins.x, uvs.mins.y);
	mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y + dimensions.y),	(center.z - dimensions.z)));

	// tl
	mb.SetUV(uvs.mins.x,uvs.maxs.y);
	mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y + dimensions.y),	(center.z + dimensions.z)));

	// tr
	mb.SetUV(uvs.maxs.x, uvs.maxs.y);
	mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y - dimensions.y),	(center.z + dimensions.z)));

	mb.AddFace(idx + 0, idx + 1, idx + 2);
	mb.AddFace(idx + 2, idx + 3, idx + 0);

	//-----------------------------------------------------------------------------------------------
	// right face
	mb.SetUV(uvs.maxs.x, uvs.mins.y);
	idx = mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y - dimensions.y),	(center.z - dimensions.z)));

	//bl
	mb.SetUV(uvs.mins.x, uvs.mins.y);
	mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y - dimensions.y),	(center.z - dimensions.z)));

	// tl
	mb.SetUV(uvs.mins.x,uvs.maxs.y);
	mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y - dimensions.y),	(center.z + dimensions.z)));

	// tr
	mb.SetUV(uvs.maxs.x, uvs.maxs.y);
	mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y - dimensions.y),	(center.z + dimensions.z)));

	mb.AddFace(idx + 0, idx + 1, idx + 2);
	mb.AddFace(idx + 2, idx + 3, idx + 0);

	//-----------------------------------------------------------------------------------------------
	// Bottom Face
	uvs = theDefinition->m_bottomUVs;
	mb.SetUV(uvs.maxs.x, uvs.mins.y);
	idx = mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y - dimensions.y),	(center.z - dimensions.z)));

	//bl
	mb.SetUV(uvs.mins.x, uvs.mins.y);
	mb.PushVertex(Vector3((center.x - dimensions.x),	(center.y + dimensions.y),	(center.z - dimensions.z)));

	// tl
	mb.SetUV(uvs.mins.x,uvs.maxs.y);
	mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y + dimensions.y),	(center.z - dimensions.z)));

	// tr
	mb.SetUV(uvs.maxs.x, uvs.maxs.y);
	mb.PushVertex(Vector3((center.x + dimensions.x),	(center.y - dimensions.y),	(center.z - dimensions.z)));

	mb.AddFace(idx + 0, idx + 1, idx + 2);
	mb.AddFace(idx + 2, idx + 3, idx + 0);

	//-----------------------------------------------------------------------------------------------
	// Do the draw
	Mesh* mesh = mb.CreateMesh<Vertex3D_PCU>();

	r->SetShader(Shader::CreateOrGetShader("default"));
	r->SetCurrentTexture(0 , g_blockSpriteSheet.m_spriteSheetTexture);
	r->SetCamera(m_camera);
	r->DrawMesh(mesh, true);
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
	if(IsDevConsoleOpen())
		return;

	if(DidMouseWheelScrollUp())
	{
		m_cameraSpeed *= 1.2f;
	}

	if(DidMouseWheelScrollDown())
	{
		m_cameraSpeed *= .5f;

		m_cameraSpeed = ClampFloat(m_cameraSpeed, 10.f, 1000.f);
	}
	
	// do this last cause itll move the mouse 
	MoveCamera();
}

void Playing::MoveCamera()
{
	float dt = g_theGameClock->deltaTime; // this needs to be after keyboard because we might fuck with ds for go to next frames
	float rotationSpeed = .20f;
	float sensitivity = .05f;
	
	// Apply Rotation
	Vector2 mouse_delta = g_theInput->GetMouseDelta();

	m_gameCamera->pitchDegreesAboutY += mouse_delta.y * sensitivity;
	m_gameCamera->yawDegreesAboutZ -= mouse_delta.x * sensitivity;

	m_gameCamera->pitchDegreesAboutY = ClampFloat(m_gameCamera->pitchDegreesAboutY, -90.f, 90.f);
	m_gameCamera->yawDegreesAboutZ = fmod(m_gameCamera->yawDegreesAboutZ, 360.f);
	
	// movement
	Vector3 amountToMove = Vector3::ZERO;
	float ds = g_theGameClock->deltaTime;

	Vector3 forward = m_gameCamera->GetForwardXY0();
	Vector3 right = Vector3(forward.y, -forward.x, 0.f);

	if(IsKeyPressed(G_THE_LETTER_W))
		amountToMove = forward;
	if(IsKeyPressed(G_THE_LETTER_S))
		amountToMove = -forward;
	if(IsKeyPressed(G_THE_LETTER_A))
		amountToMove = -right;
	if(IsKeyPressed(G_THE_LETTER_D))
		amountToMove = right;
	if(IsKeyPressed(G_THE_LETTER_E))
		amountToMove = Vector3::UP;
	if(IsKeyPressed(G_THE_LETTER_Q))
		amountToMove = Vector3::DOWN;

	if(amountToMove != Vector3::ZERO)
		m_gameCamera->pos += (amountToMove * ds * m_cameraSpeed);
}



