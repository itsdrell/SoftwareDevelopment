#include "BlockLocator.hpp"
#include "Game/General/World/Block.hpp"
#include "Game/General/World/Chunk.hpp"
#include "Game/General/World/BlockDefinition.hpp"

//===============================================================================================
BlockLocator::BlockLocator(Chunk* theChunk, BlockIndex indexOfBlock)
{
	m_chunk = theChunk;
	m_indexOfBlock = indexOfBlock;
}

//-----------------------------------------------------------------------------------------------
BlockLocator::BlockLocator(Chunk* theChunk, const BlockCoords& theCoords)
{
	m_chunk = theChunk;
	m_indexOfBlock = Chunk::GetBlockIndexForBlockCoords(theCoords);
}

//-----------------------------------------------------------------------------------------------
Block & BlockLocator::GetBlock()
{
	if (m_chunk != nullptr)
		return m_chunk->m_blocks[m_indexOfBlock];

	return g_invalidBlock;
}

//-----------------------------------------------------------------------------------------------
Block& BlockLocator::GetInvalidBlock()
{
	g_invalidBlock = Block(BLOCK_TYPE_TEST);
	return g_invalidBlock;
}

//-----------------------------------------------------------------------------------------------
BlockDefinition* BlockLocator::GetBlockDefinition()
{
	
	if (IsValid())
	{
		Block theBlock = GetBlock();
		return BlockDefinition::GetDefinitionByType((BlockTypes) theBlock.m_type);
	}
	else
	{
		return BlockDefinition::GetDefinitionByName("test");
	}

}

//-----------------------------------------------------------------------------------------------
BlockLocator BlockLocator::GetBlockLocatorOfNorthNeighbor()
{
	if (m_chunk)
	{
		if ((m_indexOfBlock & CHUNK_Y_MASK) == CHUNK_Y_MASK)
		{
			BlockIndex newIndex = m_indexOfBlock & ~CHUNK_Y_MASK;
			return BlockLocator(m_chunk->m_northNeighbor, newIndex);
		}
		else
		{
			BlockIndex newIndex = m_indexOfBlock + (CHUNK_SIZE_X);
			return BlockLocator(m_chunk, newIndex);
		}
	}

	return BlockLocator(nullptr, -1);
}

//-----------------------------------------------------------------------------------------------
BlockLocator BlockLocator::GetBlockLocatorOfSouthNeighbor()
{
	if (m_chunk)
	{
		if ((m_indexOfBlock & CHUNK_Y_MASK) == 0)
		{
			BlockIndex newIndex = m_indexOfBlock | CHUNK_Y_MASK;
			return BlockLocator(m_chunk->m_southNeighbor, newIndex);
		}
		else
		{
			BlockIndex newIndex = m_indexOfBlock - (CHUNK_SIZE_X);
			return BlockLocator(m_chunk, newIndex);
		}
	}

	return BlockLocator(nullptr, -1);
}

//-----------------------------------------------------------------------------------------------
BlockLocator BlockLocator::GetBlockLocatorOfEastNeighbor()
{
	if (m_chunk)
	{
		if ((m_indexOfBlock & CHUNK_X_MASK) == CHUNK_X_MASK)
		{
			BlockIndex newIndex = m_indexOfBlock & ~CHUNK_X_MASK;
			return BlockLocator(m_chunk->m_eastNeighbor, newIndex);
		}
		else
		{
			BlockIndex newIndex = m_indexOfBlock + 1;
			return BlockLocator(m_chunk, newIndex);
		}
	}

	return BlockLocator(nullptr, -1);
}

//-----------------------------------------------------------------------------------------------
BlockLocator BlockLocator::GetBlockLocatorOfWestNeighbor()
{
	if (m_chunk)
	{
		if ((m_indexOfBlock & CHUNK_X_MASK) == 0)
		{
			BlockIndex newIndex = m_indexOfBlock | CHUNK_X_MASK; // could also just add 15, but this is faster
			return BlockLocator(m_chunk->m_westNeighbor, newIndex);
		}
		else
		{
			BlockIndex newIndex = m_indexOfBlock - 1;
			return BlockLocator(m_chunk, newIndex);
		}
	}

	return BlockLocator(nullptr, -1);
}

