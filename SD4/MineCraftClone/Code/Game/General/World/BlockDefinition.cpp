#include "BlockDefinition.hpp"
#include "Game/Main/GameCommon.hpp"

//===============================================================================================
std::map<String, BlockDefinition*> BlockDefinition::s_definitions;

//===============================================================================================
BlockDefinition::BlockDefinition(const String& name, const IntVector2& spriteCoordsForTop, const IntVector2& spriteCoordsForSides, const IntVector2& spriteCoordsForBottom)
{
	m_topUVs = g_blockSpriteSheet.GetTexCoordsForSpriteCoords(spriteCoordsForTop);
	m_bottomUVs = g_blockSpriteSheet.GetTexCoordsForSpriteCoords(spriteCoordsForBottom);
	m_sideUVs = g_blockSpriteSheet.GetTexCoordsForSpriteCoords(spriteCoordsForSides);

	s_definitions.insert(std::pair<String , BlockDefinition*>(name, this));
}

//-----------------------------------------------------------------------------------------------
BlockDefinition* BlockDefinition::GetDefinitionByName(const String& nameOfDefinition)
{
	std::map<std::string,BlockDefinition*>::iterator theIterator;
	theIterator = BlockDefinition::s_definitions.find(nameOfDefinition);
	if(theIterator != BlockDefinition::s_definitions.end()){ return theIterator->second; }

	return nullptr;
}
