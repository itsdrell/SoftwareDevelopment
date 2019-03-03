#include "Chunk.hpp"
#include "Game/General/World/BlockDefinition.hpp"
#include "Engine/ThirdParty/SquirrelNoise/SmoothNoise.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "../Utils/BlockLocator.hpp"
#include <stdio.h>
#include "Engine/Core/Tools/DevConsole.hpp"
#include "Game/Main/Game.hpp"
#include "Game/General/World/World.hpp"


//===============================================================================================
Chunk::Chunk(const ChunkCoords& myCoords)
	: m_chunkCoords(myCoords)
{	
	GenerateMyBounds();
	if (LoadFromFile())
	{
		GenerateBlocksFromFile();
	}
	else
	{
		GenerateBlocks();
	}
	GenerateTestMesh();

	m_isGPUDirty = true;
	m_cpuMesh.ReserveSpace(10'000);
}

//-----------------------------------------------------------------------------------------------
Chunk::~Chunk()
{
	if(m_gpuMesh != nullptr)
		delete m_gpuMesh;

	m_cpuMesh.Clear();

	if (m_hasBeenModified)
		SaveToFile();
}

//-----------------------------------------------------------------------------------------------
void Chunk::Update()
{
}

//-----------------------------------------------------------------------------------------------
void Chunk::Render() const
{
	Renderer* r = Renderer::GetInstance();
	if (m_isGPUDirty && m_gpuMesh == nullptr)
		r->SetCurrentTexture(0, r->m_defaultNormalTexture);
	else
		r->SetCurrentTexture(0, r->m_testTexture);
	
	r->m_currentShader->SetCullMode(CULLMODE_NONE);
	r->DrawMesh(m_debugMeshTop);
	r->DrawMesh(m_debugMeshBottom);
	r->SetCurrentTexture(0, g_blockSpriteSheet.m_spriteSheetTexture);
	r->m_currentShader->SetCullMode(CULLMODE_FRONT);
}

//-----------------------------------------------------------------------------------------------
bool Chunk::LoadFromFile()
{
	String fullPath = "Saves/" + GetNameOfFileFromChunkCoords(m_chunkCoords);
	FILE* theFile;
	fopen_s(&theFile, fullPath.c_str(), "rb");

	if (theFile == NULL)
		return false;

	// read the buffer
	int c; // note: int, not char, required to handle EOF
	while ((c = fgetc(theFile)) != EOF) 
	{ 
		putchar(c);
		m_fileData.push_back((unsigned char)c);
	}

	// terminate
	fclose(theFile);
	return true;
}

//-----------------------------------------------------------------------------------------------
void Chunk::SaveToFile()
{
	String fullPath = "Saves/" + GetNameOfFileFromChunkCoords(m_chunkCoords);
	FILE* theFile;
	fopen_s(&theFile, fullPath.c_str(), "wb");

	if (theFile == NULL)
		return;

	m_fileData.reserve(500);
	WriteHeaderToBuffer();

	unsigned char currentBlockType = BLOCK_TYPE_TEST;
	uint amountOfBlocks = 0;
	for(uint i = 0; i < AMOUNT_OF_BLOCKS_IN_CHUNK; i++)
	{
		Block& currentBlock = m_blocks[i];

		if (currentBlock.m_type != currentBlockType)
		{
			m_fileData.push_back(currentBlockType);
			m_fileData.push_back((unsigned char)amountOfBlocks);

			amountOfBlocks = 1;
			currentBlockType = currentBlock.m_type;
		}
		else
		{
			amountOfBlocks++;

			// need to do this before checking for next block?
			if (amountOfBlocks == 255)
			{
				m_fileData.push_back(currentBlockType);
				m_fileData.push_back((unsigned char)255);

				amountOfBlocks = 0U;
			}
		}
	}

	// make sure to add the last part
	m_fileData.push_back(currentBlockType);
	m_fileData.push_back((unsigned char)amountOfBlocks);

	fwrite(m_fileData.data(), sizeof(unsigned char), sizeof(unsigned char) * m_fileData.size(), theFile);

	// terminate
	fclose(theFile);
}

//-----------------------------------------------------------------------------------------------
void Chunk::WriteHeaderToBuffer()
{
	ChunkHeader theHeader;
	
	m_fileData.push_back(theHeader.m_4cc[0]);
	m_fileData.push_back(theHeader.m_4cc[1]);
	m_fileData.push_back(theHeader.m_4cc[2]);
	m_fileData.push_back(theHeader.m_4cc[3]);
	
	m_fileData.push_back(theHeader.m_version);
	m_fileData.push_back(theHeader.m_chunkBitsX);
	m_fileData.push_back(theHeader.m_chunkBitsY);
	m_fileData.push_back(theHeader.m_chunkBitsZ);

	m_fileData.push_back(theHeader.m_reserved1);
	m_fileData.push_back(theHeader.m_reserver2);
	m_fileData.push_back( theHeader.m_reserved3);

	m_fileData.push_back( theHeader.m_format);
}

//-----------------------------------------------------------------------------------------------
String Chunk::GetNameOfFileFromChunkCoords(const ChunkCoords& theCoords)
{
	return Stringf("Chunk_%i,%i.chunk", theCoords.x, theCoords.y);
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
	float seaLevel = (((float)CHUNK_HEIGHT) * .6f);
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
				300.f, 1);
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
					else if (blockZ == seaLevel)
					{
						SetBlockType(blockX, blockY, blockZ, "snow");
					}
					else
					{
						SetBlockType(blockX, blockY, blockZ, "snow");
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
void Chunk::GenerateBlocksFromFile()
{
	ChunkHeader* theHeader = (ChunkHeader*)m_fileData.data();

	if (!theHeader->IsValid())
	{
		GenerateBlocks(); 
		return;
	}
		
	uint currentBlock = 0;
	for (uint i = sizeof(ChunkHeader); i < m_fileData.size(); i+=2)
	{
		unsigned char type = m_fileData.at(i);
		uint amountOfBlocks = (uint) m_fileData.at(i + 1);

		for (uint blockIndex = 0; blockIndex < amountOfBlocks; blockIndex++)
		{
			//Block& theBlock = m_blocks[currentBlock];
			//theBlock.m_type = type;

			SetBlockType(currentBlock, (Byte) type);

			currentBlock++;
		}
	}

	m_fileData.clear();
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
	m_isGPUDirty = false;
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

	m_debugMeshTop = mb.CreateMesh<Vertex3D_PCU>();

	//-----------------------------------------------------------------------------------------------
	// bottom
	mb.Clear();
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	mb.SetColor(Rgba::WHITE);

	mb.SetUV(uvs.maxs.x, uvs.mins.y);
	idx = mb.PushVertex(Vector3(bounds.maxs.x, bounds.mins.y, m_bounds.mins.z));

	//bl
	mb.SetUV(uvs.mins.x, uvs.mins.y);
	mb.PushVertex(Vector3(bounds.mins.x, bounds.mins.y, m_bounds.mins.z));

	// tl
	mb.SetUV(uvs.mins.x, uvs.maxs.y);
	mb.PushVertex(Vector3(bounds.mins.x, bounds.maxs.y, m_bounds.mins.z));

	// tr
	mb.SetUV(uvs.maxs.x, uvs.maxs.y);
	mb.PushVertex(Vector3(bounds.maxs.x, bounds.maxs.y, m_bounds.mins.z));

	mb.AddFace(idx + 0, idx + 1, idx + 2);
	mb.AddFace(idx + 2, idx + 3, idx + 0);

	m_debugMeshBottom = mb.CreateMesh<Vertex3D_PCU>();
}

//-----------------------------------------------------------------------------------------------
Vector3 Chunk::GetWorldPositionOfColumn(int theX, int theY)
{
	float howFarWeAreInX = (float)((m_chunkCoords.x * CHUNK_SIZE_X) + theX);
	float howFarWeAreInY = (float)((m_chunkCoords.y * CHUNK_SIZE_Y) + theY);

	return Vector3(howFarWeAreInX, howFarWeAreInY, 0.f);
}

//-----------------------------------------------------------------------------------------------
void Chunk::SetBlockType(int blockX, int blockY, int blockZ, const String & name)
{
	BlockIndex theIndex = GetBlockIndexForBlockCoords(BlockCoords(blockX, blockY, blockZ));
	BlockDefinition* theDef = BlockDefinition::GetDefinitionByName(name);
	
	SetBlockType(theIndex, theDef->m_type);
}

//-----------------------------------------------------------------------------------------------
void Chunk::SetBlockType(BlockIndex theIndex, Byte type)
{
	Block& theBlock = m_blocks[theIndex];
	theBlock.m_type = type;

	BlockDefinition* theDef = BlockDefinition::GetDefinitionByType(type);
	if (theDef->m_isFullyOpaque)
		theBlock.SetIsFullyOpaque();
	if (theDef->m_isVisible)
		theBlock.SetIsVisible();
	if (theDef->m_isSolid)
		theBlock.SetIsSolid();

	// we don't set the value, we let the lighting fix it so it'll spread
	if (theDef->m_lightLevel > 0)
		g_theGame->GetCurrentWorld()->MarkLightingDirty(BlockLocator(this, theIndex));
}

//-----------------------------------------------------------------------------------------------
void Chunk::AddVertsForBlock(BlockIndex theIndex)
{
	Block& theBlock = m_blocks[theIndex];

	if(theBlock.m_type == BLOCK_TYPE_AIR)
		return;

	BlockDefinition* theDefinition = BlockDefinition::GetDefinitionByType( (BlockTypes) theBlock.m_type);
	BlockCoords theCoords = GetBlockCoordsForBlockIndex(theIndex);
	Vector3 center = Vector3(m_chunkCoords.GetAsVector2() * (float) CHUNK_SIZE_X, 0.f) + theCoords.GetAsVector3() + Vector3(.5f);
	Vector3 dimensions = Vector3(.5f);

	AABB2 uvs;
	uint idx = 0;

	m_cpuMesh.SetColor(Rgba::WHITE);

	BlockLocator myBlock = BlockLocator(this, theIndex);
	BlockLocator northBlock = myBlock.GetBlockLocatorOfNorthNeighbor();
	BlockLocator southBlock = myBlock.GetBlockLocatorOfSouthNeighbor();
	BlockLocator eastBlock = myBlock.GetBlockLocatorOfEastNeighbor();
	BlockLocator westBlock = myBlock.GetBlockLocatorOfWestNeighbor();
	BlockLocator aboveBlock = myBlock.GetBlockLocatorOfAboveNeighbor();
	BlockLocator belowBlock = myBlock.GetBlockLocatorOfBelowNeighbor();


	//-----------------------------------------------------------------------------------------------
	// Top
	// br
	if (!aboveBlock.IsFullyOpaque())
	{
		uvs = theDefinition->m_topUVs;

		m_cpuMesh.SetUV(uvs.maxs.x, uvs.mins.y);
		idx = m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x), (center.y - dimensions.y), (center.z + dimensions.z)));

		//bl
		m_cpuMesh.SetUV(uvs.mins.x, uvs.mins.y);
		m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x), (center.y + dimensions.y), (center.z + dimensions.z)));

		// tl
		m_cpuMesh.SetUV(uvs.mins.x, uvs.maxs.y);
		m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x), (center.y + dimensions.y), (center.z + dimensions.z)));

		// tr
		m_cpuMesh.SetUV(uvs.maxs.x, uvs.maxs.y);
		m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x), (center.y - dimensions.y), (center.z + dimensions.z)));

		m_cpuMesh.AddFace(idx + 0, idx + 1, idx + 2);
		m_cpuMesh.AddFace(idx + 2, idx + 3, idx + 0);
	}


	//-----------------------------------------------------------------------------------------------
	// Front face
	if (!westBlock.IsFullyOpaque())
	{
		uvs = theDefinition->m_sideUVs;

		m_cpuMesh.SetUV(uvs.maxs.x, uvs.mins.y);
		idx = m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x), (center.y - dimensions.y), (center.z - dimensions.z)));

		//bl
		m_cpuMesh.SetUV(uvs.mins.x, uvs.mins.y);
		m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x), (center.y + dimensions.y), (center.z - dimensions.z)));

		// tl
		m_cpuMesh.SetUV(uvs.mins.x, uvs.maxs.y);
		m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x), (center.y + dimensions.y), (center.z + dimensions.z)));

		// tr
		m_cpuMesh.SetUV(uvs.maxs.x, uvs.maxs.y);
		m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x), (center.y - dimensions.y), (center.z + dimensions.z)));

		m_cpuMesh.AddFace(idx + 0, idx + 1, idx + 2);
		m_cpuMesh.AddFace(idx + 2, idx + 3, idx + 0);
	}
	

	//-----------------------------------------------------------------------------------------------
	// left
	if (!northBlock.IsFullyOpaque())
	{
		uvs = theDefinition->m_sideUVs;
		m_cpuMesh.SetUV(uvs.maxs.x, uvs.mins.y);
		idx = m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x), (center.y + dimensions.y), (center.z - dimensions.z)));

		//bl
		m_cpuMesh.SetUV(uvs.mins.x, uvs.mins.y);
		m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x), (center.y + dimensions.y), (center.z - dimensions.z)));

		// tl
		m_cpuMesh.SetUV(uvs.mins.x, uvs.maxs.y);
		m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x), (center.y + dimensions.y), (center.z + dimensions.z)));

		// tr
		m_cpuMesh.SetUV(uvs.maxs.x, uvs.maxs.y);
		m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x), (center.y + dimensions.y), (center.z + dimensions.z)));

		m_cpuMesh.AddFace(idx + 0, idx + 1, idx + 2);
		m_cpuMesh.AddFace(idx + 2, idx + 3, idx + 0);
	}


	//-----------------------------------------------------------------------------------------------
	// back
	if (!eastBlock.IsFullyOpaque())
	{
		uvs = theDefinition->m_sideUVs;
		m_cpuMesh.SetUV(uvs.maxs.x, uvs.mins.y);
		idx = m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x), (center.y + dimensions.y), (center.z - dimensions.z)));

		//bl
		m_cpuMesh.SetUV(uvs.mins.x, uvs.mins.y);
		m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x), (center.y - dimensions.y), (center.z - dimensions.z)));

		// tl
		m_cpuMesh.SetUV(uvs.mins.x, uvs.maxs.y);
		m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x), (center.y - dimensions.y), (center.z + dimensions.z)));

		// tr
		m_cpuMesh.SetUV(uvs.maxs.x, uvs.maxs.y);
		m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x), (center.y + dimensions.y), (center.z + dimensions.z)));

		m_cpuMesh.AddFace(idx + 0, idx + 1, idx + 2);
		m_cpuMesh.AddFace(idx + 2, idx + 3, idx + 0);
	}


	//-----------------------------------------------------------------------------------------------
	// right face
	if (!southBlock.IsFullyOpaque())
	{
		uvs = theDefinition->m_sideUVs;
		m_cpuMesh.SetUV(uvs.maxs.x, uvs.mins.y);
		idx = m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x), (center.y - dimensions.y), (center.z - dimensions.z)));

		//bl
		m_cpuMesh.SetUV(uvs.mins.x, uvs.mins.y);
		m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x), (center.y - dimensions.y), (center.z - dimensions.z)));

		// tl
		m_cpuMesh.SetUV(uvs.mins.x, uvs.maxs.y);
		m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x), (center.y - dimensions.y), (center.z + dimensions.z)));

		// tr
		m_cpuMesh.SetUV(uvs.maxs.x, uvs.maxs.y);
		m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x), (center.y - dimensions.y), (center.z + dimensions.z)));

		m_cpuMesh.AddFace(idx + 0, idx + 1, idx + 2);
		m_cpuMesh.AddFace(idx + 2, idx + 3, idx + 0);
	}


	//-----------------------------------------------------------------------------------------------
	// Bottom Face
	if (!belowBlock.IsFullyOpaque())
	{
		uvs = theDefinition->m_bottomUVs;
		m_cpuMesh.SetUV(uvs.maxs.x, uvs.mins.y);
		idx = m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x), (center.y - dimensions.y), (center.z - dimensions.z)));

		//bl
		m_cpuMesh.SetUV(uvs.mins.x, uvs.mins.y);
		m_cpuMesh.PushVertex(Vector3((center.x + dimensions.x), (center.y + dimensions.y), (center.z - dimensions.z)));

		// tl
		m_cpuMesh.SetUV(uvs.mins.x, uvs.maxs.y);
		m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x), (center.y + dimensions.y), (center.z - dimensions.z)));

		// tr
		m_cpuMesh.SetUV(uvs.maxs.x, uvs.maxs.y);
		m_cpuMesh.PushVertex(Vector3((center.x - dimensions.x), (center.y - dimensions.y), (center.z - dimensions.z)));

		m_cpuMesh.AddFace(idx + 0, idx + 1, idx + 2);
		m_cpuMesh.AddFace(idx + 2, idx + 3, idx + 0);
	}

}

