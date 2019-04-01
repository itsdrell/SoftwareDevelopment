#pragma once
#include "Engine/Math/Matrices/Matrix44.hpp"

//====================================================================================
// Forward Declare
//====================================================================================
class Entity;

//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================
enum CameraMode
{
	CAMERA_MODE_MANUAL, // someone is controlling it
	CAMERA_MODE_FIRST_PERSON,
	CAMERA_MODE_OVER_THE_SHOULDER,
	CAMERA_MODE_FIXED_ANGLE_TRACKING,
	CAMERA_MODE_INDEPENDENT, // neesargs thesis
	NUM_OF_CAMERA_MODES
};
String CameraModeToString(CameraMode mode);

//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class GameCamera
{
public:
	GameCamera() {}
	~GameCamera() {}


public:
	virtual void Update();
	void ManualUpdate();
	void FirstPersonUpdate();


public:
	void SwapCameraMode();
	void CheckKeyboardInput();
	void SetCameraMode(CameraMode modeToBeIn) { m_mode = modeToBeIn; }
	void CheckAndApplyMovementAndRotation();

public:
	Vector3 GetForwardXY0();

	Matrix44 GetModelMatrix();
	Matrix44 GetViewMatrix();

public:
	Entity*			m_entityToFollow = nullptr;
	CameraMode		m_mode = CAMERA_MODE_MANUAL;

public:
	float yawDegreesAboutZ = 0.f;
	float pitchDegreesAboutY = 0.f;
	float rollDegreesAboutX = 0.f;
	
	Vector3 pos = Vector3::ONE;

	float m_movementSpeed = 20.f;
};


//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [2/4/2019]
//====================================================================================