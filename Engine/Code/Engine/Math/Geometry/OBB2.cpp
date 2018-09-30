#include "OBB2.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Disc2.hpp"

//-----------------------------------------------------------------------------------------------
OBB2::OBB2()
{
	m_center = Vector2();
	m_normalizedRight = Vector2::EAST;
	m_halfDimensions = Vector2::ONE;
}

//-----------------------------------------------------------------------------------------------
OBB2::OBB2(const Vector2 & theCenter, const Vector2 & normalizedRight, const Vector2 & halfDimensions)
{
	m_center = theCenter;
	m_normalizedRight = normalizedRight;
	m_halfDimensions = halfDimensions;
}

//-----------------------------------------------------------------------------------------------
Vector2 OBB2::GetUP() const
{
	return GetNormalizedUp() * m_halfDimensions.y;
}

//-----------------------------------------------------------------------------------------------
Vector2 OBB2::GetNormalizedUp() const
{
	return Vector2(-m_normalizedRight.y, m_normalizedRight.x);
}

//-----------------------------------------------------------------------------------------------
Vector2 OBB2::GetRight() const
{
	return m_normalizedRight * m_halfDimensions.x;
}

//-----------------------------------------------------------------------------------------------
Vector2 OBB2::GetLocalPositionFromWorld(const Vector2 & worldPos) const
{
	// Get displacement from point
	Vector2 displacement = worldPos - m_center;
	float theX = DotProduct(displacement, m_normalizedRight);
	float theY = DotProduct(displacement, GetNormalizedUp());
	
	return Vector2(theX, theY);
}

//-----------------------------------------------------------------------------------------------
Vector2 OBB2::GetWorldPositionFromLocal(const Vector2 & localPos) const
{
	Vector2 rightPart = localPos.x * m_normalizedRight;
	Vector2 upPart = localPos.y * GetNormalizedUp();
	Vector2 result = m_center + upPart + rightPart;
	
	return result;
}

//-----------------------------------------------------------------------------------------------
Vector2 OBB2::GetClosestPoint(const Vector2& worldPos) const
{
	Vector2 localPos = GetLocalPositionFromWorld(worldPos);

	float closetLocalX = ClampFloat(localPos.x, -m_halfDimensions.x, m_halfDimensions.x);
	float closetLocalY = ClampFloat(localPos.y, -m_halfDimensions.y, m_halfDimensions.y);

	return GetWorldPositionFromLocal(Vector2(closetLocalX, closetLocalY));
}

//-----------------------------------------------------------------------------------------------
bool OBB2::IsPointInBox(const Vector2& worldPos)
{
	Vector2 localPos = GetLocalPositionFromWorld(worldPos);

	if(localPos.x > m_halfDimensions.x ||
		localPos.x < -m_halfDimensions.x ||
		localPos.y > m_halfDimensions.y || 
		localPos.y < - m_halfDimensions.y)
	{
		return false;
	}

	return true;
}

//===============================================================================================
void PushDiscOutOfOBB2(Vector2& center, float theRadius, const OBB2& theBox)
{
	Vector2 closestPoint = theBox.GetClosestPoint(center);
	center = CorrectiveDiscVsDot(Disc2(center, theRadius), closestPoint);
}

//-----------------------------------------------------------------------------------------------
bool DoesDiscOverlapOBB2(const Vector2& center, float theRadius, const OBB2& theBox)
{
	Vector2 closestPoint = theBox.GetClosestPoint(center);
	Disc2 theDisc(center, theRadius);

	return theDisc.IsPointInside(closestPoint);
}
