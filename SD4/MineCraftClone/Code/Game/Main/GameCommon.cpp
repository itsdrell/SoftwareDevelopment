#include "Game/Main/GameCommon.hpp"
#include "Game/General/World/Block.hpp"
#include "Game/General/World/BlockDefinition.hpp"

#pragma warning(disable : 4239)

Renderer* g_theRenderer = nullptr;
InputSystem* g_theInput = nullptr;
AudioSystem* g_audio = nullptr;


SpriteSheet		g_blockSpriteSheet;

Block& g_invalidBlock = Block(BLOCK_TYPE_TEST);