//-----------------------------------------------------------------------------------------------
bool Chunk::CanRebuildItsMesh()
{
	return (
		m_northNeighbor != nullptr 
		&& m_southNeighbor != nullptr
		&& m_eastNeighbor != nullptr 
		&& m_westNeighbor != nullptr);
}

//-----------------------------------------------------------------------------------------------
void Chunk::Dirty()
{
	m_isGPUDirty = true;
	m_hasBeenModified = true;
}

//-----------------------------------------------------------------------------------------------
void Chunk::LeaveYourNeighbors()
{
	// set my neighbors pointers of me to null
	if (m_eastNeighbor) {  m_eastNeighbor->m_westNeighbor = nullptr; }
	if (m_westNeighbor) {  m_westNeighbor->m_eastNeighbor = nullptr; }
	if (m_southNeighbor) { m_southNeighbor->m_northNeighbor = nullptr; }
	if (m_northNeighbor) { m_northNeighbor->m_southNeighbor = nullptr; }

	// make them dirty
	if (m_eastNeighbor) {  m_eastNeighbor->m_isGPUDirty = true;	 }
	if (m_westNeighbor) {  m_westNeighbor->m_isGPUDirty = true;	 }
	if (m_southNeighbor) { m_southNeighbor->m_isGPUDirty = true; }
	if (m_northNeighbor) { m_northNeighbor->m_isGPUDirty = true; }
}