//-----------------------------------------------------------------------------------------------
BlockLocator BlockLocator::GetBlockLocatorOfAboveNeighbor()
{
	if (m_chunk)
	{
		if ((m_indexOfBlock & CHUNK_Z_MASK) == (CHUNK_HEIGHT - 1))
		{
			return BlockLocator(nullptr, -1);
		}
		else
		{
			BlockIndex newIndex = m_indexOfBlock + (CHUNK_SIZE_X * CHUNK_SIZE_Y);
			return BlockLocator(m_chunk, newIndex);
		}
	}

	return BlockLocator(nullptr, -1);
}

//-----------------------------------------------------------------------------------------------
BlockLocator BlockLocator::GetBlockLocatorOfBelowNeighbor()
{
	if (m_chunk)
	{
		if ((m_indexOfBlock & CHUNK_Z_MASK) == 0)
		{
			return BlockLocator(nullptr, -1);
		}
		else
		{
			BlockIndex newIndex = m_indexOfBlock - (CHUNK_SIZE_X * CHUNK_SIZE_Y);
			return BlockLocator(m_chunk, newIndex);
		}
	}

	return BlockLocator(nullptr, -1);
}

//-----------------------------------------------------------------------------------------------
BlockLocator BlockLocator::GetBlockLocatorNextToMeFromNormal(const Vector3& theNormal)
{
	if (theNormal.x == 1) { return	GetBlockLocatorOfEastNeighbor(); }
	if (theNormal.x == -1) { return GetBlockLocatorOfWestNeighbor(); }
	if (theNormal.y == 1) { return GetBlockLocatorOfNorthNeighbor(); }
	if (theNormal.y == -1) { return GetBlockLocatorOfSouthNeighbor(); }
	if (theNormal.z == 1) { return GetBlockLocatorOfAboveNeighbor(); }
	if (theNormal.z == -1) { return GetBlockLocatorOfBelowNeighbor(); }

	return BlockLocator(nullptr, -1);
}

//-----------------------------------------------------------------------------------------------
Vector3 BlockLocator::GetCenterOfBlock() const
{
	if (m_chunk == nullptr)
		return Vector3();
	
	BlockCoords myCords = Chunk::GetBlockCoordsForBlockIndex(m_indexOfBlock);

	float newX = (float)(myCords.x + (m_chunk->m_chunkCoords.x * CHUNK_SIZE_X));
	float newY = (float)(myCords.y + (m_chunk->m_chunkCoords.y * CHUNK_SIZE_Y));

	Vector3 worldPos = Vector3(newX, newY, (float)myCords.z);

	Vector3 center = worldPos + Vector3(.5f);

	return center;
}

//-----------------------------------------------------------------------------------------------
int BlockLocator::GetHighestIndoorLightValueFromNeighbors()
{
	int maxValue = 0;
	
	int eastNeighborLightValue =	GetBlockLocatorOfEastNeighbor().GetBlock().GetIndoorLightLevel();
	int westNeighborLightValue =	GetBlockLocatorOfNorthNeighbor().GetBlock().GetIndoorLightLevel();
	int northNeightborLightValue =	GetBlockLocatorOfNorthNeighbor().GetBlock().GetIndoorLightLevel();
	int southNeightborLightValue =	GetBlockLocatorOfSouthNeighbor().GetBlock().GetIndoorLightLevel();
	int aboveNeightborLightValue =	GetBlockLocatorOfAboveNeighbor().GetBlock().GetIndoorLightLevel();
	int bottomNeightborLightValue = GetBlockLocatorOfBelowNeighbor().GetBlock().GetIndoorLightLevel();

	if (eastNeighborLightValue > maxValue)
		maxValue = eastNeighborLightValue;
	if (westNeighborLightValue > maxValue)
		maxValue = westNeighborLightValue;
	if (northNeightborLightValue > maxValue)
		maxValue = northNeightborLightValue;
	if (southNeightborLightValue > maxValue)
		maxValue = southNeightborLightValue;
	if (aboveNeightborLightValue > maxValue)
		maxValue = aboveNeightborLightValue;
	if (bottomNeightborLightValue > maxValue)
		maxValue = bottomNeightborLightValue;

	return maxValue;
}

