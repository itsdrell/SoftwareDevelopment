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
Block & BlockLocator::GetBlock()
{
	if (m_chunk != nullptr)
		return m_chunk->m_blocks[m_indexOfBlock];

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
void BlockLocator::MoveUp()
{
}

//-----------------------------------------------------------------------------------------------
void BlockLocator::MoveDown()
{
}