//-----------------------------------------------------------------------------------------------
BlockIndex Chunk::GetBlockIndexForWorldCoords(const Vector3& worldPos)
{
	IntVector3 thePos = worldPos.GetAsIntVector3();
	int x = thePos.x - (m_chunkCoords.x * CHUNK_SIZE_X);
	int y = thePos.y - (m_chunkCoords.y * CHUNK_SIZE_Y);

	return GetBlockIndexForBlockCoords(BlockCoords(x, y, thePos.z));
}

//-----------------------------------------------------------------------------------------------
STATIC BlockIndex Chunk::GetBlockIndexForBlockCoords(const BlockCoords & bc)
{
	// Indexing an array in 3D:  x + (y * size.x) + (z * (size.x * size.y))
	return bc.x + (bc.y * CHUNK_SIZE_X) + (bc.z * (CHUNK_SIZE_X * CHUNK_SIZE_Y));
}

//-----------------------------------------------------------------------------------------------
STATIC BlockCoords Chunk::GetBlockCoordsForBlockIndex(BlockIndex bi)
{
	int z = bi >> (CHUNK_BITS_WIDE_X + CHUNK_BITS_WIDE_Y);
	int y = (bi & CHUNK_Y_MASK) >> CHUNK_BITS_WIDE_X;
	int x = bi & CHUNK_X_MASK;
	
	return BlockCoords(x,y,z);
}