//-----------------------------------------------------------------------------------------------
int BlockLocator::GetHighestOutdoorLightValueFromNeighbors()
{
	int maxValue = 0;

	int eastNeighborLightValue = GetBlockLocatorOfEastNeighbor().GetBlock().GetOutdoorLightLevel();
	int westNeighborLightValue = GetBlockLocatorOfNorthNeighbor().GetBlock().GetOutdoorLightLevel();
	int northNeightborLightValue = GetBlockLocatorOfNorthNeighbor().GetBlock().GetOutdoorLightLevel();
	int southNeightborLightValue = GetBlockLocatorOfSouthNeighbor().GetBlock().GetOutdoorLightLevel();
	int aboveNeightborLightValue = GetBlockLocatorOfAboveNeighbor().GetBlock().GetOutdoorLightLevel();
	int bottomNeightborLightValue = GetBlockLocatorOfBelowNeighbor().GetBlock().GetOutdoorLightLevel();

	if (eastNeighborLightValue > maxValue)
		maxValue = eastNeighborLightValue;
	if (westNeighborLightValue > maxValue)
		maxValue = westNeighborLightValue;
	if (northNeightborLightValue > maxValue)
		maxValue = northNeightborLightValue;
	if (southNeightborLightValue > maxValue)
		maxValue = southNeightborLightValue;
	if (aboveNeightborLightValue > maxValue)
		maxValue = aboveNeightborLightValue;
	if (bottomNeightborLightValue > maxValue)
		maxValue = bottomNeightborLightValue;

	return maxValue;
}

//-----------------------------------------------------------------------------------------------
Rgba BlockLocator::GetTintForBlock()
{
	Rgba theColor;

	//( (1.f/16.f) * (float)( 1 + eastNeighbor.indoorExposure ) 
	float indoorValue = (1.f / 16.f) * (float)(1 + GetIndoorLightValueFromBlock()); 

	//( (1.f/15.f) * (float)( 0 + eastNeighbor.outdoorExposure ) )
	float outdoorValue = (1.f / 15.f) * (float)(0 + GetOutdoorLightValueFromBlock()); 

	//theColor.SetFromFloats(indoorValue, outdoorValue, 127, 255);
	theColor.SetFromNormalizedFloats(indoorValue, outdoorValue, .5f, 1.f);

	return theColor;
}

//-----------------------------------------------------------------------------------------------
bool BlockLocator::IsBlockOnEastEdge() const
{
	return ((m_indexOfBlock & CHUNK_X_MASK) == CHUNK_X_MASK);
}

//-----------------------------------------------------------------------------------------------
bool BlockLocator::IsBlockOnWestEdge() const
{
	return ((m_indexOfBlock & CHUNK_X_MASK) == 0);
}

//-----------------------------------------------------------------------------------------------
bool BlockLocator::IsBlockOnNorthEdge() const
{
	return ((m_indexOfBlock & CHUNK_Y_MASK) == CHUNK_Y_MASK);
}

//-----------------------------------------------------------------------------------------------
bool BlockLocator::IsBlockOnSouthEdge() const
{
	return ((m_indexOfBlock & CHUNK_Y_MASK) == 0);
}

//-----------------------------------------------------------------------------------------------
bool BlockLocator::IsBlockOnTopEdge() const
{
	return ((m_indexOfBlock & CHUNK_Z_MASK) == (CHUNK_HEIGHT - 1));
}

//-----------------------------------------------------------------------------------------------
bool BlockLocator::IsBlockOnBottomEdge() const
{
	return ((m_indexOfBlock & CHUNK_Z_MASK) == 0);
}

bool BlockLocator::IsBlockOnAChunkEdge() const
{
	if (IsBlockOnEastEdge() || IsBlockOnNorthEdge() || IsBlockOnSouthEdge() || IsBlockOnWestEdge())
		return true;

	return false;
}

//-----------------------------------------------------------------------------------------------
void BlockLocator::MoveNorth()
{
}

//-----------------------------------------------------------------------------------------------
void BlockLocator::MoveSouth()
{
}

//-----------------------------------------------------------------------------------------------
void BlockLocator::MoveEast()
{
}

//-----------------------------------------------------------------------------------------------
void BlockLocator::MoveWest()
{
}

//-----------------------------------------------------------------------------------------------
void BlockLocator::MoveBelow()
{
	BlockLocator below = GetBlockLocatorOfBelowNeighbor();
	m_indexOfBlock = below.m_indexOfBlock;
	m_chunk = below.m_chunk;
}

