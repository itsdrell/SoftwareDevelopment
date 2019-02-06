#include "GameCamera.hpp"
#include "Engine/Math/MathUtils.hpp"


//===============================================================================================
Vector3 GameCamera::GetForwardXY0()
{
	// returns a normalized vector (direction)
	return Vector3(CosDegrees(yawDegreesAboutZ), SinDegrees(yawDegreesAboutZ), 0.f);
}

//-----------------------------------------------------------------------------------------------
Matrix44 GameCamera::GetModelMatrix()
{
	Matrix44 theMatrix =  Matrix44();

	theMatrix.Append(Matrix44::MakeTranslation3D(pos));
	theMatrix.Append(Matrix44::MakeRotationDegreesAroundZ(yawDegreesAboutZ));
	theMatrix.Append(Matrix44::MakeRotationDegreesAroundY(pitchDegreesAboutY));
	theMatrix.Append(Matrix44::MakeRotationDegreesAroundX(rollDegreesAboutX));

	return theMatrix;
}

//-----------------------------------------------------------------------------------------------
Matrix44 GameCamera::GetViewMatrix()
{
	Matrix44 view = Matrix44(
		Vector4(0,0,1,0),
		Vector4(-1,0,0,0), 
		Vector4(0,1,0,0), 
		Vector4(0,0,0,1));

	view.Append(Matrix44::MakeRotationDegreesAroundX(-rollDegreesAboutX));
	view.Append(Matrix44::MakeRotationDegreesAroundY(-pitchDegreesAboutY));
	view.Append(Matrix44::MakeRotationDegreesAroundZ(-yawDegreesAboutZ));
	view.Append(Matrix44::MakeTranslation3D(-pos));

	return view;
}
