#include "AABB3.hpp"
#include "Engine\Core\General/EngineCommon.hpp"


AABB3::AABB3(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax)
{
	mins = Vector3(xmin, ymin, zmin);
	maxs = Vector3(xmax, ymax, zmax);
}

AABB3::AABB3(const Vector3 & mins, const Vector3 & maxs)
{
	this->mins = mins;
	this->maxs = maxs;
}

bool AABB3::IsValid() const
{
	return (maxs.x >= mins.x);
}

void AABB3::Invalidate()
{
	mins = Vector3(INFINITY);
	maxs = Vector3(-INFINITY);
}

bool AABB3::IsContained(const Vector3 & pos)
{
	//return (pos >= mins) && (pos <= maxs);
	return false;
}

Vector3 AABB3::GetCenter() const
{
	float xx = maxs.x - (maxs.x - mins.x) * .5f;
	float yy = maxs.y - (maxs.y - mins.y) * .5f;
	float zz = maxs.z - (maxs.z - mins.z) * .5f;

	return Vector3(xx,yy,zz);
}

Vector3 AABB3::GetDimensions() const
{
	float xx = maxs.x - mins.x;
	float yy = maxs.y - mins.y;
	float zz = maxs.z - mins.z;

	return Vector3(xx,yy,zz);
}

AABB3 AABB3::Centered(const Vector3& centerPos, const Vector3& bounds)
{
	AABB3 newAABB3;
	
	newAABB3.mins = centerPos - (bounds * .5f);
	newAABB3.maxs = centerPos + (bounds * .5f);

	return newAABB3;
}
