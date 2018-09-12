#include "CombatLookUpTable.hpp"
#include "Engine\Core\Utils\XmlUtilities.hpp"

//===============================================================================================
std::vector<CombatRelationship>	CombatLookUpTable::s_relationships;

//===============================================================================================
void CombatLookUpTable::AddRelationshipToTable(tinyxml2::XMLElement & node)
{
	String attackerName = ParseXmlAttribute(node, "attacker", "ERROR");
	String defenderName = ParseXmlAttribute(node, "defender", "ERROR");
	float theDamage = ParseXmlAttribute(node,"damage", 0.f);

	s_relationships.push_back(CombatRelationship(attackerName,defenderName,theDamage));
}

//-----------------------------------------------------------------------------------------------
Vector2 CombatLookUpTable::GetDamageDoneToBoth(const String & attacker, const String & defender)
{
	float attackerDamage = GetRelationship(attacker,defender).m_damage;
	float defenderDamage = GetRelationship(defender, attacker).m_damage;
	return Vector2(attackerDamage, defenderDamage);
}

//-----------------------------------------------------------------------------------------------
float CombatLookUpTable::GetDamage(const Unit& attacker, const Unit& defender)
{
	return GetRelationship(attacker.m_definition->m_name, defender.m_definition->m_name).m_damage;
}

//-----------------------------------------------------------------------------------------------
CombatRelationship CombatLookUpTable::GetRelationship(const String & attacker, const String & defender)
{
	for(uint i = 0; i < s_relationships.size(); i++)
	{
		CombatRelationship current = s_relationships.at(i);

		if((current.m_attacker == attacker) && (current.m_defender == defender))
		{
			return current;
		}
	}
	
	ERROR_RECOVERABLE("Could not find a relationship for: " + attacker + " vs " + defender);

	return CombatRelationship();
}
