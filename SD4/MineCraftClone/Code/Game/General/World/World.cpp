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
#include "Engine/Renderer/Systems/DebugRenderSystem.hpp"
#include "Game/General/UI/HUD.hpp"
#include <xtgmath.h>
#include "Engine/Core/General/EngineCommon.hpp"
#include "Engine/Core/General/Blackboard.hpp"
#include "Engine/Math/CubicSpline.hpp"
#include "Engine/ThirdParty/SquirrelNoise/SmoothNoise.hpp"
#include "Game/General/Entities/Player.hpp"



//===============================================================================================
void WorldTime::Advance(float ds)
{
	m_time += ds / (60 * 60 * 24);
}

//-----------------------------------------------------------------------------------------------
int WorldTime::GetDay()
{
	return (int)(ceil(m_time));
}

//-----------------------------------------------------------------------------------------------
int WorldTime::GetHour()
{
	float time = GetFractionOf(m_time);
	float hour = RangeMapFloat(time, 0.f, 1.f, 0.f, 24.f);
	return (int)(floor(hour));
}

//-----------------------------------------------------------------------------------------------
float WorldTime::GetNormalizedPercentThroughDay()
{
	return GetFractionOf(m_time);
}

//-----------------------------------------------------------------------------------------------
bool WorldTime::IsNightTime()
{
	float time = GetFractionOf(m_time);

	if (time < .25f && time > .75f)
		return true;

	return false;
}

//===============================================================================================
World::World()
{
	//m_skyBox = new TextureCube();
	//m_skyBox->make_from_image("Data/Images/galaxy2.png");

	m_chunkActivationCheatSheet = new Neighborhood(ACTIVATION_RADIUS_IN_CHUNKS);

	MeshBuilder mb;
	mb.AddCube(Vector3::ZERO, Vector3::ONE);
	m_skyMesh = mb.CreateMesh<Vertex3D_PCU>();

	// Cameras
	m_camera = new Camera();
	m_camera->SetColorTarget(g_theRenderer->m_defaultColorTarget);
	m_camera->SetDepthStencilTarget(g_theRenderer->m_defaultDepthTarget);
	m_camera->LookAt(Vector3::ZERO, Vector3(0.f, 0.f, -10.f));
	g_theRenderer->SetCamera();
	DebugRenderSet3DCamera(m_camera);

	AddPlayer(Vector3(-21.f, 6.f, 129.f));

	m_gameCamera = new GameCamera();
	m_gameCamera->pos = Vector3(0.f, 0.f, 200.f);
	m_gameCamera->pitchDegreesAboutY = 89.f;
	m_gameCamera->m_entityToFollow = m_player;
	m_gameCamera->SetCameraMode(CAMERA_MODE_FIRST_PERSON);

	m_skyColor = Rgba(20, 20, 40, 255);
	m_indoorLightColor.SetFromNormalizedFloats(1.0f, .9f, .8f, 1.f);

}

//-----------------------------------------------------------------------------------------------
World::~World()
{
	std::map<ChunkCoords, Chunk*>::iterator theIterator = m_activeChunks.begin();
	for (theIterator; theIterator != m_activeChunks.end(); theIterator++)
	{
		delete theIterator->second;
	}

	m_activeChunks.clear();
}

//-----------------------------------------------------------------------------------------------
void World::Update()
{
	m_theWorldTime.Advance(g_theGameClock->deltaTime * m_worldScale);
	UpdateSky();

	CheckAndActivateChunk();
	CheckAndDeactivateChunk();
	UpdateEntities();
	FindPlayersTargetedBlock(); // move to player
	CheckKeyboardInputs(); // move to player

	// if this is false, we do it in check debug keys :)
	if (g_gameConfigBlackboard.GetValue("stepDebugLighting", false) == false)
	{
		m_debugDirtyLighting.clear();
		UpdateDirtyLighting();
	}
	CheckAndRebuildChunkMesh();
	AddSkyDebugPointsForChunkIAmOn();
	//DebugRenderLog(.1f, "Active Chunks: " + std::to_string(m_activeChunks.size()));
	//DebugRenderLog(0.05f, std::to_string(m_debugDirtyLighting.size()), Rgba::YELLOW);
}

//-----------------------------------------------------------------------------------------------
void World::UpdateChunks()
{
	std::map<ChunkCoords, Chunk*>::iterator theIterator;
	for (theIterator = m_activeChunks.begin(); theIterator != m_activeChunks.end(); theIterator++)
	{
		theIterator->second->Update();
	}
}

