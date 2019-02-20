#pragma once
#include "Game/Main/GameCommon.hpp"

//====================================================================================
// Forward Declare
//====================================================================================
class Chunk;
class Block;
class BlockDefinition;

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
	bool IsValid() { return m_chunk != nullptr;  }

	bool IsFullyOpaque();
	BlockDefinition* GetBlockDefinition();

public:
	BlockLocator GetBlockLocatorOfNorthNeighbor();
	BlockLocator GetBlockLocatorOfSouthNeighbor();
	BlockLocator GetBlockLocatorOfEastNeighbor();
	BlockLocator GetBlockLocatorOfWestNeighbor();
	BlockLocator GetBlockLocatorOfAboveNeighbor();
	BlockLocator GetBlockLocatorOfBelowNeighbor();

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
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [2/18/2019]
//====================================================================================