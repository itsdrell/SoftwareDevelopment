#include "BlockDefinition.hpp"
#include "Game/Main/GameCommon.hpp"

//===============================================================================================
std::map<String, BlockDefinition*> BlockDefinition::s_definitions;

//===============================================================================================
BlockDefinition::BlockDefinition(const String& name, BlockTypes theType, bool isOpqaue, const IntVector2& spriteCoordsForTop, const IntVector2& spriteCoordsForSides, const IntVector2& spriteCoordsForBottom)
{
	m_topUVs = g_blockSpriteSheet.GetTexCoordsForSpriteCoords(spriteCoordsForTop);
	m_bottomUVs = g_blockSpriteSheet.GetTexCoordsForSpriteCoords(spriteCoordsForBottom);
	m_sideUVs = g_blockSpriteSheet.GetTexCoordsForSpriteCoords(spriteCoordsForSides);
	m_type = theType;
	m_name = name;
	m_isFullyOpaque = isOpqaue;

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

//-----------------------------------------------------------------------------------------------
BlockDefinition* BlockDefinition::GetDefinitionByType(BlockTypes theType)
{
	std::map<std::string,BlockDefinition*>::iterator theIterator;

	for(theIterator = s_definitions.begin(); theIterator != s_definitions.end(); theIterator++)
	{
		if(theIterator->second->m_type == theType)
			return theIterator->second;
	}

	//ERROR_AND_DIE("Could not find a definiton for the type");
	return nullptr;
}
