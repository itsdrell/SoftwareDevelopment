#include "Entity.hpp"
#include "Game/General/World/World.hpp"

//===============================================================================================
Entity::Entity()
{
}

//-----------------------------------------------------------------------------------------------
Entity::Entity(const Vector3 & position)
{
	m_position = position;
}

//-----------------------------------------------------------------------------------------------
void Entity::Update()
{
	// gravity?
}

//-----------------------------------------------------------------------------------------------
void Entity::Render() const
{
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
