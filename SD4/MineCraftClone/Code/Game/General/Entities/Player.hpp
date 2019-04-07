#pragma once
#include "Game/General/Entities/Entity.hpp"
#include "Engine/Math/Geometry/Sphere.hpp"

//====================================================================================
// Forward Declare
//====================================================================================
class World;
class HUD;
class BlockDefinition;
class GameCamera;

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
class Player : public Entity
{
public:
	Player(const Vector3& pos);

public:
	virtual void CreateMesh();

public:
	virtual void Update() override;
	virtual void CheckKeyboardInput() override;
	virtual void UpdateCollisionVolumesPositions() override;
	virtual void CorrectivePhysics() override;
	
	void GetMoveIntentions(const Vector3& forward, const Vector3& right);
	void CheckAndApplyMovementAndRotation();

public:
	virtual void Render() const;
	void DrawSelf() const;

public:
	void Dig();
	void Place();
	void NoClipAndFlyingMovement(const Vector3& forward, const Vector3& right);
	void MovementWithForces(const Vector3& forward, const Vector3& right);

public:
	HUD*					m_playerHUD = nullptr;
	BlockDefinition*		m_blockToPlace = nullptr;
	//GameCamera*				m_gameCamera = nullptr;

	Vector3					m_spawnLocation;
	Sphere					m_bottomPhysicsSphere;

	AABB3					m_collisionBox;
	
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