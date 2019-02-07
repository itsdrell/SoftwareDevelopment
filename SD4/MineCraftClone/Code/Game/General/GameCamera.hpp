#pragma once
#include "Engine/Math/Matrices/Matrix44.hpp"

//====================================================================================
// Forward Declare
//====================================================================================


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
class GameCamera
{
public:
	GameCamera() {}
	~GameCamera() {}

public:
	Vector3 GetForwardXY0();

	Matrix44 GetModelMatrix();
	Matrix44 GetViewMatrix();

public:
	float yawDegreesAboutZ = 0.f;
	float pitchDegreesAboutY = 0.f;
	float rollDegreesAboutX = 0.f;
	
	Vector3 pos = Vector3::ONE;
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