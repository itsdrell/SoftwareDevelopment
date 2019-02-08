#pragma once
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/Images/Sprites/SpriteSheet.hpp"

//====================================================================================
// Forward Declare
//====================================================================================


//====================================================================================
// Type Defs + Defines
//====================================================================================
typedef IntVector2 ChunkCoords;
typedef IntVector3 BlockCoords; // 0 to 255 on xy and 0 - 255
typedef int BlockIndex; // 0 to 65535

constexpr int CHUNK_BITS_WIDE_X = 4; //(CHUNC IS 2_THIS BLOCKS WIDE ON X
constexpr int CHUNK_BITS_WIDE_Y = 4;
constexpr int CHUNK_BITS_TALL_Z = 8;

constexpr int CHUNK_SIZE_X = 16;
constexpr int CHUNK_SIZE_Y = 16;
constexpr int CHUNK_HEIGHT = 256;
constexpr int BLOCKS_PER_LAYER = CHUNK_SIZE_X * CHUNK_SIZE_Y;
constexpr int BLOCKS_WIDE_X = (1 << CHUNK_BITS_WIDE_X); //eg if x-bits us 4

// 0b0111'1111
constexpr int CHUNK_X_MASK = 0b0000'1111;
constexpr int CHUNK_Y_MASK = 0b1111'0000;
constexpr int CHUNK_Z_MASK = 0b1111'1111'0000'0000;
//X mask = 0000001111
//Y mask = 000001110000
//Z mask = 111111110000000

//====================================================================================
// ENUMS
//====================================================================================



//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================


//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================
extern Renderer*		g_theRenderer;
extern InputSystem*		g_theInput;
extern AudioSystem*		g_audio; // not the audio cause we could have multiple...?


extern SpriteSheet		g_blockSpriteSheet;

//====================================================================================
// Written by Zachary Bracken : [6/19/2018]
//====================================================================================


