#include "Chunk.hpp"
#include "Game/General/World/BlockDefinition.hpp"
#include "Engine/ThirdParty/SquirrelNoise/SmoothNoise.hpp"
#include "Engine/Math/MathUtils.hpp"


//===============================================================================================
Chunk::Chunk(const ChunkCoords& myCoords)
	: m_chunkCoords(myCoords)
{	
	GenerateMyBounds();
	//GenerateBlocks();
	//GenerateMesh();
	GenerateTestMesh();
}

//-----------------------------------------------------------------------------------------------
Chunk::~Chunk()
{
	if(m_gpuMesh != nullptr)
		delete m_gpuMesh;
}

//-----------------------------------------------------------------------------------------------
void Chunk::Update()
{
}

//-----------------------------------------------------------------------------------------------
void Chunk::Render() const
{
	Renderer* r = Renderer::GetInstance();
	r->SetCurrentTexture(0, r->m_testTexture);
	r->DrawMesh(m_debugMesh);

}

//-----------------------------------------------------------------------------------------------
void Chunk::GenerateMyBounds()
{
	m_bounds.mins.x = (float)(m_chunkCoords.x * CHUNK_SIZE_X);
	m_bounds.mins.y = (float)(m_chunkCoords.y * CHUNK_SIZE_Y);
	m_bounds.mins.z = 0.f;
	
	m_bounds.maxs.x = m_bounds.mins.x + ((float)CHUNK_SIZE_X);
	m_bounds.maxs.y = m_bounds.mins.y + ((float)CHUNK_SIZE_Y);
	m_bounds.maxs.z = (float)CHUNK_HEIGHT;
}

//-----------------------------------------------------------------------------------------------
void Chunk::GenerateBlocks()
{
	float seaLevel = (float)(CHUNK_HEIGHT / 2);
	float grassLevel[CHUNK_SIZE_X * CHUNK_SIZE_Y];
	// shift alr r
	for (int blockY = 0; blockY < CHUNK_SIZE_Y; blockY++)
	{
		for (int blockX = 0; blockX < CHUNK_SIZE_X; blockX++)
		{
			Vector3 blockWorldCoords = GetWorldPositionOfColumn(blockX,blockY);
			float amount =  Compute2dPerlinNoise(
				blockWorldCoords.x,
				blockWorldCoords.y,
				300.f, 5);
				//* 5.f;

			int columnindex = (blockY * CHUNK_SIZE_X) + blockX;
			grassLevel[columnindex] = RangeMapFloat(amount, -1.f, 1.f, seaLevel, (float)CHUNK_HEIGHT * .3f);
			//grassLevel[xDir * yDir] = seaLevel + amount;
		}
	}

	for(int blockZ = 0; blockZ < CHUNK_HEIGHT; blockZ++)
	{
		for(int blockY = 0; blockY < CHUNK_SIZE_Y; blockY++)
		{
			for(int blockX = 0; blockX < CHUNK_SIZE_X; blockX++)
			{
// 				if(blockZ < seaLevel)
// 				{
// 					SetBlockType(blockX, blockY, blockZ, "stone");
// 				}
// 				else
// 				{
				int columnindex = (blockY * CHUNK_SIZE_X) + blockX;	
				int theGrassLevel = (int) grassLevel[columnindex];

					if (blockZ > (theGrassLevel))
					{
						SetBlockType(blockX, blockY, blockZ, "air");
					}
					else if (blockZ < (theGrassLevel - 3))
					{
						SetBlockType(blockX, blockY, blockZ, "stone");
					}
					else
					{
						SetBlockType(blockX, blockY, blockZ, "grass");
					}
			}
				//else if( blockZ >= seaLevel && blockZ < seaLevel + 3)
				//{
				//	SetBlockType(blockX, blockY, blockZ, "grass");
				//}
				//else
				//{
				//	SetBlockType(blockX, blockY, blockZ, "air");
				//}
			
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
void Chunk::GenerateTestMesh()
{
	MeshBuilder mb;	

	AABB2 bounds = AABB2(m_bounds.mins.xy(), m_bounds.maxs.xy());
	bounds.AddPaddingToSides(-4.f, -4.f);
	
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	mb.SetColor(Rgba::WHITE);
	
	AABB2 uvs = AABB2(0.f, 0.f, 1.f, 1.f);
	mb.SetUV(uvs.maxs.x, uvs.mins.y);
	uint idx = mb.PushVertex(Vector3(bounds.maxs.x, bounds.mins.y, m_bounds.maxs.z));

	//bl
	mb.SetUV(uvs.mins.x, uvs.mins.y);
	mb.PushVertex(Vector3(bounds.mins.x, bounds.mins.y, m_bounds.maxs.z));

	// tl
	mb.SetUV(uvs.mins.x, uvs.maxs.y);
	mb.PushVertex(Vector3(bounds.mins.x, bounds.maxs.y, m_bounds.maxs.z));

	// tr
	mb.SetUV(uvs.maxs.x, uvs.maxs.y);
	mb.PushVertex(Vector3(bounds.maxs.x, bounds.maxs.y, m_bounds.maxs.z));

	mb.AddFace(idx + 0, idx + 1, idx + 2);
	mb.AddFace(idx + 2, idx + 3, idx + 0);

	m_debugMesh = mb.CreateMesh<Vertex3D_PCU>();
}

Vector3 Chunk::GetWorldPositionOfColumn(int theX, int theY)
{
	float howFarWeAreInX = (float)((m_chunkCoords.x * CHUNK_SIZE_X) + theX);
	float howFarWeAreInY = (float)((m_chunkCoords.y * CHUNK_SIZE_Y) + theY);
	
	//float x =  howFarWeAreInX * CHUNK_SIZE_X;
	//float y =  howFarWeAreInY * CHUNK_SIZE_Y;

	return Vector3(howFarWeAreInX, howFarWeAreInY, 0.f);
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
	Vector3 center = Vector3(m_chunkCoords.GetAsVector2() * CHUNK_SIZE_X,0) + theCoords.GetAsVector3() + Vector3(.5f);
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