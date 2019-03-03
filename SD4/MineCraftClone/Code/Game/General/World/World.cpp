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


//===============================================================================================
World::World()
{
	m_skyBox = new TextureCube();
	m_skyBox->make_from_image("Data/Images/galaxy2.png");

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

	m_gameCamera = new GameCamera();
	m_gameCamera->pos = Vector3(0.f, 0.f, 200.f);
	m_gameCamera->pitchDegreesAboutY = 89.f;

	m_playerHUD = new HUD();
	m_playerHUD->m_world = this;

	m_blockToPlace = BlockDefinition::GetDefinitionByName("glowStone");

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
	CheckAndActivateChunk();
	CheckAndDeactivateChunk();
	FindPlayersTargetedBlock();
	CheckKeyboardInputs();

	// if this is false, we do it in check debug keys :)
	if (g_gameConfigBlackboard.GetValue("stepDebugLighting", false) == false)
	{
		m_debugDirtyLighting.clear();
		UpdateDirtyLighting();
	}
		
	CheckAndRebuildChunkMesh();

	//DebugRenderLog(.1f, "Active Chunks: " + std::to_string(m_activeChunks.size()));
	DebugRenderLog(0.05f, std::to_string(m_debugDirtyLighting.size()), Rgba::YELLOW);
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
void World::CheckKeyboardInputs()
{
	if (IsDevConsoleOpen())
		return;
	
	if (DidMouseWheelScrollUp())
	{
		m_blockToPlace = BlockDefinition::GetNextBlockDefinition(1, m_blockToPlace);
	}

	if (DidMouseWheelScrollDown())
	{
		m_blockToPlace = BlockDefinition::GetNextBlockDefinition(-1, m_blockToPlace);
	}

	if(IsKeyPressed(KEYBOARD_SHIFT))
	{
		m_cameraSpeed = 50.f;
	}
	else
	{
		m_cameraSpeed = 10.f;
	}

	if (WasMouseButtonJustPressed(LEFT_MOUSE_BUTTON))
	{
		if (m_targetBlockRaycast.DidImpact())
		{
			BlockLocator& theLocator = m_targetBlockRaycast.m_impactBlock;
			Block& theBlockWeHit = theLocator.GetBlock();
			theBlockWeHit.m_type = BLOCK_TYPE_AIR;

			m_targetBlockRaycast.m_impactBlock.m_chunk->Dirty();

			// check to dirty neighbors if edge block
			if (theLocator.IsBlockOnEastEdge()) { theLocator.m_chunk->Dirty(); }
			if (theLocator.IsBlockOnWestEdge()) { theLocator.m_chunk->Dirty();}
			if (theLocator.IsBlockOnNorthEdge()) { theLocator.m_chunk->Dirty(); }
			if (theLocator.IsBlockOnSouthEdge()) { theLocator.m_chunk->Dirty(); }
		}
	}

	if (WasMouseButtonJustPressed(RIGHT_MOUSE_BUTTON))
	{
		BlockLocator& theLocator = m_targetBlockRaycast.m_impactBlock;
		BlockLocator nextToMeBlock = theLocator.GetBlockLocatorNextToMeFromNormal(m_targetBlockRaycast.m_impactNormal);

		if (nextToMeBlock.IsValid())
		{
			Block& blockToEdit = nextToMeBlock.GetBlock();
			blockToEdit.m_type = m_blockToPlace->m_type;

			nextToMeBlock.m_chunk->Dirty();

			// check to dirty neighbors if edge block
			if (nextToMeBlock.IsBlockOnEastEdge()) { nextToMeBlock.m_chunk->Dirty(); }
			if (nextToMeBlock.IsBlockOnWestEdge()) { nextToMeBlock.m_chunk->Dirty(); }
			if (nextToMeBlock.IsBlockOnNorthEdge()) { nextToMeBlock.m_chunk->Dirty(); }
			if (nextToMeBlock.IsBlockOnSouthEdge()) { nextToMeBlock.m_chunk->Dirty(); }
		}

	}

	DebugKeys();

	// do this last cause it'll move the mouse 
	UpdateCamera();
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
void World::FindPlayersTargetedBlock()
{
	if (!m_showTargettedBlockRaycast)
	{
		m_targetBlockRaycast = RayCast(m_gameCamera->pos, m_camera->GetForward(), 8.f);
	}
}

//-----------------------------------------------------------------------------------------------
void World::Render() const
{
	//Renderer* r = Renderer::GetInstance();

	// Set up Cameras
	m_camera->SetPerspective(45.f, (16.f / 9.f), .1f, 400.f);

	//m_camera->m_cameraMatrix = m_camera->transform.GetLocalMatrix();//Matrix44(); //modelMatrix;
	Matrix44 theModel = m_gameCamera->GetModelMatrix();
	Matrix44 theView = m_gameCamera->GetViewMatrix();
	m_camera->m_cameraMatrix = theModel;
	m_camera->m_viewMatrix = theView;

	RenderSkyBox();

	RenderChunks();
	RenderBasis();
	RenderTargetBlock();
	RenderTargettedBlockRaycast();

	if (g_gameConfigBlackboard.GetValue("useDebugLighting", false))
	{
		RenderDebugDirtyLighting();
	}

	m_playerHUD->Render();
}

//-----------------------------------------------------------------------------------------------
void World::RenderChunks() const
{
	Renderer* r = Renderer::GetInstance();
	
	Shader* chunkShader = Shader::CreateOrGetShader("default");
	chunkShader->SetCullMode(CULLMODE_FRONT);

	r->SetShader(chunkShader);
	r->SetCurrentTexture(0, g_blockSpriteSheet.m_spriteSheetTexture);
	r->SetCamera(m_camera);
	
	for (auto theIterator = m_activeChunks.begin(); theIterator != m_activeChunks.end(); theIterator++)
	{
		Mesh* theMesh = theIterator->second->m_gpuMesh;

		// sometimes the mesh isn't made yet
		if (theMesh != nullptr)
		{
			r->DrawMesh(theIterator->second->m_gpuMesh);
		}

		// debug stuff
		theIterator->second->Render();
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
void World::RenderDebugDirtyLighting() const
{
	Renderer* r = Renderer::GetInstance();
	MeshBuilder mb;
	mb.Begin(PRIMITIVE_POINTS, false);

	for (uint i = 0; i < m_debugDirtyLighting.size(); i++)
	{
		DebugPoint current = m_debugDirtyLighting.at(i);
		
		mb.AppendPoint(current.m_position, current.m_color);
	}

	mb.End();
	Mesh* theMesh = mb.CreateMesh<Vertex3D_PCU>();

	r->SetPointSize(5);
	r->DrawMesh(theMesh, true);
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

	Vector2 currentWorldPos = Vector2(
		(float)(furthestChunk->m_chunkCoords.x * CHUNK_SIZE_X),
		(float)(furthestChunk->m_chunkCoords.y * CHUNK_SIZE_Y));

	float distanceSquaredForCurrent = GetDistanceSquared(currentWorldPos, m_gameCamera->pos.xy());

	if (distanceSquaredForCurrent > CHUNK_DEACTIVATION_DISTANCE_SQUARED)
	{
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
void World::UpdateDirtyLighting()
{
	if (g_gameConfigBlackboard.GetValue("stepDebugLighting", false))
	{
		int amountInQueAtStart = m_dirtyBlocks.size();

		for (uint i = 0; i < amountInQueAtStart; i++)
		{
			ProcessDirtyLightBlock();
		}
	}
	else
	{
		while (m_dirtyBlocks.size() != 0)
		{
			// pops the front off and updates it's light
			ProcessDirtyLightBlock();
		}
	}
	

}

//-----------------------------------------------------------------------------------------------
void World::ProcessDirtyLightBlock()
{
	BlockLocator& theLocator = m_dirtyBlocks.front();

	if(theLocator.m_chunk != nullptr)
		theLocator.m_chunk->Dirty();
	
	Block& theBlock = theLocator.GetBlock();
	theBlock.ClearIsLightDirty();

	// get my neighbors
	BlockLocator eastNeighbor = theLocator.GetBlockLocatorOfEastNeighbor();
	BlockLocator westNeighbor = theLocator.GetBlockLocatorOfWestNeighbor();
	BlockLocator northNeighbor = theLocator.GetBlockLocatorOfNorthNeighbor();
	BlockLocator southNeighbor = theLocator.GetBlockLocatorOfSouthNeighbor();
	BlockLocator aboveNeighbor = theLocator.GetBlockLocatorOfAboveNeighbor();
	BlockLocator belowNeighbor = theLocator.GetBlockLocatorOfBelowNeighbor();

	Block& eastBlock = eastNeighbor.GetBlock();
	Block& westBlock = westNeighbor.GetBlock();
	Block& northBlock = northNeighbor.GetBlock();
	Block& southBlock = southNeighbor.GetBlock();
	Block& aboveBlock = aboveNeighbor.GetBlock();
	Block& belowBlock = belowNeighbor.GetBlock();

	int maxIndoorLightValue = 0;
	int maxOutdoorLightValue = 0;

	// outdoor lighting check
	if (theBlock.IsSky())
	{
		maxOutdoorLightValue = MAX_LIGHT_VALUE;
	}
	else
	{
		int eastNeighborLightValue		=	eastBlock.GetOutdoorLightLevel();
		int westNeighborLightValue		=	westBlock.GetOutdoorLightLevel();
		int northNeightborLightValue	=	northBlock.GetOutdoorLightLevel();
		int southNeightborLightValue	=	southBlock.GetOutdoorLightLevel();
		int aboveNeightborLightValue	=	aboveBlock.GetOutdoorLightLevel();
		int bottomNeightborLightValue	=	belowBlock.GetOutdoorLightLevel();

		if (eastNeighborLightValue > maxOutdoorLightValue)
			maxOutdoorLightValue = eastNeighborLightValue;
		if (westNeighborLightValue > maxOutdoorLightValue)
			maxOutdoorLightValue = westNeighborLightValue;
		if (northNeightborLightValue > maxOutdoorLightValue)
			maxOutdoorLightValue = northNeightborLightValue;
		if (southNeightborLightValue > maxOutdoorLightValue)
			maxOutdoorLightValue = southNeightborLightValue;
		if (aboveNeightborLightValue > maxOutdoorLightValue)
			maxOutdoorLightValue = maxOutdoorLightValue;
		if (bottomNeightborLightValue > maxOutdoorLightValue)
			maxOutdoorLightValue = bottomNeightborLightValue;
	}

	// indoor lighting check
	if (!theBlock.IsFullyOpaque())
	{
		int eastNeighborLightValue		=	eastBlock.GetIndoorLightLevel();
		int westNeighborLightValue		=	westBlock.GetIndoorLightLevel();
		int northNeightborLightValue	=	northBlock.GetIndoorLightLevel();
		int southNeightborLightValue	=	southBlock.GetIndoorLightLevel();
		int aboveNeightborLightValue	=	aboveBlock.GetIndoorLightLevel();
		int bottomNeightborLightValue	=	belowBlock.GetIndoorLightLevel();

		if (eastNeighborLightValue > maxIndoorLightValue)
			maxIndoorLightValue = eastNeighborLightValue;
		if (westNeighborLightValue > maxIndoorLightValue)
			maxIndoorLightValue = westNeighborLightValue;
		if (northNeightborLightValue > maxIndoorLightValue)
			maxIndoorLightValue = northNeightborLightValue;
		if (southNeightborLightValue > maxIndoorLightValue)
			maxIndoorLightValue = southNeightborLightValue;
		if (aboveNeightborLightValue > maxIndoorLightValue)
			maxIndoorLightValue = maxOutdoorLightValue;
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

	// Check to see if we changed (were wrong!), if we did, dirty the neighbors
	bool dirtyNeighbors = false;
	if (theBlock.GetIndoorLightLevel() != expectedIndoorValue)
	{
		theBlock.SetIndoorLightLevel(expectedIndoorValue);
		dirtyNeighbors = true;
	}

	if (theBlock.GetOutdoorLightLevel() != expectedOutdoorValue)
	{
		theBlock.SetOutdoorLightLevel(expectedOutdoorValue);
		dirtyNeighbors = true;
	}

	// spread the love
	if (dirtyNeighbors)
	{
		MarkLightingDirty(eastNeighbor);
		MarkLightingDirty(westNeighbor);
		MarkLightingDirty(northNeighbor);
		MarkLightingDirty(southNeighbor);
		MarkLightingDirty(aboveNeighbor);
		MarkLightingDirty(belowNeighbor);
	}

	m_dirtyBlocks.pop_front();
}

//-----------------------------------------------------------------------------------------------
void World::MarkLightingDirty(BlockLocator & blockToDirty)
{
	Block& theBlock = blockToDirty.GetBlock();

	// we mark them dirty when they are in the list, so if its already dirty,
	// its already in the list. Early out
	if (theBlock.IsLightDirty())
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
