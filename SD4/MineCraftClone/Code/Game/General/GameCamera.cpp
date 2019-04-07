#include "GameCamera.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "../Main/GameCommon.hpp"
#include "Engine/Core/Tools/Clock.hpp"
#include "Game/General/Entities/Entity.hpp"


//-----------------------------------------------------------------------------------------------
String CameraModeToString(CameraMode mode)
{
	switch (mode)
	{
	case CAMERA_MODE_MANUAL:
		return "Manual";
		break;
	case CAMERA_MODE_FIRST_PERSON:
		return "First Person";
		break;
	case CAMERA_MODE_OVER_THE_SHOULDER:
		return "Over the shoulder";
		break;
	case CAMERA_MODE_FIXED_ANGLE_TRACKING:
		return "fixed angle tracking";
		break;
	case CAMERA_MODE_INDEPENDENT:
		return "independent";
		break;
	case NUM_OF_CAMERA_MODES:
		break;
	default:
		break;
	}
}

//-----------------------------------------------------------------------------------------------
void GameCamera::Update()
{
	switch (m_mode)
	{
	case CAMERA_MODE_MANUAL:
		ManualUpdate();
		break;
	case CAMERA_MODE_FIRST_PERSON:
		FirstPersonUpdate();
		break;
	case CAMERA_MODE_OVER_THE_SHOULDER:
		break;
	case CAMERA_MODE_FIXED_ANGLE_TRACKING:
		break;
	case CAMERA_MODE_INDEPENDENT:
		break;
	case NUM_OF_CAMERA_MODES:
		break;
	default:
		break;
	}

	if (WasKeyJustPressed(G_THE_LETTER_C))
		SwapCameraMode();
}

//-----------------------------------------------------------------------------------------------
void GameCamera::ManualUpdate()
{
	CheckKeyboardInput();
}

//-----------------------------------------------------------------------------------------------
void GameCamera::FirstPersonUpdate()
{
	pos = (m_entityToFollow->m_position + m_entityToFollow->m_eyeOffsetFromCenter);

	rollDegreesAboutX = m_entityToFollow->rollDegreesAboutX;
	pitchDegreesAboutY = m_entityToFollow->pitchDegreesAboutY;
	yawDegreesAboutZ = m_entityToFollow->yawDegreesAboutZ;

}

//-----------------------------------------------------------------------------------------------
void GameCamera::SwapCameraMode()
{
	if (m_mode == CAMERA_MODE_FIRST_PERSON)
	{
		m_mode = CAMERA_MODE_MANUAL;
		return;
	}
	else
	{
		m_mode = CAMERA_MODE_FIRST_PERSON;
	}

	
	//m_mode = (CameraMode)(m_mode + 1);
	//
	//if (m_mode == NUM_OF_CAMERA_MODES)
	//	m_mode = (CameraMode)0;
}

//-----------------------------------------------------------------------------------------------
void GameCamera::CheckKeyboardInput()
{
	if (IsKeyPressed(KEYBOARD_SHIFT))
	{
		m_movementSpeed = 50.f;
	}
	else
	{
		m_movementSpeed = 10.f;
	}
	
	CheckAndApplyMovementAndRotation();
}

//-----------------------------------------------------------------------------------------------
void GameCamera::CheckAndApplyMovementAndRotation()
{
	// Apply Rotation
	Vector2 mouse_delta = g_theInput->GetMouseDelta();

	pitchDegreesAboutY += mouse_delta.y * .5f;
	yawDegreesAboutZ -= mouse_delta.x * 1.5f;

	pitchDegreesAboutY = ClampFloat(pitchDegreesAboutY, -90.f, 90.f);
	yawDegreesAboutZ = fmod(yawDegreesAboutZ, 360.f);

	// movement
	Vector3 amountToMove = Vector3::ZERO;
	float ds = g_theGameClock->deltaTime;

	Vector3 forward = Vector3(CosDegrees(yawDegreesAboutZ), SinDegrees(yawDegreesAboutZ), 0.f);
	Vector3 right = Vector3(forward.y, -forward.x, 0.f);

	if (IsKeyPressed(G_THE_LETTER_W))
		amountToMove = forward;
	if (IsKeyPressed(G_THE_LETTER_S))
		amountToMove = -forward;
	if (IsKeyPressed(G_THE_LETTER_A))
		amountToMove = -right;
	if (IsKeyPressed(G_THE_LETTER_D))
		amountToMove = right;
	if (IsKeyPressed(G_THE_LETTER_E))
		amountToMove = Vector3::UP;
	if (IsKeyPressed(G_THE_LETTER_Q))
		amountToMove = Vector3::DOWN;

	if (amountToMove != Vector3::ZERO)
		pos += (amountToMove * ds * m_movementSpeed);
}

//-----------------------------------------------------------------------------------------------
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


