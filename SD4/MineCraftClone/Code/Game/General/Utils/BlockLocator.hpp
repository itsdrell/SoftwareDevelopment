#pragma once
#include "Game/Main/GameCommon.hpp"
#include "Game/General/World/BlockDefinition.hpp"
#include "Game/General/World/Block.hpp"

//====================================================================================
// Forward Declare
//====================================================================================
class Chunk;

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
	BlockLocator(Chunk* theChunk, const BlockCoords& theCoords);

public:
	Block& GetBlock();
	inline bool IsValid() { return m_chunk != nullptr;  }
	static Block& GetInvalidBlock();

	inline bool IsFullyOpaque();
	BlockDefinition* GetBlockDefinition();

public:
	BlockLocator GetBlockLocatorOfNorthNeighbor();
	BlockLocator GetBlockLocatorOfSouthNeighbor();
	BlockLocator GetBlockLocatorOfEastNeighbor();
	BlockLocator GetBlockLocatorOfWestNeighbor();
	BlockLocator GetBlockLocatorOfAboveNeighbor();
	BlockLocator GetBlockLocatorOfBelowNeighbor();

	BlockLocator GetBlockLocatorNextToMeFromNormal(const Vector3& theNormal);

	Vector3 GetCenterOfBlock() const;

public:
	inline int GetIndoorLightValueFromBlock();
	inline int GetOutdoorLightValueFromBlock();
	int GetHighestIndoorLightValueFromNeighbors();
	int GetHighestOutdoorLightValueFromNeighbors();
	Rgba GetTintForBlock();


public:
	bool IsBlockOnEastEdge() const;
	bool IsBlockOnWestEdge() const;
	bool IsBlockOnNorthEdge() const;
	bool IsBlockOnSouthEdge() const;
	bool IsBlockOnTopEdge() const;
	bool IsBlockOnBottomEdge() const;
	// NSWE check
	bool IsBlockOnAChunkEdge() const;

public:
	inline bool IsSky() { return GetBlock().IsSky();  }

public:
	void MoveNorth();
	void MoveSouth();
	void MoveEast();
	void MoveWest();
	void MoveAbove();
	void MoveBelow();

public:
	Chunk*			m_chunk = nullptr;
	BlockIndex		m_indexOfBlock;
};

//====================================================================================
// Inline
//====================================================================================
inline bool BlockLocator::IsFullyOpaque()
{
	return GetBlockDefinition()->m_isFullyOpaque;
}

//-----------------------------------------------------------------------------------------------
inline int BlockLocator::GetIndoorLightValueFromBlock()
{
	return GetBlock().GetIndoorLightLevel();
}

//-----------------------------------------------------------------------------------------------
inline int BlockLocator::GetOutdoorLightValueFromBlock()
{
	return GetBlock().GetOutdoorLightLevel();
}

//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [2/18/2019]
//====================================================================================