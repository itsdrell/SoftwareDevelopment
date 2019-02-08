#include "Chunk.hpp"
#include "Game/General/World/BlockDefinition.hpp"


//===============================================================================================
Chunk::Chunk(const ChunkCoords& myCoords)
	: m_chunkCoords(myCoords)
{	
	GenerateMyBounds();
	GenerateBlocks();
	GenerateMesh();
}

//-----------------------------------------------------------------------------------------------
Chunk::~Chunk()
{
	if(m_gpuMesh != nullptr)
		delete m_gpuMesh;
}

//-----------------------------------------------------------------------------------------------
void Chunk::GenerateMyBounds()
{
	m_bounds.mins.x = (float)(m_chunkCoords.x * CHUNK_SIZE_X);
	m_bounds.mins.y = (float)(m_chunkCoords.y * CHUNK_SIZE_Y);
	m_bounds.mins.z = 0.f;

	m_bounds.maxs.x = m_bounds.mins.x + ((float) CHUNK_SIZE_X);
	m_bounds.maxs.y = m_bounds.mins.y + ((float) CHUNK_SIZE_Y);
	m_bounds.maxs.z = (float) CHUNK_HEIGHT;
}

//-----------------------------------------------------------------------------------------------
void Chunk::GenerateBlocks()
{
	int seaLevel = CHUNK_HEIGHT / 2;

	for(uint blockZ = 0; blockZ < CHUNK_HEIGHT; blockZ++)
	{
		for(uint blockY = 0; blockY < CHUNK_SIZE_Y; blockY++)
		{
			for(uint blockX = 0; blockX < CHUNK_SIZE_X; blockX++)
			{
				if(blockZ < seaLevel)
				{
					SetBlockType(blockX, blockY, blockZ, "stone");
				}
				else if( blockZ >= seaLevel && blockZ < seaLevel + 3)
				{
					SetBlockType(blockX, blockY, blockZ, "grass");
				}
				else
				{
					SetBlockType(blockX, blockY, blockZ, "air");
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Chunk::GenerateMesh()
{
	m_cpuMesh.Clear(); // this happens when we make a mesh so its redundant but explicit
	m_cpuMesh.Begin(PRIMITIVE_TRIANGLES, true);

	for(uint i = 0; i < AMOUNT_OF_BLOCKS_IN_CHUNK; i++)
	{
		AddVertsForBlock(i);
	}

	if(m_gpuMesh != nullptr)
		delete m_gpuMesh;

	m_gpuMesh = m_cpuMesh.CreateMesh<Vertex3D_PCU>();
}

//-----------------------------------------------------------------------------------------------
void Chunk::SetBlockType(int blockX, int blockY, int blockZ, const String & name)
{
	BlockIndex theIndex = GetBlockIndexForBlockCoords(BlockCoords(blockX, blockY, blockZ));
	Block& theBlock = m_blocks[theIndex];

	theBlock.m_type = BlockDefinition::GetDefinitionByName(name)->m_type;
}

//-----------------------------------------------------------------------------------------------
void Chunk::AddVertsForBlock(BlockIndex theIndex)
{
	Block& theBlock = m_blocks[theIndex];

	if(theBlock.m_type == BLOCK_TYPE_AIR)
		return;

	BlockDefinition* theDefinition = BlockDefinition::GetDefinitionByType( (BlockTypes) theBlock.m_type);
	BlockCoords theCoords = GetBlockCoordsForBlockIndex(theIndex);
	Vector3 center = theCoords.GetAsVector3() + Vector3(.5f);
	Vector3 dimensions = Vector3(.5f);

	m_cpuMesh.SetColor(Rgba::WHITE);

	//-----------------------------------------------------------------------------------------------
	// Top
	// br
	AABB2 uvs = theDefinition->m_topUVs;

	m_cpuMesh.SetUV(uvs.maxs.x, uvs.mins.y);
	uint idx = m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x),	(center.y - dimensions.y),	(center.z + dimensions.z)));

	//bl
	m_cpuMesh.SetUV(uvs.mins.x, uvs.mins.y);
	m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x),	(center.y + dimensions.y),	(center.z + dimensions.z)));

	// tl
	m_cpuMesh.SetUV(uvs.mins.x,uvs.maxs.y);
	m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x),	(center.y + dimensions.y),	(center.z + dimensions.z)));

	// tr
	m_cpuMesh.SetUV(uvs.maxs.x, uvs.maxs.y);
	m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x),	(center.y - dimensions.y),	(center.z + dimensions.z)));

	m_cpuMesh.AddFace(idx + 0, idx + 1, idx + 2);
	m_cpuMesh.AddFace(idx + 2, idx + 3, idx + 0);

	//-----------------------------------------------------------------------------------------------
	// Front face
	uvs = theDefinition->m_sideUVs;

	m_cpuMesh.SetUV(uvs.maxs.x, uvs.mins.y);
	idx = m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x),	(center.y - dimensions.y),	(center.z - dimensions.z)));

	//bl
	m_cpuMesh.SetUV(uvs.mins.x, uvs.mins.y);
	m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x),	(center.y + dimensions.y),	(center.z - dimensions.z)));

	// tl
	m_cpuMesh.SetUV(uvs.mins.x,uvs.maxs.y);
	m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x),	(center.y + dimensions.y),	(center.z + dimensions.z)));

	// tr
	m_cpuMesh.SetUV(uvs.maxs.x, uvs.maxs.y);
	m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x),	(center.y - dimensions.y),	(center.z + dimensions.z)));

	m_cpuMesh.AddFace(idx + 0, idx + 1, idx + 2);
	m_cpuMesh.AddFace(idx + 2, idx + 3, idx + 0);

	//-----------------------------------------------------------------------------------------------
	// left
	m_cpuMesh.SetUV(uvs.maxs.x, uvs.mins.y);
	idx = m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x),	(center.y + dimensions.y),	(center.z - dimensions.z)));

	//bl
	m_cpuMesh.SetUV(uvs.mins.x, uvs.mins.y);
	m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x),	(center.y + dimensions.y),	(center.z - dimensions.z)));

	// tl
	m_cpuMesh.SetUV(uvs.mins.x,uvs.maxs.y);
	m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x),	(center.y + dimensions.y),	(center.z + dimensions.z)));

	// tr
	m_cpuMesh.SetUV(uvs.maxs.x, uvs.maxs.y);
	m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x),	(center.y + dimensions.y),	(center.z + dimensions.z)));

	m_cpuMesh.AddFace(idx + 0, idx + 1, idx + 2);
	m_cpuMesh.AddFace(idx + 2, idx + 3, idx + 0);

	//-----------------------------------------------------------------------------------------------
	// back
	m_cpuMesh.SetUV(uvs.maxs.x, uvs.mins.y);
	idx = m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x),	(center.y - dimensions.y),	(center.z - dimensions.z)));

	//bl
	m_cpuMesh.SetUV(uvs.mins.x, uvs.mins.y);
	m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x),	(center.y + dimensions.y),	(center.z - dimensions.z)));

	// tl
	m_cpuMesh.SetUV(uvs.mins.x,uvs.maxs.y);
	m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x),	(center.y + dimensions.y),	(center.z + dimensions.z)));

	// tr
	m_cpuMesh.SetUV(uvs.maxs.x, uvs.maxs.y);
	m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x),	(center.y - dimensions.y),	(center.z + dimensions.z)));

	m_cpuMesh.AddFace(idx + 0, idx + 1, idx + 2);
	m_cpuMesh.AddFace(idx + 2, idx + 3, idx + 0);

	//-----------------------------------------------------------------------------------------------
	// right face
	m_cpuMesh.SetUV(uvs.maxs.x, uvs.mins.y);
	idx = m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x),	(center.y - dimensions.y),	(center.z - dimensions.z)));

	//bl
	m_cpuMesh.SetUV(uvs.mins.x, uvs.mins.y);
	m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x),	(center.y - dimensions.y),	(center.z - dimensions.z)));

	// tl
	m_cpuMesh.SetUV(uvs.mins.x,uvs.maxs.y);
	m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x),	(center.y - dimensions.y),	(center.z + dimensions.z)));

	// tr
	m_cpuMesh.SetUV(uvs.maxs.x, uvs.maxs.y);
	m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x),	(center.y - dimensions.y),	(center.z + dimensions.z)));

	m_cpuMesh.AddFace(idx + 0, idx + 1, idx + 2);
	m_cpuMesh.AddFace(idx + 2, idx + 3, idx + 0);

	//-----------------------------------------------------------------------------------------------
	// Bottom Face
	uvs = theDefinition->m_bottomUVs;
	m_cpuMesh.SetUV(uvs.maxs.x, uvs.mins.y);
	idx = m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x),	(center.y - dimensions.y),	(center.z - dimensions.z)));

	//bl
	m_cpuMesh.SetUV(uvs.mins.x, uvs.mins.y);
	m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x),	(center.y + dimensions.y),	(center.z - dimensions.z)));

	// tl
	m_cpuMesh.SetUV(uvs.mins.x,uvs.maxs.y);
	m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x),	(center.y + dimensions.y),	(center.z - dimensions.z)));

	// tr
	m_cpuMesh.SetUV(uvs.maxs.x, uvs.maxs.y);
	m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x),	(center.y - dimensions.y),	(center.z - dimensions.z)));

	m_cpuMesh.AddFace(idx + 0, idx + 1, idx + 2);
	m_cpuMesh.AddFace(idx + 2, idx + 3, idx + 0);

}

//-----------------------------------------------------------------------------------------------
BlockIndex Chunk::GetBlockIndexForBlockCoords(const BlockCoords & bc)
{
	// Indexing an array in 3D:  x + (y * size.x) + (z * (size.x * size.y))
	return bc.x + (bc.y * CHUNK_SIZE_X) + (bc.z * (CHUNK_SIZE_X * CHUNK_SIZE_Y));
}

//-----------------------------------------------------------------------------------------------
BlockCoords Chunk::GetBlockCoordsForBlockIndex(BlockIndex bi)
{
	int z = bi >> (CHUNK_BITS_WIDE_X + CHUNK_BITS_WIDE_Y);
	int y = (bi & CHUNK_Y_MASK) >> CHUNK_BITS_WIDE_X;
	int x = bi & CHUNK_X_MASK;
	
	return BlockCoords(x,y,z);
}
