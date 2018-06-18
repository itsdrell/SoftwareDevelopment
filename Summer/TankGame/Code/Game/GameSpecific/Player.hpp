#pragma once
#include "Engine\Core\General\GameObject.hpp"
#include "Engine\Core\Tools\Stopwatch.hpp"

//=============================================================
// Forward Declare
//=============================================================


//=============================================================
// ENUMS
//=============================================================


//=============================================================
// Structs
//=============================================================


//=============================================================
// Classes
//=============================================================
class Player : public GameObject
{
public:

	Player();

	void Update();
	
	void Reset();

	void TakeDamage();

public:

	Transform		m_cameraLocation;


	Timer*			m_breadCrumbTimer;

	Timer*			m_damageCooldown;
	
	int				m_maxHealth;
	int				m_currentHealth;

};

//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