//-----------------------------------------------------------------------------------------------
void World::UpdateSky()
{	
	float percentThroughDay = m_theWorldTime.GetNormalizedPercentThroughDay();

	float cosValue = CosDegrees(percentThroughDay * 360.f);
	float inverse = cosValue * -1.f;
	float actualValue = RangeMapFloat(inverse, -1.f, 1.f, 0.f, 1.f);

	m_skyColor = Interpolate(Rgba(20, 20, 40, 255), Rgba(200, 230, 255, 255), actualValue);

	float lightningNoise = Compute1dPerlinNoise(g_theGameClock->totalTime, 1.0f , 9);
	float lightningStrength = RangeMapFloat(lightningNoise, .6f, .9f, 0.f, 1.f);
	lightningStrength = ClampFloat(lightningStrength, 0.f, 1.f);
	m_skyColor = Interpolate(m_skyColor, Rgba::WHITE, lightningStrength);

	float glowPerlin = Compute1dPerlinNoise(g_theGameClock->totalTime, 1.0f, 9);
	float glowStrength = RangeMapFloat(glowPerlin, -1.f, 1.f, .8f, 1.f);
	m_indoorLightFlickerStrength = ClampFloat(glowStrength,.8f, 1.f);

}

//-----------------------------------------------------------------------------------------------
void World::CheckKeyboardInputs()
{
	if (IsKeyPressed(G_THE_LETTER_T))
	{
		m_worldScale = 20000;
	}
	else
	{
		m_worldScale = 1;
	}

	DebugKeys();
}

//-----------------------------------------------------------------------------------------------
void World::DebugKeys()
{
	// go back to origin
	if (WasKeyJustPressed(G_THE_LETTER_O))
	{
		m_gameCamera->pos = Vector3(0.f, 0.f, 200.f);
		m_gameCamera->rollDegreesAboutX = 0.f;
		m_gameCamera->pitchDegreesAboutY = 89.f;
		m_gameCamera->yawDegreesAboutZ = 0.f;
	}

	if (WasKeyJustPressed(G_THE_LETTER_U))
	{
		std::map<ChunkCoords, Chunk*>::iterator theIterator = m_activeChunks.begin();
		for (theIterator; theIterator != m_activeChunks.end(); theIterator++)
		{
			delete theIterator->second;
		}
		
		m_activeChunks.clear();
	}

	if (WasKeyJustPressed(G_THE_LETTER_L))
	{
		m_showTargettedBlockRaycast = !m_showTargettedBlockRaycast;

		if(m_showTargettedBlockRaycast)
			DebugRenderLog(1.f, "Switched to showing targetted raycast block");
		else
			DebugRenderLog(1.f, "Switched to hiding raycast show (and recalculating)");
	}


	if (g_gameConfigBlackboard.GetValue("stepDebugLighting", false))
	{
		if (WasKeyJustPressed(G_THE_LETTER_N))
		{
			m_debugDirtyLighting.clear();
			UpdateDirtyLighting();
		}
	}
}

