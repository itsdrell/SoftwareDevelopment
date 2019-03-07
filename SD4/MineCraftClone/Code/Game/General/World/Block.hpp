#pragma once
#include "Engine/Core/General/EngineCommon.hpp"

//====================================================================================
// Forward Declare
//====================================================================================


//====================================================================================
// Type Defs + Defines
//====================================================================================
constexpr Byte BLOCK_BIT_IS_SKY =			0b1000'0000;  // I am non - opaque & no opaque blocks directly above me
constexpr Byte BLOCK_BIT_IS_LIGHT_DIRTY =	0b0100'0000;  // A BlockLocator for me is currently in the dirty light queue
constexpr Byte BLOCK_BIT_IS_FULL_OPAQUE =	0b0010'0000;  // I block light, visibility, and hide my neighbors’ faces
constexpr Byte BLOCK_BIT_IS_VISIBLE =		0b0001'0000;  // Physical objects and raycasts collide with me
constexpr Byte BLOCK_BIT_IS_SOLID =			0b0000'1000;  // I cannot be skipped during chunk mesh rebuilding


constexpr Byte BLOCK_LIGHT_MASK_INDOOR =	0b0000'1111;
constexpr Byte BLOCK_LIGHT_MASK_OUTDOOR =	0b1111'0000;

//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class Block
{
public:
	Block() {}
	Block(unsigned char type) {
		m_type = type; m_light = 0; m_bitFlags = 0;
	}
	~Block() {}

public:
	inline void SetIndoorLightLevel(int lightLevel);
	inline void SetOutdoorLightLevel(int lightLevel);
	inline void SetToSky();
	inline void SetIsLightDirty();
	inline void SetIsFullyOpaque();
	inline void SetIsSolid();
	inline void SetIsVisible();

public:
	inline void ClearFlagBits();
	inline void ClearIsSky();
	inline void ClearIsLightDirty();

public:
	inline int GetIndoorLightLevel();
	inline int GetOutdoorLightLevel();

public:
	inline bool IsSky() const;
	inline bool IsLightDirty();
	inline bool IsSolid();
	inline bool IsVisible();
	inline bool IsFullyOpaque();

public:
	Byte	m_type = 0; // default to air

private:
	// indoor is the low bits (0-15 ranges), outdoor is high (0-15 ranges)
	Byte	m_light = 0; 
	Byte	m_bitFlags = 0;
};

//====================================================================================
// Standalone C Functions
//====================================================================================

//====================================================================================
// Inlines
//====================================================================================
void Block::SetIndoorLightLevel(int lightLevel)
{
	Byte value = (Byte)m_light & ~BLOCK_LIGHT_MASK_INDOOR; // clears indoor bits
	m_light = value | ((Byte)lightLevel); // set indoor bits
}

//-----------------------------------------------------------------------------------------------
void Block::SetOutdoorLightLevel(int lightLevel)
{
	Byte value = (Byte)m_light & ~BLOCK_LIGHT_MASK_OUTDOOR;
	m_light = value | (((Byte)lightLevel) << 4);
}

//-----------------------------------------------------------------------------------------------
void Block::SetToSky()
{
	m_bitFlags |= BLOCK_BIT_IS_SKY;
}

//-----------------------------------------------------------------------------------------------
void Block::SetIsLightDirty()
{
	m_bitFlags |= BLOCK_BIT_IS_LIGHT_DIRTY;
}

//-----------------------------------------------------------------------------------------------
inline void Block::SetIsFullyOpaque()
{
	m_bitFlags |= BLOCK_BIT_IS_FULL_OPAQUE;
}

//-----------------------------------------------------------------------------------------------
inline void Block::SetIsSolid()
{
	m_bitFlags |= BLOCK_BIT_IS_SOLID;
}

//-----------------------------------------------------------------------------------------------
inline void Block::SetIsVisible()
{
	m_bitFlags |= BLOCK_BIT_IS_VISIBLE;
}

//-----------------------------------------------------------------------------------------------
void Block::ClearFlagBits()
{
	m_bitFlags = 0;
}

//-----------------------------------------------------------------------------------------------
inline void Block::ClearIsSky()
{
	m_bitFlags &= (~BLOCK_BIT_IS_SKY);
}

//-----------------------------------------------------------------------------------------------
inline void Block::ClearIsLightDirty()
{
	m_bitFlags &= (~BLOCK_BIT_IS_LIGHT_DIRTY);
}

//-----------------------------------------------------------------------------------------------
int Block::GetIndoorLightLevel()
{
	return (int)(m_light & BLOCK_LIGHT_MASK_INDOOR);
}

//-----------------------------------------------------------------------------------------------
int Block::GetOutdoorLightLevel()
{
	return (int)((m_light & BLOCK_LIGHT_MASK_OUTDOOR) >> 4);
}

//-----------------------------------------------------------------------------------------------
bool Block::IsSky() const
{
	return (m_bitFlags & BLOCK_BIT_IS_SKY) == BLOCK_BIT_IS_SKY;
}

//-----------------------------------------------------------------------------------------------
inline bool Block::IsLightDirty()
{
	return (m_bitFlags & BLOCK_BIT_IS_LIGHT_DIRTY) == BLOCK_BIT_IS_LIGHT_DIRTY;
}

//-----------------------------------------------------------------------------------------------
inline bool Block::IsSolid()
{
	return (m_bitFlags & BLOCK_BIT_IS_SOLID) == BLOCK_BIT_IS_SOLID;
}

//-----------------------------------------------------------------------------------------------
inline bool Block::IsVisible() 
{
	return (m_bitFlags & BLOCK_BIT_IS_VISIBLE) == BLOCK_BIT_IS_VISIBLE;
}

//-----------------------------------------------------------------------------------------------
bool Block::IsFullyOpaque()
{
	return(m_bitFlags & BLOCK_BIT_IS_FULL_OPAQUE) == BLOCK_BIT_IS_FULL_OPAQUE;
}

//====================================================================================
// Written by Zachary Bracken : [2/6/2019]
//====================================================================================