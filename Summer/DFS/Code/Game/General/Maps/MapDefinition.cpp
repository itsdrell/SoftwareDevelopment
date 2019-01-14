#include "MapDefinition.hpp"
#include "Engine\Core\Utils\XmlUtilities.hpp"

//===============================================================================================
std::map<std::string, MapDefinition*>	MapDefinition::s_definitions;
const char*								MapDefinition::s_definitionPath = "Data/Definitions/Maps.xml";

//===============================================================================================
MapDefinition::MapDefinition(tinyxml2::XMLElement & node)
{
	m_name = ParseXmlAttribute(node, "name", "ERROR");
	m_scriptPath = ParseXmlAttribute(node, "script", "");
	m_imagePath = ParseXmlAttribute(node, "image", "");

	s_definitions.insert(std::pair<String, MapDefinition*>(m_name, this));
}

MapDefinition * MapDefinition::GetDefinition(String name)
{
	std::map<std::string,MapDefinition*>::iterator mapIterator;
	mapIterator = MapDefinition::s_definitions.find(name);
	if(mapIterator != MapDefinition::s_definitions.end()){ return mapIterator->second;}

	//ERROR_AND_DIE("Could not find definition");

	return nullptr;
}

//-----------------------------------------------------------------------------------------------
void MapDefinition::DeleteAllDefinitions()
{
	std::map<std::string,MapDefinition*>::iterator theIterator;
	theIterator = MapDefinition::s_definitions.begin();

	while(theIterator != MapDefinition::s_definitions.end())
	{
		MapDefinition* current = theIterator->second;

		delete current;
		current = nullptr;

		theIterator++;
	}

	s_definitions.clear();
}
