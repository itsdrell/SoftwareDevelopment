#include "BlockDefinition.hpp"
#include "Game/Main/GameCommon.hpp"
#include "Engine/Core/Utils/XmlUtilities.hpp"

//===============================================================================================
std::map<String, BlockDefinition*> BlockDefinition::s_definitions;

//===============================================================================================
BlockDefinition::BlockDefinition(const tinyxml2::XMLElement & definitionNode)
{
	m_name = ParseXmlAttribute(definitionNode,"name","Error");
	m_type = (unsigned char) ParseXmlAttribute(definitionNode,"bitType", 99);
	m_isFullyOpaque = ParseXmlAttribute(definitionNode,"isOpaque", false);

	IntVector2 spriteIndex = ParseXmlAttribute(definitionNode,"topUVs", IntVector2());
	m_topUVs = g_blockSpriteSheet.GetTexCoordsForSpriteCoords(spriteIndex);
	
	spriteIndex = ParseXmlAttribute(definitionNode,"sideUVs", IntVector2());
	m_sideUVs = g_blockSpriteSheet.GetTexCoordsForSpriteCoords(spriteIndex);

	spriteIndex = ParseXmlAttribute(definitionNode,"bottomUVs", IntVector2());
	m_bottomUVs = g_blockSpriteSheet.GetTexCoordsForSpriteCoords(spriteIndex);

	s_definitions.insert(std::pair<String , BlockDefinition*>(m_name, this));
}

//-----------------------------------------------------------------------------------------------
BlockDefinition::BlockDefinition(const String& name, BlockTypes theType, bool isOpqaue, const IntVector2& spriteCoordsForTop, const IntVector2& spriteCoordsForSides, const IntVector2& spriteCoordsForBottom)
{
	m_topUVs = g_blockSpriteSheet.GetTexCoordsForSpriteCoords(spriteCoordsForTop);
	m_bottomUVs = g_blockSpriteSheet.GetTexCoordsForSpriteCoords(spriteCoordsForBottom);
	m_sideUVs = g_blockSpriteSheet.GetTexCoordsForSpriteCoords(spriteCoordsForSides);
	m_type = (unsigned char) theType;
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
	return GetDefinitionByName("test");
}

//-----------------------------------------------------------------------------------------------
void BlockDefinition::DeleteDefinitions()
{
	std::map<String,BlockDefinition*>::iterator theIterator;
	for(theIterator = s_definitions.begin(); theIterator != s_definitions.end(); theIterator++)
	{
		BlockDefinition* current = theIterator->second;
		
		delete current;
		current = nullptr;
	}

	s_definitions.clear();
}
