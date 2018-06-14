#pragma once
#include "..\Vectors\Vector3.hpp"


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
class OBB3
{
public:


	bool IsContained(const Vector3& pos);


	bool GetCorners( Vector3 *out);

public:
	Vector3		center;
	Vector3		right;
	Vector3		up;
	Vector3		forward;
};

//--------------------------------------------------------------------------
//	Points
//		   6--------------7
//		  /|             /|
//		 / |            / |
//		2--+-----------3  |
//		|  |           |  |
//		|  |           |  |
//		|  |           |  |
//		|  4-----------+--5
//		| /            | /
//		|/             |/
//		0--------------1
//--------------------------------------------------------------------------


//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================