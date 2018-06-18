#pragma once
#include "Engine\Math\Geometry\AABB2.hpp"


//====================================================================================
// Forward Declare
//====================================================================================
class Renderable;

//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class HUD
{
public:
	HUD();

	void Update();
	void UpdateReloadBar();
	void UpdateTextUI();
	void UpdateHP();

public:
	AABB2			m_cooldownBounds;
	Renderable*		m_reloadBarCurrentRenderable;
	Renderable*		m_reloadBarBackgroundRenderable;

	Renderable*		m_enemiesAliveRenderable;
	Renderable*		m_spawnersLeftRenderable;

	Renderable*		m_healthBar;
};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : June 2018
//====================================================================================