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
class Player;
class Entity;

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

//-----------------------------------------------------------------------------------------------
struct WorldTime
{
public:
	void Advance(float ds);
	int GetDay();
	int GetHour();
	float GetNormalizedPercentThroughDay();
	bool IsNightTime();
	
public:
	float m_time = 0.f;
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
	void UpdateSky();
	void UpdateCamera();
	void UpdateEntities();
	void CheckKeyboardInputs();
	void DebugKeys();
	void FindPlayersTargetedBlock();
	void AddSkyDebugPointsForChunkIAmOn();

public:
	void Render() const;
	void RenderSky() const;
	void RenderChunks() const;
	void RenderSkyBox() const;
	void RenderBasis() const;
	void RenderTargettedBlockRaycast() const;
	void RenderTargetBlock() const;
	void RenderDebugPoints() const;
	void RenderEntities() const;

public:
	void AddPlayer(const Vector3& position);

public:
	void DebugValidateAllChunks();
	void CheckAndActivateChunk();
	void CheckAndDeactivateChunk();
	void CheckAndRebuildChunkMesh();
	void ActivateChunk(const ChunkCoords& theCoords);
	bool IsChunkActivated(const ChunkCoords& theCoords);
	Chunk* GetFarthestChunkFromPlayer(const Vector2& playerWorldPos);
	Chunk* GetChunkFromChunkCoords( const ChunkCoords& theCoords );
	void HammerCameraValuesToEngineCamera() const;

public:
	void UpdateDirtyLighting();
	void ProcessDirtyLightBlock();
	void MarkLightingDirty( BlockLocator& blockToDirty );
	void UndirtyAllBlocksInChunk( const Chunk* theChunk );

public:
	void AddSkyDebugPoint(const Vector3& pos);

public:
	RaycastResult RayCast(const Vector3& start, const Vector3& forward, float maxDistance);

public:
	std::map<ChunkCoords, Chunk*>	m_activeChunks;

private:
	std::deque<BlockLocator>		m_dirtyBlocks;
	std::vector<DebugPoint>			m_debugDirtyLighting;
	std::vector<DebugPoint>			m_debugSkyPoints;

public:
	Player*					m_player = nullptr;
	GameCamera*				m_gameCamera = nullptr;
	std::vector<Entity*>	m_entities;

public:
	Camera*					m_camera = nullptr;
	float					m_cameraSpeed = 10.f;

	TextureCube*			m_skyBox = nullptr;
	Mesh*					m_skyMesh = nullptr;

	Neighborhood*			m_chunkActivationCheatSheet = nullptr;
	
	bool					m_showTargettedBlockRaycast = false;
	RaycastResult			m_targetBlockRaycast;

	WorldTime				m_theWorldTime;
	int						m_worldScale = 1;

	Rgba					m_skyColor;
	Rgba					m_indoorLightColor;
	float					m_indoorLightFlickerStrength = 0.8f;

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