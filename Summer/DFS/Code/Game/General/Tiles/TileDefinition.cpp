#include "TileDefinition.hpp"
#include "Engine\Core\Utils\XmlUtilities.hpp"
#include "Engine\Core\General\EngineCommon.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteSheet.hpp"

//=========================================================
std::map<std::string, TileDefinition*> TileDefinition::s_definitions;
//=========================================================

TileDefinition* GetTileDefinition(std::string name)
{
	std::map<std::string,TileDefinition*>::iterator tileIterator;
	tileIterator = TileDefinition::s_definitions.find(name);
	if(tileIterator != TileDefinition::s_definitions.end()){return tileIterator->second;}

	//ERROR_AND_DIE("Could not find definition");
	return nullptr;
}

TileDefinition::TileDefinition(const tinyxml2::XMLElement & definitionNode)
{
	m_name = ParseXmlAttribute(definitionNode,"name","Error");
	m_spriteCoords = ParseXmlAttribute(definitionNode, "spriteCoords", IntVector2(0,0));

	// Translate sprite coords to UV
	m_uvCoords = g_tileSpriteSheet.GetTexCoordsForSpriteCoords(m_spriteCoords);

	// Add definition to map
	s_definitions.insert(std::pair<std::string,TileDefinition*>(m_name,this));
}
