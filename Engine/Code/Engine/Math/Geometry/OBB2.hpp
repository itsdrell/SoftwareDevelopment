#pragma once
#include "..\Vectors\Vector2.hpp"

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

// 
class OBB2
{
public:
	OBB2();
	OBB2(const Vector2& theCenter, const Vector2& normalizedRight, const Vector2& halfDimensions);

	Vector2 GetUP() const;
	Vector2 GetNormalizedUp() const;
	Vector2 GetRight() const;

	// When dealing with OBB2s it's good to think about them in their own space
	// so we needs these 
	Vector2 GetLocalPositionFromWorld(const Vector2& worldPos) const;
	Vector2 GetWorldPositionFromLocal(const Vector2& localPos) const;

	Vector2 GetClosestPoint(const Vector2& worldPos) const;
	bool IsPointInBox(const Vector2& worldPos);

public:
	Vector2			m_center;
	Vector2			m_normalizedRight;
	Vector2			m_halfDimensions;
};

//====================================================================================
// Standalone C Functions
//====================================================================================
void PushDiscOutOfOBB2( Vector2& center, float theRadius, const OBB2& theBox );
bool DoesDiscOverlapOBB2( const Vector2& center, float theRadius, const OBB2& theBox);

//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [9/28/2018]
//====================================================================================