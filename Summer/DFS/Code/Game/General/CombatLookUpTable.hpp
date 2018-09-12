#pragma once
#include "Engine\Core\General\EngineCommon.hpp"
#include "Engine\ThirdParty\tinyxml\tinyxml2.h"
#include "GameObjects\Unit.hpp"

//====================================================================================
// Forward Declare
//====================================================================================


//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================
struct CombatRelationship
{
	CombatRelationship() {} // for returning if we don't find something
	
	CombatRelationship(const String& attacker, const String& def, float damage)
		: m_attacker(attacker)
		, m_defender(def)
		, m_damage(damage) {}
	
	String		m_attacker;
	String		m_defender;
	float		m_damage;

};

//====================================================================================
// Classes
//====================================================================================
class CombatLookUpTable
{
public:
	static void AddRelationshipToTable(tinyxml2::XMLElement& node);

	// returns a vector 2 with damage(attacker will do, defender will do)
	static Vector2 GetDamageDoneToBoth(const String& attacker, const String& defender);
	
	static float GetDamage(const Unit& attacker, const Unit& defender);

	static CombatRelationship GetRelationship(const String& attacker, const String& defender);

private:
	static std::vector<CombatRelationship>		s_relationships;
};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [9/11/2018]
//====================================================================================