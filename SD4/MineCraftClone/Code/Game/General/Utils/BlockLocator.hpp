#pragma once
#include "Game/Main/GameCommon.hpp"
#include "Game/General/World/BlockDefinition.hpp"

//====================================================================================
// Forward Declare
//====================================================================================
class Chunk;
class Block;

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
class BlockLocator
{
public:
	BlockLocator( Chunk* theChunk, BlockIndex indexOfBlock ); 

public:
	Block& GetBlock();
	inline bool IsValid() { return m_chunk != nullptr;  }

	inline bool IsFullyOpaque();
	BlockDefinition* GetBlockDefinition();

public:
	BlockLocator GetBlockLocatorOfNorthNeighbor();
	BlockLocator GetBlockLocatorOfSouthNeighbor();
	BlockLocator GetBlockLocatorOfEastNeighbor();
	BlockLocator GetBlockLocatorOfWestNeighbor();
	BlockLocator GetBlockLocatorOfAboveNeighbor();
	BlockLocator GetBlockLocatorOfBelowNeighbor();

	Vector3 GetCenterOfBlock() const;

public:
	void MoveNorth();
	void MoveSouth();
	void MoveEast();
	void MoveWest();
	void MoveUp();
	void MoveDown();

public:
	Chunk*			m_chunk;
	BlockIndex		m_indexOfBlock;
};

//====================================================================================
// Inline
//====================================================================================
inline bool BlockLocator::IsFullyOpaque()
{
	return GetBlockDefinition()->m_isFullyOpaque;
}

//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [2/18/2019]
//====================================================================================