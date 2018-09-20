#include "CommandingOfficer.hpp"
#include "Engine\Core\Utils\XmlUtilities.hpp"

//===============================================================================================
std::map<String, CommandingOfficerDefinition*>		CommandingOfficerDefinition::s_definitions;

//===============================================================================================
CommandingOfficerDefinition::CommandingOfficerDefinition(tinyxml2::XMLElement & node)
{
	m_name = ParseXmlAttribute(node,"name","Error");

	tinyxml2::XMLElement* indexNode = node.FirstChildElement();
	while(indexNode)
	{
		String name = indexNode->Name();

		if(name == "power")
		{
			String powerName = ParseXmlAttribute(*indexNode, "name", "IDK");
			float amount = ParseXmlAttribute(*indexNode, "amount", 0.f);

			m_power = new COPower(powerName, amount);
		}

		if(name == "stats")
		{
			m_stats = new CommandingOfficerStat(*indexNode);
		}

		indexNode = indexNode->NextSiblingElement();
	}

	s_definitions.insert(std::pair<String, CommandingOfficerDefinition*>(m_name, this));
}

//-----------------------------------------------------------------------------------------------
CommandingOfficerDefinition::~CommandingOfficerDefinition()
{
	delete m_power;
	m_power = nullptr;

	delete m_stats;
	m_stats = nullptr;
}

//-----------------------------------------------------------------------------------------------
CommandingOfficerDefinition* CommandingOfficerDefinition::GetDefinition(const String& name)
{
	std::map<std::string,CommandingOfficerDefinition*>::iterator commanderIterator;
	commanderIterator = CommandingOfficerDefinition::s_definitions.find(name);
	if(commanderIterator != CommandingOfficerDefinition::s_definitions.end()){return commanderIterator->second;}

	ERROR_RECOVERABLE("Could not find Commanding Officer Resorce: " + name);

	return nullptr;
}

//-----------------------------------------------------------------------------------------------
void CommandingOfficerDefinition::DeleteAllDefinitions()
{
	std::map<std::string,CommandingOfficerDefinition*>::iterator tileIterator;
	tileIterator = CommandingOfficerDefinition::s_definitions.begin();

	while(tileIterator != CommandingOfficerDefinition::s_definitions.end())
	{
		CommandingOfficerDefinition* current = tileIterator->second;

		delete current;
		current = nullptr;

		tileIterator++;
	}

	s_definitions.clear();
}

//===============================================================================================
CommandingOfficer::CommandingOfficer(const String name, const TeamName & team, CO_TYPES typeOfController)
{
	m_name = name;
	m_team = team;
	m_money = 0U;

	m_definition = CommandingOfficerDefinition::GetDefinition(name);

	// make a controller 
	UNUSED(typeOfController);
}

//-----------------------------------------------------------------------------------------------
CommandingOfficer::CommandingOfficer( CommandingOfficerDefinition & definition, const TeamName & team, CO_TYPES typeOfController)
{
	m_team = team;
	m_money = 0U;
	m_name = m_definition->m_name;

	m_definition = &definition;
}

//-----------------------------------------------------------------------------------------------
CommandingOfficer::~CommandingOfficer()
{
	delete m_controller;
	m_controller = nullptr;
}


