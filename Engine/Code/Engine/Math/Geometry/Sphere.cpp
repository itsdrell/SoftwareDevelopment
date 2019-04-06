#include "Sphere.hpp"
#include "Engine/Math/Geometry/AABB3.hpp"
#include "../MathUtils.hpp"

//===============================================================================================
Sphere::Sphere(const Vector3 & theCenter, float theRadius)
{
	center = theCenter;
	radius = theRadius;
}

//===============================================================================================
// c functions
//===============================================================================================
Vector3 CorrectiveSphereVsAABB3(const Sphere& theSphere, const AABB3& theBox)
{
	// get the closest point
	Vector3 closestPoint = Clamp(theSphere.center, theBox.mins, theBox.maxs);

	// get distance from closest point and center sphere
	float distance = GetDistance(closestPoint, theSphere.center);

	// see how much overlap there is, if it is negative then there is an overlap
	float overlap = (distance - theSphere.radius);

	// if it is less than disc radius squared, collision
	if (overlap < 0)
	{
		// get displacement
		Vector3 displacement = closestPoint - theSphere.center;
		Vector3 direction = displacement.Normalize();

		Vector3 amountToPush = direction * overlap;
		return amountToPush;
	}
	else
	{
		// no collision
		return Vector3(0.f);
	}
}
