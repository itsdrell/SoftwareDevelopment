#pragma once
#include "Engine\Core\General\GameObject.hpp"
#include "Engine\Core\Tools\Command.hpp"

//====================================================================================
// Forward Declare
//====================================================================================


//====================================================================================
// Type Defs + Defines
//====================================================================================

// Weights are ranges 0-1
extern float seekWeight;
extern float alignmentWeight;
extern float seperationWeight;


constexpr float	neighborRadius = 10.f;
constexpr float personalSpace = 4.f;

//constexpr Vector3 forceLimit;

//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class Enemy : public GameObject
{
public:
	Enemy(const Vector3& pos);
	virtual ~Enemy();

	void Update();

	void ApplyForce(const Vector3& theForce);
	void GenerateForces();
	
	Vector3 Seek();
	Vector3	Alignment(); 
	Vector3 Seperation();

public:
	float			m_speed;
	float			m_radius;


	Vector3			m_velocity;

	Transform		m_eyeTransform;
	Renderable*		m_eyes;
};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================
void SetSeekWeight(Command& theCommand);
void SetAlignmentWeight(Command& theCommand);
void SetSeperationWeight(Command& theCommand);


//====================================================================================
// Written by Zachary Bracken : June 2018
//====================================================================================