//-----------------------------------------------------------------------------------------------
STATIC ChunkCoords Chunk::GetChunkCoordsFromWorldPosition(const Vector3& worldPos)
{
	int x = (int)(floorf(worldPos.x / (float)CHUNK_SIZE_X));
	int y = (int)(floorf(worldPos.y / (float)CHUNK_SIZE_Y));

	return ChunkCoords(x, y);
}


//===============================================================================================
bool ChunkHeader::IsValid()
{
	ChunkHeader correctVersion;

	if (m_4cc[0] != correctVersion.m_4cc[0])
	{
		DevConsole::AddConsoleDialogue("Error with m_4cc[0]");
		return false;
	}

	if (m_4cc[2] != correctVersion.m_4cc[2])
	{
		DevConsole::AddConsoleDialogue("Error with m_4cc[1]");
		return false;
	}

	if (m_4cc[3] != correctVersion.m_4cc[3])
	{
		DevConsole::AddConsoleDialogue("Error with m_4cc[2]");
		return false;
	}

	if (m_version != correctVersion.m_version)
	{
		DevConsole::AddConsoleDialogue("Error with m_4cc[3]");
		return false;
	}

	if (m_chunkBitsX != correctVersion.m_chunkBitsX)
	{
		DevConsole::AddConsoleDialogue("Error with chunkbitsX");
		return false;
	}

	if (m_chunkBitsY != correctVersion.m_chunkBitsY)
	{
		DevConsole::AddConsoleDialogue("Error with chunkBitsY");
		return false;
	}

	if (m_chunkBitsZ != correctVersion.m_chunkBitsZ)
	{
		DevConsole::AddConsoleDialogue("Error with chunkBitsZ");
		return false;
	}

	if (m_reserved1 != correctVersion.m_reserved1)
	{
		DevConsole::AddConsoleDialogue("Error with reserved slot 1");
		return false;
	}

	if (m_reserver2 != correctVersion.m_reserver2)
	{
		DevConsole::AddConsoleDialogue("Error with reserved slot 2");
		return false;
	}

	if (m_reserved3 != correctVersion.m_reserved3)
	{
		DevConsole::AddConsoleDialogue("Error with reserved slot 3");
		return false;
	}

	if (m_format != correctVersion.m_format)
	{
		DevConsole::AddConsoleDialogue("Error with format");
		return false;
	}

	return true;
}
