#include "COPower.hpp"
#include "Map.hpp"
#include "GameObjects\Unit.hpp"
#include "Player\CommandingOfficer.hpp"

//===============================================================================================
// CO Powers
COPower::COPower(const String & nameOfPower, float amount)
{
	m_nameOfPower = nameOfPower;
	m_amount = amount;

	AssignCallbacks();
}

//-----------------------------------------------------------------------------------------------
COPower::~COPower()
{
	m_start = nullptr;
	m_end = nullptr;
}

//-----------------------------------------------------------------------------------------------
void COPower::UsePower()
{
	m_start(m_amount);
}

//-----------------------------------------------------------------------------------------------
void COPower::AssignCallbacks()
{
	if(m_nameOfPower == "heal")
	{
		m_start = HealAllUnits;
		m_end = nullptr;
	}
}

//===============================================================================================
void HealAllUnits(float amount)
{
	CommandingOfficer& currentOfficer = *g_theCurrentMap->m_currentOfficer;
	std::vector<Unit*>& listOfUnits = g_theCurrentMap->m_units;

	for(uint i = 0; i < listOfUnits.size(); i++)
	{
		Unit& currentUnit = *listOfUnits.at(i);

		if(currentUnit.m_team == currentOfficer.m_team)
		{
			currentUnit.AddHealth( (int) amount);
		}
	}
}
