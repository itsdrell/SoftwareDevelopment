#include "Entity.hpp"
#include "Game/General/World/World.hpp"
#include "Engine/Core/Tools/Clock.hpp"
#include "Engine/Math/MathUtils.hpp"

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

	// make some forces
	Vector2 willPowerForceXY = FRICTION_SCALE * m_willPowerStrength * m_willPowerMoveIntentions.xy();
	float willPowerForceZ = m_willPowerStrength * m_willPowerMoveIntentions.z;  // should be frictionZ * m_flyStrength * m_willPowerMoveIntentions.z

	// compute total fores
	Vector2 totalForceXY = willPowerForceXY + frictionForceXY; // gravitForceXY + willpowerForceXY + frictionForceXY
	float totalForceZ = -GRAVITY_SCALE + willPowerForceZ; // gravityFroceZ + willpowerForceZ + frictionForceZ

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
void Entity::GetForward()
{

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
