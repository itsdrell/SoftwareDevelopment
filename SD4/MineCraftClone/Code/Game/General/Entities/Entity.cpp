#include "Entity.hpp"
#include "Game/General/World/World.hpp"
#include "Engine/Core/Tools/Clock.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Geometry/Sphere.hpp"
#include "Engine/Math/Geometry/AABB3.hpp"
#include "../World/Chunk.hpp"


//===============================================================================================
String PhysicsModeToString(PhysicsMode theMode)
{
	switch (theMode)
	{
	case PHYSICS_MODE_NO_CLIP:
		return "No Clip";
	case PHYSICS_MODE_GRAVITY:
		return "Gravity";
	case PHYSICS_MODE_FLYING:
		return "Flying";
	case NUM_PHYSICS_MODES:
		return "number";
	default:
		return "number";
	}
}

//===============================================================================================
Entity::Entity()
{
}

//-----------------------------------------------------------------------------------------------
Entity::Entity(const Vector3 & position)
{
	m_position = position;
	m_spawnLocation = m_position;
}

//-----------------------------------------------------------------------------------------------
void Entity::Update()
{
	CheckKeyboardInput();
	ApplyForces();
	UpdateCollisionVolumesPositions();
	if(m_physicsType != PHYSICS_MODE_NO_CLIP) CorrectivePhysics();
	UpdateCollisionVolumesPositions();
}

//-----------------------------------------------------------------------------------------------
void Entity::CheckKeyboardInput()
{
	if (IsKeyPressed(KEYBOARD_SHIFT))
	{
		m_movementSpeed = 50.f;
	}
	else
	{
		m_movementSpeed = 10.f;
	}

	if (WasKeyJustPressed(G_THE_LETTER_R))
	{
		m_position = m_spawnLocation;
		m_velocity = Vector3::ZERO;
	}

	if (WasKeyJustPressed(KEYBOARD_F1))
	{
		m_physicsType = (PhysicsMode) ((m_physicsType + 1) % NUM_PHYSICS_MODES);
	}
}

//-----------------------------------------------------------------------------------------------
void Entity::Render() const
{
	Renderer* r = Renderer::GetInstance();

	Vector3 endPos = m_position + (m_velocity * g_theGameClock->deltaTime * 2.f);
	r->DrawLine3D(m_position, endPos, 1.f, Rgba::YELLOW);
}

//-----------------------------------------------------------------------------------------------
void Entity::ApplyForces()
{
	float ds = g_theGameClock->deltaTime;
	
	Vector2 frictionForceXY = Vector2(-m_velocity.x * FRICTION_SCALE, -m_velocity.y * FRICTION_SCALE);
	float frictionForceZ = -m_velocity.z * FRICTION_SCALE;
	if (m_physicsType == PHYSICS_MODE_GRAVITY) frictionForceZ = 0.f;

	// make some forces
	Vector2 willPowerForceXY = FRICTION_SCALE * m_willPowerStrength * m_willPowerMoveIntentions.xy();
	float willPowerForceZ = FRICTION_SCALE* m_willPowerStrength * m_willPowerMoveIntentions.z;  // should be frictionZ * m_flyStrength * m_willPowerMoveIntentions.z

	// compute total fores
	Vector2 totalForceXY = willPowerForceXY + frictionForceXY; // gravitForceXY + willpowerForceXY + frictionForceXY

	float gravityScale = GRAVITY_SCALE;
	if (m_physicsType != PHYSICS_MODE_GRAVITY)
		gravityScale = 0;
	float totalForceZ = -gravityScale + willPowerForceZ + frictionForceZ; // gravityFroceZ + willpowerForceZ + frictionForceZ

	float speedBeforeXY = m_velocity.xy().GetLength();
	float speedBeforeZ = m_velocity.z;

	Vector2 totalDeltaVelocityXY = totalForceXY * ds; // force is a change in momentum per second, we want change in momentum NOW #todo divide by mass
	float totalDeltaVelocityZ = totalForceZ * ds; // for now, force is same as acceleration since mass is assumed 1,0 for everyone #todo divide by mass

	m_velocity += Vector3(totalDeltaVelocityXY);
	m_velocity += Vector3(0.f, 0.f, totalDeltaVelocityZ);

	float speedAfterXY = m_velocity.xy().GetLength();
	//float speedAfterZ = m_velocity.z;

	if (speedAfterXY > speedBeforeXY)
	{
		float speedLimit = Max(m_maxVelocity, speedBeforeXY);
		Vector3 previousVelocity = m_velocity;
		Vector3 clampedVelocity = m_velocity.Normalize() * m_maxVelocity;

		m_velocity = Vector3(clampedVelocity.x, clampedVelocity.y, previousVelocity.z);

	}

	// apply all the forces
	m_position += (m_velocity * ds);
}

