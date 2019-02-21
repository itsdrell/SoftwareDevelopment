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
struct ChunkHeader
{
	unsigned char	m_4cc[4]			= { 'S', 'M','C', 'D' };
	unsigned char	m_version			= 1;
	unsigned char	m_chunkBitsX		= CHUNK_BITS_WIDE_X;
	unsigned char	m_chunkBitsY		= CHUNK_BITS_WIDE_Y;
	unsigned char	m_chunkBitsZ		= CHUNK_BITS_TALL_Z;
	unsigned char	m_reserved1			= 0;
	unsigned char	m_reserver2			= 0;
	unsigned char	m_reserved3			= 0;
	unsigned char	m_format			= 'R';

	bool IsValid();
};

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

	bool LoadFromFile();
	void SaveToFile();
	void WriteHeaderToBuffer();
	String GetNameOfFileFromChunkCoords(const ChunkCoords& theCoords);

public:
	void GenerateMyBounds();
	void GenerateBlocks();
	void GenerateBlocksFromFile();
	void GenerateMesh();
	void GenerateTestMesh();

public:

	Vector3 GetWorldPositionOfColumn(int theX, int theY);
	void SetBlockType(int blockX, int blockY, int blockZ, const String& name);
	void AddVertsForBlock( BlockIndex theIndex );
	bool CanRebuildItsMesh();
	void Dirty();
	void LeaveYourNeighbors();

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
	bool			m_hasBeenModified = false;

private:
	std::vector<unsigned char>		m_fileData;
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