#pragma once
#include "Game/Main/GameCommon.hpp"
#include "Engine/Math/Vectors/Vector3.hpp"
#include "Engine/Math/Matrices/Matrix44.hpp"
#include "Game/General/Utils/BlockLocator.hpp"

//====================================================================================
// Forward Declare
//====================================================================================
class World;
class Sphere;
class AABB3;

//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================
enum PhysicsMode
{
	PHYSICS_MODE_NO_CLIP,
	PHYSICS_MODE_GRAVITY,
	PHYSICS_MODE_FLYING,
	NUM_PHYSICS_MODES
};
String PhysicsModeToString(PhysicsMode theMode);

//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class Entity
{
public:
	Entity();
	Entity(const Vector3& position);
	virtual ~Entity() {}

public:
	virtual void CreateMesh() = 0;

public:
	virtual void Update();
	virtual void CheckKeyboardInput();
	virtual void UpdateCollisionVolumesPositions() = 0;
	virtual void Render() const;

public:
	virtual void ApplyForces();
	virtual void CorrectivePhysics();
	virtual void PushSphereOutOfBoxes( Sphere& theSphere);
	virtual bool PushSphereEntityOutOfBox( Sphere& collider, const AABB3& theBox);

public:
	virtual Vector3 GetForwardXY0();
	virtual Vector3 GetForward();
	virtual Matrix44 GetModelMatrix();
	virtual Matrix44 GetViewMatrix();
	void GetAllPossibleCollisionBoxes( BlockLocator& centerBlockLocator, std::vector<BlockLocator>& boxes);

public:
	BlockLocator GetMyPositionsBlockLocator();
	BlockLocator GetBlockLocatorOfOffsetFromPosition(const Vector3& offset);
	BlockLocator GetBlockLocatorForColliderCenter( const Sphere& collider) const;

public:
	PhysicsMode		m_physicsType = PHYSICS_MODE_NO_CLIP;
	
	float yawDegreesAboutZ = 0.f;
	float pitchDegreesAboutY = 0.f;
	float rollDegreesAboutX = 0.f;
	

public:
	Vector3		m_position = Vector3::ZERO;
	Vector3		m_acceleration = Vector3::ZERO;
	Vector3		m_velocity = Vector3::ZERO;
	Vector3		m_willPowerMoveIntentions = Vector3::ZERO;
	Vector3		m_spawnLocation = Vector3::ZERO;
	Vector3		m_eyeOffsetFromCenter = Vector3::ZERO;
	Vector3		m_bottomOfEntityOffset = Vector3::ZERO;

public:
	float		m_willPowerStrength = 5.f;
	float		m_maxVelocity = 5.f;
	float		m_movementSpeed = 2.f;
	float		m_jumpForce = GRAVITY_SCALE * .6f;
	bool		m_isOnGround = false;
	

public:
	World*	m_worldTheyAreIn = nullptr;

};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [3/30/2019]
//====================================================================================