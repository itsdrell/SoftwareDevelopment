#pragma once
#include "Game/Main/GameCommon.hpp"
#include <map>

//====================================================================================
// Forward Declare
//====================================================================================
class Chunk;
class GameCamera;
class Camera;
class TextureCube;
class Mesh;
class Neighborhood;

//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================


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
	void UpdateCamera();

	void Render() const;
	void RenderChunks() const;
	void RenderSkyBox() const;
	void RenderBasis() const;

public:
	void CheckAndActivateChunk();
	void CheckAndDeactivateChunk();
	void CheckAndRebuildChunkMesh();
	void ActivateChunk(const ChunkCoords& theCoords);
	bool IsChunkActivated(const ChunkCoords& theCoords);
	Chunk* GetFarthestChunkFromPlayer(const Vector2& playerWorldPos);
	Chunk* GetChunkFromChunkCoords( const ChunkCoords& theCoords );


public:
	std::map<ChunkCoords, Chunk*> m_activeChunks;

public:
	Camera*					m_camera = nullptr;
	GameCamera*				m_gameCamera = nullptr;
	float					m_cameraSpeed = 10.f;

	TextureCube*			m_skyBox = nullptr;
	Mesh*					m_skyMesh = nullptr;

	Neighborhood*			m_chunkActivationCheatSheet = nullptr;

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