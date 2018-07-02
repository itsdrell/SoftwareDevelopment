#pragma once
#include "Game\SystemsAndTools\GameObject2D.hpp"
#include "Game\SystemsAndTools\Tags.hpp"
#include "Game\Main\GameCommon.hpp"
#include "Engine\Math\Ranges\IntRange.hpp"


//=============================================================
// Forward Declare
//=============================================================
class Tile;

//=============================================================
// ENUMS
//=============================================================


//=============================================================
// Structs
//=============================================================


//=============================================================
// Classes
//=============================================================
class Unit : public GameObject2D
{
public:
	Unit(TeamName team);

	void Update();

public:
	TeamName	m_team;
	uint		m_health;

	bool		m_beenMoved;
	bool		m_usedAction;

	IntRange	m_attackRange;

	Tags		m_tags;
	Tile*		m_tileIAmOn;
};

//=============================================================
// Standalone C Functions
//=============================================================
float CalculateWinChance(const Unit& attacking, const Unit& defending);

//=============================================================
// Externs
//=============================================================
