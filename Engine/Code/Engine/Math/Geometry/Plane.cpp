#include "Plane.hpp"
#include "..\..\Core\Tools\ErrorWarningAssert.hpp"
#include "..\MathUtils.hpp"

Plane::Plane(const Vector3 & theNormal, const Vector3 & pos)
{
	distance = DotProduct(normal, pos);
	normal = normal;
}

Plane::Plane(const Vector3 & a, const Vector3 & b, const Vector3 & c)
{
	Vector3 e0 = b - a;
	Vector3 e1 = c - a;

	normal = Cross( e1, e0);

	GUARANTEE_OR_DIE( !IsNearZero(normal.GetLength()), "Plane Construction Failed");

	normal.Normalize();
	distance = DotProduct(normal, a);
}

float Plane::GetDistance(const Vector3 & pos) const
{
	float dist = DotProduct(pos, normal);
	return dist - distance;
}

bool Plane::IsInFront(const Vector3 & pos) const
{
	return GetDistance(pos) > 0.f;
}

void Plane::FlipNormal()
{
	// Gotta do both, not just the normal
	normal = -normal;
	distance = -distance;
}
