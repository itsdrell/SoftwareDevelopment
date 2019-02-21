#pragma once
#include "Game/General/World/Block.hpp"
#include "Game/Main/GameCommon.hpp"
#include "Engine/Renderer/Systems/MeshBuilder.hpp"


//====================================================================================
// Forward Declare
//====================================================================================
class Mesh;

//====================================================================================
// Type Defs + Defines
//====================================================================================
constexpr int AMOUNT_OF_BLOCKS_IN_CHUNK = 65536;

//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class Chunk
{
public:
	Chunk(const ChunkCoords& myCoords);
	~Chunk();

	void Update();
	void Render() const;

public:
	void GenerateMyBounds();
	void GenerateBlocks();
	void GenerateMesh();
	void GenerateTestMesh();

public:

	Vector3 GetWorldPositionOfColumn(int theX, int theY);
	void SetBlockType(int blockX, int blockY, int blockZ, const String& name);
	void AddVertsForBlock( BlockIndex theIndex );
	bool CanRebuildItsMesh();

public:
	BlockIndex GetBlockIndexForWorldCoords(const Vector3& worldPos);
	static BlockCoords GetBlockCoordsForBlockIndex( BlockIndex bi);
	static BlockIndex GetBlockIndexForBlockCoords(const BlockCoords& bc);
	static ChunkCoords GetChunkCoordsFromWorldPosition(const Vector3& worldPos);


public:
	Chunk*			m_northNeighbor = nullptr;
	Chunk*			m_southNeighbor = nullptr;
	Chunk*			m_eastNeighbor = nullptr;
	Chunk*			m_westNeighbor = nullptr;

public:
	Block			m_blocks[AMOUNT_OF_BLOCKS_IN_CHUNK];
	ChunkCoords		m_chunkCoords;
	AABB3			m_bounds;
	MeshBuilder		m_cpuMesh;
	Mesh*			m_gpuMesh = nullptr; // can be null
	Mesh*			m_debugMesh = nullptr;
	bool			m_isGPUDirty; // so we can rebuild over multiple frames then set 
};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [2/7/2019]
//====================================================================================