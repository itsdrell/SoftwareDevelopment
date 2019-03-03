#pragma once
#include "Game/Main/GameCommon.hpp"
#include <map>
#include "../Utils/BlockLocator.hpp"
#include <deque>

//====================================================================================
// Forward Declare
//====================================================================================
class Chunk;
class GameCamera;
class Camera;
class TextureCube;
class Mesh;
class Neighborhood;
class HUD;

//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================
struct RaycastResult
{
	Vector3			m_startPos			= Vector3::ZERO;
	Vector3			m_direction			= Vector3::ZERO;
	float			m_maxDistance		= 0.f;
	Vector3			m_endPosition		= Vector3::ZERO;
	Vector3			m_impactPosition	= 0.f;
	float			m_impactFraction	= 0.f;
	float			m_impactDistance	= 0.f;
	BlockLocator	m_impactBlock		= BlockLocator(nullptr, -1);
	Vector3			m_impactNormal		= Vector3::ZERO;

	bool DidImpact() const { return m_impactFraction < 1.f; }
};

//-----------------------------------------------------------------------------------------------
struct DebugPoint
{
	DebugPoint(const Vector3& pos, const Rgba& color = Rgba::YELLOW)
		:	m_position(pos),
			m_color(color) {}

	Vector3		m_position;
	Rgba		m_color;
};

//====================================================================================
// Classes
//====================================================================================
class World
{
public:
	World();
	~World();

public:
	void Update();
	void UpdateChunks();
	void CheckKeyboardInputs();
	void DebugKeys();
	void UpdateCamera();
	void FindPlayersTargetedBlock();

public:
	void Render() const;
	void RenderChunks() const;
	void RenderSkyBox() const;
	void RenderBasis() const;
	void RenderTargettedBlockRaycast() const;
	void RenderTargetBlock() const;
	void RenderDebugDirtyLighting() const;

public:
	void CheckAndActivateChunk();
	void CheckAndDeactivateChunk();
	void CheckAndRebuildChunkMesh();
	void ActivateChunk(const ChunkCoords& theCoords);
	bool IsChunkActivated(const ChunkCoords& theCoords);
	Chunk* GetFarthestChunkFromPlayer(const Vector2& playerWorldPos);
	Chunk* GetChunkFromChunkCoords( const ChunkCoords& theCoords );

public:
	void UpdateDirtyLighting();
	void ProcessDirtyLightBlock();
	void MarkLightingDirty( BlockLocator& blockToDirty );
	void UndirtyAllBlocksInChunk( const Chunk* theChunk );

public:
	RaycastResult RayCast(const Vector3& start, const Vector3& forward, float maxDistance);

public:
	std::map<ChunkCoords, Chunk*>	m_activeChunks;

private:
	std::deque<BlockLocator>		m_dirtyBlocks;
	std::vector<DebugPoint>			m_debugDirtyLighting;

public:
	Camera*					m_camera = nullptr;
	GameCamera*				m_gameCamera = nullptr;
	float					m_cameraSpeed = 10.f;

	BlockDefinition*		m_blockToPlace = nullptr;
	HUD*					m_playerHUD = nullptr;

	TextureCube*			m_skyBox = nullptr;
	Mesh*					m_skyMesh = nullptr;

	Neighborhood*			m_chunkActivationCheatSheet = nullptr;
	
	bool					m_showTargettedBlockRaycast = false;
	RaycastResult			m_targetBlockRaycast;

};

//====================================================================================
// Standalone C Functions
//====================================================================================

//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [2/6/2019]
//====================================================================================