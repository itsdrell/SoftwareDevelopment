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


	void GenerateMyBounds();
	void GenerateBlocks();
	void GenerateMesh();

	void SetBlockType(int blockX, int blockY, int blockZ, const String& name);
	void AddVertsForBlock( BlockIndex theIndex );

	BlockIndex GetBlockIndexForBlockCoords(const BlockCoords& bc);
	BlockCoords GetBlockCoordsForBlockIndex( BlockIndex bi);


public:
	Block			m_blocks[AMOUNT_OF_BLOCKS_IN_CHUNK];
	ChunkCoords		m_chunkCoords;
	AABB3			m_bounds;
	MeshBuilder		m_cpuMesh;
	Mesh*			m_gpuMesh = nullptr; // can be null
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