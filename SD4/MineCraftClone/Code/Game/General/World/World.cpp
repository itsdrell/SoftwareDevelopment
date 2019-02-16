#include "World.hpp"
#include "Engine/Core/General/Camera.hpp"
#include "Engine/Renderer/Systems/MeshBuilder.hpp"
#include "Engine/Renderer/Images/Textures/TextureCube.hpp"
#include "Game/General/GameCamera.hpp"
#include "Game/Main/GameCommon.hpp"
#include "Game/General/World/Chunk.hpp"
#include "Game/General/World/BlockDefinition.hpp"
#include "Engine/Core/Tools/DevConsole.hpp"
#include "Engine/Core/Tools/Clock.hpp"
#include "Game/General/Utils/Neighborhood.hpp"
#include <xtgmath.h>

//===============================================================================================
World::World()
{
	m_skyBox = new TextureCube();
	m_skyBox->make_from_image("Data/Images/galaxy2.png");

	// Create some block defs
	new BlockDefinition("air", BLOCK_TYPE_AIR, IntVector2(), IntVector2(), IntVector2());
	new BlockDefinition("grass", BLOCK_TYPE_GRASS, IntVector2(2, 0), IntVector2(3, 3), IntVector2(4, 3));
	new BlockDefinition("snow", BLOCK_TYPE_SNOW, IntVector2(1, 3), IntVector2(2, 3), IntVector2(4, 3));
	new BlockDefinition("stone", BLOCK_TYPE_STONE, IntVector2(7, 4), IntVector2(7, 4), IntVector2(7, 4));
	new BlockDefinition("test", BLOCK_TYPE_TEST, IntVector2(31, 31), IntVector2(31, 31), IntVector2(31, 31));

	m_chunkActivationCheatSheet = new Neighborhood(ACTIVATION_RADIUS_IN_CHUNKS);

	//IntVector2 pos = IntVector2(0, 0);
	//Chunk* newChunk = new Chunk(pos);
	//s_activeChunks.insert(std::pair<IntVector2, Chunk*>(pos, newChunk));
	//
	//pos = IntVector2(-1, 2);
	//newChunk = new Chunk(pos);
	//s_activeChunks.insert(std::pair<IntVector2, Chunk*>(pos, newChunk));
	//
	//pos = IntVector2(0, 2);
	//newChunk = new Chunk(pos);
	//s_activeChunks.insert(std::pair<IntVector2, Chunk*>(pos, newChunk));
	//
	//pos = IntVector2(1, 2);
	//newChunk = new Chunk(pos);
	//s_activeChunks.insert(std::pair<IntVector2, Chunk*>(pos, newChunk));
	//
	//pos = IntVector2(0, 3);
	//newChunk = new Chunk(pos);
	//s_activeChunks.insert(std::pair<IntVector2, Chunk*>(pos, newChunk));

	MeshBuilder mb;
	mb.AddCube(Vector3::ZERO, Vector3::ONE);
	m_skyMesh = mb.CreateMesh<Vertex3D_PCU>();

	// Cameras
	m_camera = new Camera();
	m_camera->SetColorTarget(g_theRenderer->m_defaultColorTarget);
	m_camera->SetDepthStencilTarget(g_theRenderer->m_defaultDepthTarget);
	m_camera->LookAt(Vector3::ZERO, Vector3(0.f, 0.f, -10.f));
	g_theRenderer->SetCamera();

	m_gameCamera = new GameCamera();
}

World::~World()
{
}

void World::Update()
{
	CheckAndActivateChunk();
	CheckKeyboardInputs();
}

void World::UpdateChunks()
{
	std::map<ChunkCoords, Chunk*>::iterator theIterator;
	for (theIterator = m_activeChunks.begin(); theIterator != m_activeChunks.end(); theIterator++)
	{
		theIterator->second->Update();
	}
}

void World::CheckKeyboardInputs()
{
	if (IsDevConsoleOpen())
		return;
	
	if (DidMouseWheelScrollUp())
	{
		m_cameraSpeed *= 1.2f;
	}

	if (DidMouseWheelScrollDown())
	{
		m_cameraSpeed *= .5f;

		m_cameraSpeed = ClampFloat(m_cameraSpeed, 10.f, 1000.f);
	}

	// do this last cause it'll move the mouse 
	UpdateCamera();
}

