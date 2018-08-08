#include "TileDefinition.hpp"
#include "Engine\Core\Utils\XmlUtilities.hpp"
#include "Engine\Core\General\EngineCommon.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteSheet.hpp"

#pragma warning( disable : 4239) // Strings parsing

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

TileDefinition* GetTileDefinition(Rgba& color)
{
	std::map<std::string,TileDefinition*>::iterator tileIterator;
	tileIterator = TileDefinition::s_definitions.begin();

	while(tileIterator != TileDefinition::s_definitions.end())
	{
		TileDefinition* current = tileIterator->second;

		if(current->m_colorRepresentation == color)
			return current;

		tileIterator++;
	}

	return nullptr;
	
}

TileDefinition::TileDefinition(const tinyxml2::XMLElement & definitionNode)
{
	m_name = ParseXmlAttribute(definitionNode,"name","Error");
	m_displayName = ParseXmlAttribute(definitionNode, "displayName", "ERROR");
	m_spriteCoords = ParseXmlAttribute(definitionNode, "spriteCoords", IntVector2(0,0));
	m_colorRepresentation = ParseXmlAttribute(definitionNode, "rgbaValue", Rgba(0,0,0));

	const Strings a;
	m_movementTags = ParseXmlAttribute(definitionNode, "movement", a);

	// Translate sprite coords to UV
	m_uvCoords = g_tileSpriteSheet.GetTexCoordsForSpriteCoords(m_spriteCoords);

	// Add definition to map
	s_definitions.insert(std::pair<std::string,TileDefinition*>(m_name,this));
}

void TileDefinition::DeleteAllDefinitions()
{
	std::map<std::string,TileDefinition*>::iterator tileIterator;
	tileIterator = TileDefinition::s_definitions.begin();

	while(tileIterator != TileDefinition::s_definitions.end())
	{
		TileDefinition* current = tileIterator->second;

		delete current;
		current = nullptr;

		tileIterator++;
	}

	s_definitions.clear();
}