//-----------------------------------------------------------------------------------------------
void Entity::CorrectivePhysics()
{
}

//-----------------------------------------------------------------------------------------------
void Entity::PushSphereOutOfBoxes( Sphere & theSphere)
{
	BlockLocator myBlockLocator = GetBlockLocatorForColliderCenter(theSphere);
	std::vector<BlockLocator> rubixCube;
	GetAllPossibleCollisionBoxes(myBlockLocator, rubixCube);

	for (uint i = 0; i < rubixCube.size(); i++)
	{
		BlockLocator& current = rubixCube.at(i);

		if (current.IsSolid())
		{
			AABB3 bounds = current.GetBlockBounds();
			bool didWePush = PushSphereEntityOutOfBox(theSphere, bounds);

			if (i == 0)
				m_isOnGround = didWePush;
		}
	}
}

//-----------------------------------------------------------------------------------------------
bool Entity::PushSphereEntityOutOfBox(Sphere & collider, const AABB3 & theBox)
{
	// get the closest point
	Vector3 closestPoint = Clamp(collider.center, theBox.mins, theBox.maxs);

	// get distance from closest point and center sphere
	float distance = GetDistance(closestPoint, collider.center);

	// see how much overlap there is, if it is negative then there is an overlap
	float overlap = (distance - collider.radius);

	// if it is less than disc radius squared, collision
	if (overlap < 0)
	{
		// get displacement
		Vector3 displacement = closestPoint - collider.center;
		Vector3 direction = displacement.Normalize();

		Vector3 amountToPush = direction * overlap;

		// move the position AND the collider
		m_position += amountToPush;
		collider.center += amountToPush;

		// kill velocity
		float displacementProjectOnVelocityStrength = DotProduct(direction, m_velocity);
		m_velocity -= (direction * displacementProjectOnVelocityStrength);

		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------
Vector3 Entity::GetForwardXY0()
{
	return Vector3(CosDegrees(yawDegreesAboutZ), SinDegrees(yawDegreesAboutZ), 0);
}

//-----------------------------------------------------------------------------------------------
Vector3 Entity::GetForward()
{
	Matrix44 theMatrix = Matrix44();

	theMatrix.Append(Matrix44::MakeRotationDegreesAroundZ(yawDegreesAboutZ));
	theMatrix.Append(Matrix44::MakeRotationDegreesAroundY(pitchDegreesAboutY));
	theMatrix.Append(Matrix44::MakeRotationDegreesAroundX(rollDegreesAboutX));

	return theMatrix.GetForward();
}

//-----------------------------------------------------------------------------------------------
Matrix44 Entity::GetModelMatrix()
{
	Matrix44 theMatrix = Matrix44();

	theMatrix.Append(Matrix44::MakeTranslation3D(m_position));
	theMatrix.Append(Matrix44::MakeRotationDegreesAroundZ(yawDegreesAboutZ));
	theMatrix.Append(Matrix44::MakeRotationDegreesAroundY(pitchDegreesAboutY));
	theMatrix.Append(Matrix44::MakeRotationDegreesAroundX(rollDegreesAboutX));

	return theMatrix;
}

//-----------------------------------------------------------------------------------------------
Matrix44 Entity::GetViewMatrix()
{
	Matrix44 view = Matrix44(
		Vector4(0, 0, 1, 0),
		Vector4(-1, 0, 0, 0),
		Vector4(0, 1, 0, 0),
		Vector4(0, 0, 0, 1));

	view.Append(Matrix44::MakeRotationDegreesAroundX(-rollDegreesAboutX));
	view.Append(Matrix44::MakeRotationDegreesAroundY(-pitchDegreesAboutY));
	view.Append(Matrix44::MakeRotationDegreesAroundZ(-yawDegreesAboutZ));
	view.Append(Matrix44::MakeTranslation3D(-m_position));

	return view;
}

//-----------------------------------------------------------------------------------------------
void Entity::GetAllPossibleCollisionBoxes( BlockLocator& centerBlockLocator, std::vector<BlockLocator>& boxes)
{
	// tier one is cardinal 
	
	// below is always first so we can check for is ground
	boxes.push_back(centerBlockLocator.GetBlockLocatorOfBelowNeighbor());
	boxes.push_back(centerBlockLocator.GetBlockLocatorOfAboveNeighbor());
	boxes.push_back(centerBlockLocator.GetBlockLocatorOfNorthNeighbor());
	boxes.push_back(centerBlockLocator.GetBlockLocatorOfSouthNeighbor());
	boxes.push_back(centerBlockLocator.GetBlockLocatorOfEastNeighbor());
	boxes.push_back(centerBlockLocator.GetBlockLocatorOfWestNeighbor());

	// tier two
	boxes.push_back(centerBlockLocator.GetBlockLocatorOfSouthNeighbor().GetBlockLocatorOfEastNeighbor());
	boxes.push_back(centerBlockLocator.GetBlockLocatorOfSouthNeighbor().GetBlockLocatorOfWestNeighbor());
	boxes.push_back(centerBlockLocator.GetBlockLocatorOfSouthNeighbor().GetBlockLocatorOfAboveNeighbor());
	boxes.push_back(centerBlockLocator.GetBlockLocatorOfSouthNeighbor().GetBlockLocatorOfBelowNeighbor());

	boxes.push_back(centerBlockLocator.GetBlockLocatorOfNorthNeighbor().GetBlockLocatorOfEastNeighbor());
	boxes.push_back(centerBlockLocator.GetBlockLocatorOfNorthNeighbor().GetBlockLocatorOfWestNeighbor());
	boxes.push_back(centerBlockLocator.GetBlockLocatorOfNorthNeighbor().GetBlockLocatorOfAboveNeighbor());
	boxes.push_back(centerBlockLocator.GetBlockLocatorOfNorthNeighbor().GetBlockLocatorOfBelowNeighbor());

	boxes.push_back(centerBlockLocator.GetBlockLocatorOfAboveNeighbor().GetBlockLocatorOfEastNeighbor());
	boxes.push_back(centerBlockLocator.GetBlockLocatorOfAboveNeighbor().GetBlockLocatorOfWestNeighbor());
	boxes.push_back(centerBlockLocator.GetBlockLocatorOfBelowNeighbor().GetBlockLocatorOfEastNeighbor());
	boxes.push_back(centerBlockLocator.GetBlockLocatorOfBelowNeighbor().GetBlockLocatorOfWestNeighbor());


	// tier three is 

}

//-----------------------------------------------------------------------------------------------
BlockLocator Entity::GetMyPositionsBlockLocator()
{
	ChunkCoords theChunkCoords = Chunk::GetChunkCoordsFromWorldPosition(m_position);
	Chunk* myChunk = m_worldTheyAreIn->GetChunkFromChunkCoords(theChunkCoords);
	if (myChunk == nullptr) return BlockLocator(nullptr, -1);
	BlockIndex bi = myChunk->GetBlockIndexForWorldCoords(m_position);

	BlockLocator myLocator = BlockLocator(myChunk, bi);
	if (!myLocator.IsValid()) return BlockLocator(nullptr, -1);

	return myLocator;
}

//-----------------------------------------------------------------------------------------------
BlockLocator Entity::GetBlockLocatorOfOffsetFromPosition(const Vector3& offset)
{
	ChunkCoords theChunkCoords = Chunk::GetChunkCoordsFromWorldPosition(m_position + offset);
	Chunk* myChunk = m_worldTheyAreIn->GetChunkFromChunkCoords(theChunkCoords);
	if (myChunk == nullptr) return BlockLocator(nullptr, -1);
	BlockIndex bi = myChunk->GetBlockIndexForWorldCoords(m_position + offset);

	BlockLocator myLocator = BlockLocator(myChunk, bi);
	if (!myLocator.IsValid()) return BlockLocator(nullptr, -1);

	return myLocator;
}

//-----------------------------------------------------------------------------------------------
BlockLocator Entity::GetBlockLocatorForColliderCenter(const Sphere& collider) const
{
	ChunkCoords theChunkCoords = Chunk::GetChunkCoordsFromWorldPosition(collider.center);
	Chunk* myChunk = m_worldTheyAreIn->GetChunkFromChunkCoords(theChunkCoords);
	if (myChunk == nullptr) return BlockLocator(nullptr, -1);
	BlockIndex bi = myChunk->GetBlockIndexForWorldCoords(collider.center);

	BlockLocator myLocator = BlockLocator(myChunk, bi);
	if (!myLocator.IsValid()) return BlockLocator(nullptr, -1);

	return myLocator;
}