void World::UpdateCamera()
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

	if (IsKeyPressed(G_THE_LETTER_W))
		amountToMove = forward;
	if (IsKeyPressed(G_THE_LETTER_S))
		amountToMove = -forward;
	if (IsKeyPressed(G_THE_LETTER_A))
		amountToMove = -right;
	if (IsKeyPressed(G_THE_LETTER_D))
		amountToMove = right;
	if (IsKeyPressed(G_THE_LETTER_E))
		amountToMove = Vector3::UP;
	if (IsKeyPressed(G_THE_LETTER_Q))
		amountToMove = Vector3::DOWN;

	if (amountToMove != Vector3::ZERO)
		m_gameCamera->pos += (amountToMove * ds * m_cameraSpeed);
}

void World::Render() const
{
	Renderer* r = Renderer::GetInstance();

	// Set up Cameras
	m_camera->SetPerspective(45.f, (16.f / 9.f), .1f, 400.f);

	//m_camera->m_cameraMatrix = m_camera->transform.GetLocalMatrix();//Matrix44(); //modelMatrix;
	Matrix44 theModel = m_gameCamera->GetModelMatrix();
	Matrix44 theView = m_gameCamera->GetViewMatrix();
	m_camera->m_cameraMatrix = theModel;
	m_camera->m_viewMatrix = theView;

	RenderSkyBox();

	RenderChunks();
	//r->DrawMesh(m_testChunk->m_gpuMesh);

	RenderBasis();
}


void World::RenderChunks() const
{
	Renderer* r = Renderer::GetInstance();
	
	r->SetShader(Shader::CreateOrGetShader("default"));
	r->SetCurrentTexture(0, g_blockSpriteSheet.m_spriteSheetTexture);
	r->SetCamera(m_camera);
	
	for (auto theIterator = m_activeChunks.begin(); theIterator != m_activeChunks.end(); theIterator++)
	{
		//r->DrawMesh(theIterator->second->m_gpuMesh);
		theIterator->second->Render();
	}

	r->SetCurrentTexture();
}

void World::RenderSkyBox() const
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

//-----------------------------------------------------------------------------------------------
void World::RenderBasis() const
{
	Renderer* r = Renderer::GetInstance();

	Shader* theShader = Shader::CreateOrGetShader("Data/Shaders/xray.shader");
	r->SetShader(theShader);
	r->DrawBasis(Matrix44(), 10.f);

	theShader->SetDepth(COMPARE_ALWAYS, true);
	r->SetShader(theShader);
	r->DrawBasis(Matrix44(), 10.f);

	// clean up for dev console
	//r->SetShader();
	r->BindRenderState(Shader::CreateOrGetShader("default")->m_state);
}

//-----------------------------------------------------------------------------------------------
void World::CheckAndActivateChunk()
{
	std::vector<IntVector2>& cheatSheat = m_chunkActivationCheatSheet->m_blockCoords;

	for (uint i = 0; i < cheatSheat.size(); i++)
	{
		ChunkCoords current = cheatSheat.at(i);
		
		int xPos = ((int)(floor(m_gameCamera->pos.x / CHUNK_SIZE_X)));
		int yPos = ((int)(floor(m_gameCamera->pos.y / CHUNK_SIZE_Y)));
		ChunkCoords worldCoords = ChunkCoords(xPos + current.x, yPos + current.y);

		if (!IsChunkActivated(worldCoords))
		{
			ActivateChunk(worldCoords);
			return;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void World::ActivateChunk(const ChunkCoords& theCoords)
{
	Chunk* newChunk = new Chunk(theCoords);
	m_activeChunks.insert(std::pair<ChunkCoords, Chunk*>(theCoords, newChunk));
}

//-----------------------------------------------------------------------------------------------
bool World::IsChunkActivated(const ChunkCoords& theCoords)
{
	std::map<ChunkCoords, Chunk*>::iterator theIterator = m_activeChunks.find(theCoords);

	return !(theIterator == m_activeChunks.end());
}