//-----------------------------------------------------------------------------------------------
void World::UpdateCamera()
{
	// Apply Rotation
	Vector2 mouse_delta = g_theInput->GetMouseDelta();

	m_gameCamera->pitchDegreesAboutY += mouse_delta.y * .5f;
	m_gameCamera->yawDegreesAboutZ -= mouse_delta.x * 1.5f;

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

//-----------------------------------------------------------------------------------------------
void World::UpdateEntities()
{
	for (uint i = 0; i < m_entities.size(); i++)
	{
		Entity*& current = m_entities.at(i);

		current->Update();
	}

	// not sure if this should be an entity
	m_gameCamera->Update();
}

//-----------------------------------------------------------------------------------------------
void World::FindPlayersTargetedBlock()
{
	// locking the camera as well
	if (!m_showTargettedBlockRaycast && m_gameCamera->m_mode != CAMERA_MODE_MANUAL)
	{
		m_targetBlockRaycast = RayCast(m_player->m_position, m_camera->GetForward(), 8.f);
	}
}

//-----------------------------------------------------------------------------------------------
void World::AddSkyDebugPointsForChunkIAmOn()
{
	if (g_gameConfigBlackboard.GetValue("showSkyBlocks", false))
	{
		m_debugSkyPoints.clear();

		ChunkCoords playerCoords = Chunk::GetChunkCoordsFromWorldPosition(Vector3(floor(m_gameCamera->pos.x), floor(m_gameCamera->pos.y), 0.f));
		Chunk* theChunk = m_activeChunks[playerCoords];

		for (uint i = 0; i < AMOUNT_OF_BLOCKS_IN_CHUNK; i++)
		{
			BlockLocator current = BlockLocator(theChunk, i);

			if (current.IsSky())
				AddSkyDebugPoint(current.GetCenterOfBlock());
		}
	}
}

//-----------------------------------------------------------------------------------------------
void World::Render() const
{
	RenderSky();
	HammerCameraValuesToEngineCamera();

	RenderChunks();
	RenderBasis();
	RenderTargetBlock();
	RenderEntities();
	RenderTargettedBlockRaycast();

	if (g_gameConfigBlackboard.GetValue("useDebugPoints", false))
	{
		RenderDebugPoints();
	}
}

//-----------------------------------------------------------------------------------------------
void World::RenderSky() const
{
	Renderer* r = Renderer::GetInstance();

	Vector4 colorValues = m_skyColor.GetAsNormalizedVector4();

	r->ClearScreen(colorValues);
}

//-----------------------------------------------------------------------------------------------
void World::RenderChunks() const
{
	Renderer* r = Renderer::GetInstance();
	
	Shader* chunkShader;
	if (g_gameConfigBlackboard.GetValue("useOldWorldShader", false))
		chunkShader = Shader::CreateOrGetShader("default");
	else
		chunkShader = Shader::CreateOrGetShader("Data/Shaders/OverworldOpaque.shader");

	r->SetShader(chunkShader);
	r->SetCurrentTexture(0, g_blockSpriteSheet.m_spriteSheetTexture);
	r->SetCamera(m_camera);

	r->SetUniform("u_cameraPos", m_gameCamera->pos);
	r->SetUniform("u_indoorLightRgb", m_indoorLightColor);
	r->SetUniform("u_flickerStrength", m_indoorLightFlickerStrength);
	r->SetUniform("u_outdoorLightRgb", m_skyColor);
	r->SetUniform("u_skyColor", m_skyColor);
	r->SetUniform("u_fogNearDistance", CHUNK_ACTIVATION_DISTANCE - 32);
	r->SetUniform("u_fogFarDistance", CHUNK_ACTIVATION_DISTANCE);
	
	for (auto theIterator = m_activeChunks.begin(); theIterator != m_activeChunks.end(); theIterator++)
	{
		if(theIterator->second == nullptr)
			continue;
		
		Mesh* theMesh = theIterator->second->m_gpuMesh;

		// sometimes the mesh isn't made yet
		if (theMesh != nullptr)
		{
			r->DrawMesh(theIterator->second->m_gpuMesh);

			// debug stuff
			theIterator->second->Render();
		}


	}

	r->SetCurrentTexture();
}

//-----------------------------------------------------------------------------------------------
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
void World::RenderTargettedBlockRaycast() const
{
	if (m_showTargettedBlockRaycast)
	{
		Renderer* r = Renderer::GetInstance();

		if (m_targetBlockRaycast.DidImpact())
		{
			r->DrawLine3D(m_targetBlockRaycast.m_startPos, m_targetBlockRaycast.m_endPosition, 3.f, Rgba::GREEN);
			r->HighlightPoint(m_targetBlockRaycast.m_impactPosition, .1f, Rgba::MAGENTA);
		}
		else
		{
			r->DrawLine3D(m_targetBlockRaycast.m_startPos, m_targetBlockRaycast.m_endPosition, 3.f, Rgba::RED);
		}
	}
}

//-----------------------------------------------------------------------------------------------
void World::RenderTargetBlock() const
{
	bool didImpact = m_targetBlockRaycast.DidImpact();
	if ( didImpact && m_targetBlockRaycast.m_impactBlock.m_chunk != nullptr)
	{
		Renderer* r = Renderer::GetInstance();

		Vector3 centerPos = m_targetBlockRaycast.m_impactBlock.GetCenterOfBlock();

		r->DrawWireFramedCube(centerPos, Vector3(.51f), 2.5f, Rgba::RED);

		Vector3 normal = m_targetBlockRaycast.m_impactNormal;
		normal.Normalize();

		if (normal == Vector3::UP || normal == Vector3::DOWN)
		{
			Vector3 right = Cross(m_targetBlockRaycast.m_impactNormal, Vector3(1.f, 0.f, 0.f));
			r->DrawWireFramedPlane(centerPos + (m_targetBlockRaycast.m_impactNormal * .52f), 1.f, 1.f, 3.f, right, Vector3(1.f, 0.f, 0.f), Rgba::BLUE);
		}
		else
		{
			Vector3 right = Cross(m_targetBlockRaycast.m_impactNormal, Vector3(0.f, 0.f, 1.f));
			r->DrawWireFramedPlane(centerPos + (m_targetBlockRaycast.m_impactNormal * .52f), 1.f, 1.f, 3.f, right, Vector3(0.f, 0.f, 1.f), Rgba::BLUE);
		}
	}
}

//-----------------------------------------------------------------------------------------------
void World::RenderDebugPoints() const
{
	Renderer* r = Renderer::GetInstance();
	MeshBuilder mb;
	mb.Begin(PRIMITIVE_POINTS, false);

	for (uint i = 0; i < m_debugDirtyLighting.size(); i++)
	{
		DebugPoint current = m_debugDirtyLighting.at(i);
		
		mb.AppendPoint(current.m_position, current.m_color);
	}
	
	for (uint j = 0; j < m_debugSkyPoints.size(); j++)
	{
		DebugPoint current = m_debugSkyPoints.at(j);

		mb.AppendPoint(current.m_position, current.m_color);
	}
	

	mb.End();
	Mesh* theMesh = mb.CreateMesh<Vertex3D_PCU>();

	r->SetPointSize(5);
	r->DrawMesh(theMesh, true);
}

//-----------------------------------------------------------------------------------------------
void World::RenderEntities() const
{
	for (uint i = 0; i < m_entities.size(); i++)
	{
		Entity* current = m_entities.at(i);

		current->Render();
	}
}

//-----------------------------------------------------------------------------------------------
void World::AddPlayer(const Vector3& position)
{
	m_player = new Player(position);
	m_player->m_worldTheyAreIn = this;
	//m_player->m_gameCamera = m_gameCamera;

	m_entities.push_back(m_player);
}

//-----------------------------------------------------------------------------------------------
void World::DebugValidateAllChunks()
{
	std::map<ChunkCoords, Chunk*>::iterator chunkIter;
	for (chunkIter = m_activeChunks.begin(); chunkIter != m_activeChunks.end(); ++chunkIter)
	{
		Chunk* chunk = chunkIter->second;
		chunk->DebugValidateMe();
	}
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
void World::CheckAndDeactivateChunk()
{
	Chunk* furthestChunk = GetFarthestChunkFromPlayer(m_gameCamera->pos.xy());

	if (furthestChunk == nullptr)
		return;

	Vector2 currentWorldPos = Vector2(
		(float)(furthestChunk->m_chunkCoords.x * CHUNK_SIZE_X),
		(float)(furthestChunk->m_chunkCoords.y * CHUNK_SIZE_Y));

	float distanceSquaredForCurrent = GetDistanceSquared(currentWorldPos, m_gameCamera->pos.xy());

	if (distanceSquaredForCurrent > CHUNK_DEACTIVATION_DISTANCE_SQUARED)
	{
		UndirtyAllBlocksInChunk(furthestChunk);
		
		furthestChunk->LeaveYourNeighbors();
		
		m_activeChunks.erase(furthestChunk->m_chunkCoords);
		
		delete furthestChunk;
		furthestChunk = nullptr;
	}
}

//-----------------------------------------------------------------------------------------------
void World::CheckAndRebuildChunkMesh()
{
	std::vector<IntVector2>& cheatSheat = m_chunkActivationCheatSheet->m_blockCoords;

	int xPos = ((int)(floor(m_gameCamera->pos.x / CHUNK_SIZE_X)));
	int yPos = ((int)(floor(m_gameCamera->pos.y / CHUNK_SIZE_Y)));
	ChunkCoords playerCoords = ChunkCoords(xPos, yPos);

	for (uint i = 0; i < cheatSheat.size(); i++)
	{
		ChunkCoords currentCoords = cheatSheat.at(i) + playerCoords;
		Chunk* currentChunk = GetChunkFromChunkCoords(currentCoords);

		if (currentChunk != nullptr && currentChunk->m_isGPUDirty && currentChunk->CanRebuildItsMesh())
		{
			currentChunk->GenerateMesh();
			return;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void World::ActivateChunk(const ChunkCoords& theCoords)
{
	Chunk* newChunk = new Chunk(theCoords);
	m_activeChunks.insert(std::pair<ChunkCoords, Chunk*>(theCoords, newChunk));

	Chunk* theNorthNeighbor= GetChunkFromChunkCoords(theCoords + IntVector2::NORTH);
	Chunk* theSouthNeighbor= GetChunkFromChunkCoords(theCoords + IntVector2::SOUTH);
	Chunk* theEastNeighbor= GetChunkFromChunkCoords(theCoords + IntVector2::EAST);
	Chunk* theWestNeighbor= GetChunkFromChunkCoords(theCoords + IntVector2::WEST);

	// let the chunk know its neighbors
	newChunk->m_northNeighbor	= theNorthNeighbor;
	newChunk->m_southNeighbor	= theSouthNeighbor;
	newChunk->m_eastNeighbor	= theEastNeighbor;
	newChunk->m_westNeighbor	= theWestNeighbor;

	// let the neighbors know about the chunk
	if (theNorthNeighbor != nullptr)	{ theNorthNeighbor->m_southNeighbor		= newChunk; }
	if (theSouthNeighbor != nullptr)	{ theSouthNeighbor->m_northNeighbor		= newChunk; }
	if (theEastNeighbor != nullptr)		{ theEastNeighbor->m_westNeighbor		= newChunk; }
	if (theWestNeighbor != nullptr)		{ theWestNeighbor->m_eastNeighbor		= newChunk; }
	
	newChunk->OnActivation();
}

//-----------------------------------------------------------------------------------------------
bool World::IsChunkActivated(const ChunkCoords& theCoords)
{
	std::map<ChunkCoords, Chunk*>::iterator theIterator = m_activeChunks.find(theCoords);

	return !(theIterator == m_activeChunks.end());
}

//-----------------------------------------------------------------------------------------------
Chunk* World::GetFarthestChunkFromPlayer(const Vector2& playerWorldPos)
{
	std::map<ChunkCoords, Chunk*>::iterator theIterator = m_activeChunks.begin();
	Chunk* currentFurthestChunk = theIterator->second;

	if (currentFurthestChunk == nullptr)
		return nullptr;
	
	Vector2 startingFurthestPos = Vector2(
		(float)(currentFurthestChunk->m_chunkCoords.x * CHUNK_SIZE_X),
		(float)(currentFurthestChunk->m_chunkCoords.y * CHUNK_SIZE_Y));

	float farthestDistance = GetDistanceSquared(startingFurthestPos, playerWorldPos);;
	
	for (theIterator; theIterator != m_activeChunks.end(); theIterator++)
	{
		Chunk* currentChunk = theIterator->second;
		Vector2 currentWorldPos = Vector2(
			(float)(currentChunk->m_chunkCoords.x * CHUNK_SIZE_X), 
			(float)(currentChunk->m_chunkCoords.y * CHUNK_SIZE_Y));

		float distanceSquaredForCurrent = GetDistanceSquared(currentWorldPos, playerWorldPos);
		
		if (distanceSquaredForCurrent > farthestDistance)
		{
			farthestDistance = distanceSquaredForCurrent;
			currentFurthestChunk = currentChunk;
		}
	}

	return currentFurthestChunk;
}

//-----------------------------------------------------------------------------------------------
Chunk* World::GetChunkFromChunkCoords(const ChunkCoords& theCoords)
{
	std::map<ChunkCoords, Chunk*>::iterator theIterator = m_activeChunks.find(theCoords);

	if (theIterator == m_activeChunks.end())
		return nullptr;

	return theIterator->second;
}

//-----------------------------------------------------------------------------------------------
void World::HammerCameraValuesToEngineCamera() const
{
	// Set up Cameras
	m_camera->SetPerspective(45.f, (16.f / 9.f), .1f, 400.f);
	
	if (m_gameCamera->m_mode == CAMERA_MODE_MANUAL)
	{
		Matrix44 theModel = m_gameCamera->GetModelMatrix();
		Matrix44 theView = m_gameCamera->GetViewMatrix();
		m_camera->m_cameraMatrix = theModel;
		m_camera->m_viewMatrix = theView;
	}
	else
	{
		Matrix44 theModel = m_player->GetModelMatrix();
		Matrix44 theView = m_player->GetViewMatrix();
		m_camera->m_cameraMatrix = theModel;
		m_camera->m_viewMatrix = theView;
	}

}

//-----------------------------------------------------------------------------------------------
void World::UpdateDirtyLighting()
{
	if (g_gameConfigBlackboard.GetValue("stepDebugLighting", false))
	{
		uint amountInQueAtStart = (uint) m_dirtyBlocks.size();

		for (uint i = 0; i < amountInQueAtStart; i++)
		{
			ProcessDirtyLightBlock();
			DebugValidateAllChunks();
		}
	}
	else
	{
		while (m_dirtyBlocks.size() != 0)
		{
			// pops the front off and updates it's light
			ProcessDirtyLightBlock();
			DebugValidateAllChunks();
		}
	}
	

}

//-----------------------------------------------------------------------------------------------
void World::ProcessDirtyLightBlock()
{
	DebugValidateAllChunks();
	BlockLocator theLocator = m_dirtyBlocks.front();
	m_dirtyBlocks.pop_front();

	if (!theLocator.IsValid())
	{
		return;
	}
	theLocator.m_chunk->DebugValidateMe();
	theLocator.m_chunk->Dirty();
	DebugValidateAllChunks();

	Block& theBlock = theLocator.GetBlock();
	theBlock.ClearIsLightDirty();
	DebugValidateAllChunks();

	// get my neighbors
	BlockLocator eastNeighbor = theLocator.GetBlockLocatorOfEastNeighbor();
	BlockLocator westNeighbor = theLocator.GetBlockLocatorOfWestNeighbor();
	BlockLocator northNeighbor = theLocator.GetBlockLocatorOfNorthNeighbor();
	BlockLocator southNeighbor = theLocator.GetBlockLocatorOfSouthNeighbor();
	BlockLocator aboveNeighbor = theLocator.GetBlockLocatorOfAboveNeighbor();
	BlockLocator belowNeighbor = theLocator.GetBlockLocatorOfBelowNeighbor();
	DebugValidateAllChunks();

	Block eastBlock =  BlockLocator::GetInvalidBlock();
	Block westBlock =  BlockLocator::GetInvalidBlock();
	Block northBlock = BlockLocator::GetInvalidBlock();
	Block southBlock = BlockLocator::GetInvalidBlock();
	Block aboveBlock = BlockLocator::GetInvalidBlock();
	Block belowBlock = BlockLocator::GetInvalidBlock();

	if (eastNeighbor.IsValid()) { eastBlock = eastNeighbor.GetBlock(); }
	if (westNeighbor.IsValid()) { westBlock = westNeighbor.GetBlock(); }
	if (northNeighbor.IsValid()) { northBlock = northNeighbor.GetBlock(); }
	if (southNeighbor.IsValid()) { southBlock = southNeighbor.GetBlock(); }
	if (aboveNeighbor.IsValid()) { aboveBlock = aboveNeighbor.GetBlock(); }
	if (belowNeighbor.IsValid()) { belowBlock = belowNeighbor.GetBlock(); }
	DebugValidateAllChunks();

	int maxIndoorLightValue = 0;
	int maxOutdoorLightValue = 0;

	// outdoor lighting check
	if (theBlock.IsSky())
	{
		maxOutdoorLightValue = MAX_LIGHT_VALUE;
	}
	else
	{
		int eastNeighborLightValue		   = 0;
		int westNeighborLightValue		   = 0;
		int northNeightborLightValue	   = 0;
		int southNeightborLightValue	   = 0;
		int aboveNeightborLightValue	   = 0;
		int bottomNeightborLightValue	   = 0;

		if (eastNeighbor.IsValid() && !eastNeighbor.IsFullyOpaque()) { eastNeighborLightValue = eastBlock.GetOutdoorLightLevel(); }
		if (westNeighbor.IsValid() && !westNeighbor.IsFullyOpaque()) { westNeighborLightValue = westBlock.GetOutdoorLightLevel(); }
		if (northNeighbor.IsValid() && !northNeighbor.IsFullyOpaque()) { northNeightborLightValue = northBlock.GetOutdoorLightLevel(); }
		if (southNeighbor.IsValid() && !southNeighbor.IsFullyOpaque()) { southNeightborLightValue = southBlock.GetOutdoorLightLevel(); }
		if (aboveNeighbor.IsValid() && !aboveNeighbor.IsFullyOpaque()) { aboveNeightborLightValue = aboveBlock.GetOutdoorLightLevel(); }
		if (belowNeighbor.IsValid() && !belowNeighbor.IsFullyOpaque()) { bottomNeightborLightValue = belowBlock.GetOutdoorLightLevel(); }
		DebugValidateAllChunks();

		if (eastNeighborLightValue > maxOutdoorLightValue) 
			maxOutdoorLightValue = eastNeighborLightValue;
		if (westNeighborLightValue > maxOutdoorLightValue)
			maxOutdoorLightValue = westNeighborLightValue;
		if (northNeightborLightValue > maxOutdoorLightValue)
			maxOutdoorLightValue = northNeightborLightValue;
		if (southNeightborLightValue > maxOutdoorLightValue)
			maxOutdoorLightValue = southNeightborLightValue;
		if (aboveNeightborLightValue > maxOutdoorLightValue)
			maxOutdoorLightValue = aboveNeightborLightValue;
		if (bottomNeightborLightValue > maxOutdoorLightValue)
			maxOutdoorLightValue = bottomNeightborLightValue;
	}

	// indoor lighting check
	if (!theBlock.IsFullyOpaque())
	{
		int eastNeighborLightValue = 0;
		int westNeighborLightValue = 0;
		int northNeightborLightValue = 0;
		int southNeightborLightValue = 0;
		int aboveNeightborLightValue = 0;
		int bottomNeightborLightValue = 0;

		if (eastNeighbor.IsValid()) { eastNeighborLightValue = eastBlock.GetIndoorLightLevel(); }
		if (westNeighbor.IsValid()) { westNeighborLightValue = westBlock.GetIndoorLightLevel(); }
		if (northNeighbor.IsValid()) { northNeightborLightValue = northBlock.GetIndoorLightLevel(); }
		if (southNeighbor.IsValid()) { southNeightborLightValue = southBlock.GetIndoorLightLevel(); }
		if (aboveNeighbor.IsValid()) { aboveNeightborLightValue = aboveBlock.GetIndoorLightLevel(); }
		if (belowNeighbor.IsValid()) { bottomNeightborLightValue = belowBlock.GetIndoorLightLevel(); }
		DebugValidateAllChunks();

		if (eastNeighborLightValue > maxIndoorLightValue)
			maxIndoorLightValue = eastNeighborLightValue;
		if (westNeighborLightValue > maxIndoorLightValue)
			maxIndoorLightValue = westNeighborLightValue;
		if (northNeightborLightValue > maxIndoorLightValue)
			maxIndoorLightValue = northNeightborLightValue;
		if (southNeightborLightValue > maxIndoorLightValue)
			maxIndoorLightValue = southNeightborLightValue;
		if (aboveNeightborLightValue > maxIndoorLightValue)
			maxIndoorLightValue = aboveNeightborLightValue;
		if (bottomNeightborLightValue > maxIndoorLightValue)
			maxIndoorLightValue = bottomNeightborLightValue;
	}

	int expectedIndoorValue = ClampInt(maxIndoorLightValue - 1, 0, MAX_LIGHT_VALUE);
	int expectedOutdoorValue = ClampInt(maxOutdoorLightValue - 1, 0, MAX_LIGHT_VALUE);

	// sanity checks
	if (theBlock.IsSky()) // sanity
		expectedOutdoorValue = MAX_LIGHT_VALUE;

	BlockDefinition* theDef = BlockDefinition::GetDefinitionByType(theBlock.m_type);
	if (theDef->m_lightLevel > expectedIndoorValue)
	{
		expectedIndoorValue = theDef->m_lightLevel;
	}
	DebugValidateAllChunks();

	// Check to see if we changed (were wrong!), if we did, dirty the neighbors
	bool dirtyNeighbors = false;
	if (theBlock.GetIndoorLightLevel() != expectedIndoorValue)
	{
		theBlock.SetIndoorLightLevel(expectedIndoorValue);
		dirtyNeighbors = true;
	}
	DebugValidateAllChunks();

	if (theBlock.GetOutdoorLightLevel() != expectedOutdoorValue)
	{
		theBlock.SetOutdoorLightLevel(expectedOutdoorValue);
		dirtyNeighbors = true;
	}
	DebugValidateAllChunks();

	// spread the love
	if (dirtyNeighbors)
	{
		DebugValidateAllChunks();
		if (!eastBlock.IsFullyOpaque()) { MarkLightingDirty(eastNeighbor);  }
		if (!westBlock.IsFullyOpaque()) { MarkLightingDirty(westNeighbor);  }
		if (!northBlock.IsFullyOpaque()) { MarkLightingDirty(northNeighbor); }
		if (!southBlock.IsFullyOpaque()) { MarkLightingDirty(southNeighbor); }
		if (!aboveBlock.IsFullyOpaque()) { MarkLightingDirty(aboveNeighbor); }
		if (!belowBlock.IsFullyOpaque()) { MarkLightingDirty(belowNeighbor); }
		DebugValidateAllChunks();
	}
}

//-----------------------------------------------------------------------------------------------
void World::MarkLightingDirty(BlockLocator & blockToDirty)
{
	if (!blockToDirty.IsValid())
		return;

	blockToDirty.m_chunk->DebugValidateMe();
	Block& theBlock = blockToDirty.GetBlock();

	// we mark them dirty when they are in the list, so if its already dirty,
	// its already in the list. Early out
	if (theBlock.IsLightDirty() )
		return;

	theBlock.SetIsLightDirty();
	m_dirtyBlocks.push_back(blockToDirty);

	// add a debug light point
	if (g_gameConfigBlackboard.GetValue("useDebugLighting", false))
	{
		m_debugDirtyLighting.push_back(DebugPoint(blockToDirty.GetCenterOfBlock(), Rgba::MAGENTA));
	}
}

//-----------------------------------------------------------------------------------------------
void World::UndirtyAllBlocksInChunk(const Chunk * theChunk)
{
	for(uint i = 0; i < m_dirtyBlocks.size(); i++)
	{
		BlockLocator& current = m_dirtyBlocks.at(i);

		// just making it invalid, will get cleaned out
		if(current.m_chunk == theChunk)
		{
			current.m_chunk = nullptr;
			current.m_indexOfBlock = -1;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void World::AddSkyDebugPoint(const Vector3& pos)
{
	m_debugSkyPoints.push_back(DebugPoint(pos, Rgba::RAINBOW_BLUE));
}

//-----------------------------------------------------------------------------------------------
RaycastResult World::RayCast(const Vector3& start, const Vector3& forward, float maxDistance)
{
	RaycastResult theResult;
	theResult.m_startPos = start;
	theResult.m_direction = forward;
	theResult.m_maxDistance = maxDistance;
	theResult.m_endPosition = start + (forward * maxDistance);

	Vector3 stepAmount = forward * RAYCAST_STEP_SIZE;
	int amountOfSteps = (int)(maxDistance * (1.f / RAYCAST_STEP_SIZE));

	Vector3 currentPosition = start;
	BlockLocator previousBlock = BlockLocator(nullptr, -1);
	for(int i = 0; i < amountOfSteps; i++)
	{
		currentPosition += stepAmount;

		ChunkCoords theChunkCoords = Chunk::GetChunkCoordsFromWorldPosition(currentPosition);
		Chunk* theCurrentChunk = GetChunkFromChunkCoords(theChunkCoords);
		if (theCurrentChunk == nullptr) { break; }
		BlockIndex theBlocksIndex = theCurrentChunk->GetBlockIndexForWorldCoords(currentPosition);

		BlockLocator blockWeJustEntered = BlockLocator(theCurrentChunk, theBlocksIndex);

		// we hit something solid!
		if (blockWeJustEntered.IsFullyOpaque())
		{
			// we are in it
			if(i == 0)
			{
				theResult.m_impactFraction = 1.f;
				return theResult;
			}
			else
			{
				theResult.m_impactBlock = blockWeJustEntered;
				theResult.m_impactPosition = currentPosition;

				theResult.m_impactDistance = (maxDistance / (float)amountOfSteps);
				theResult.m_impactFraction = (float)i / (float)amountOfSteps;

				BlockCoords prev = Chunk::GetBlockCoordsForBlockIndex(previousBlock.m_indexOfBlock);
				BlockCoords currentBlockCoords = Chunk::GetBlockCoordsForBlockIndex(theBlocksIndex);

				Vector3 prevBlockPos = previousBlock.GetCenterOfBlock();
				Vector3 currentBlockPos = blockWeJustEntered.GetCenterOfBlock();
				Vector3 theNormal = prevBlockPos - currentBlockPos;
				theResult.m_impactNormal = theNormal;

				return theResult;
			}
		}

		previousBlock = blockWeJustEntered;
	}
	
	// we didn't hit anything
	theResult.m_impactFraction = 1.f;
	return theResult;
}


