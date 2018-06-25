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

bool AABB3::IsPointInside( Vector3 & pos)
{	
	return !(pos <= mins) && (pos <= maxs);
}

void AABB3::GrowToContain( Vector3& pos)
{
	if(IsPointInside(pos) == false)
	{
		// Total length needed to add padding to get the point inside
		// Expand, but not move

		if(pos.x > maxs.x) { maxs.x += (pos.x - maxs.x); }
		if(pos.x < mins.x) { mins.x -= (mins.x - pos.x); }
		   	 						 
		if(pos.y > maxs.y) { maxs.y += (pos.y - maxs.y); }
		if(pos.y < mins.y) { mins.y -= (mins.y - pos.y); }
		   	 						 
		if(pos.z > maxs.z) { maxs.z += (pos.z - maxs.z); }
		if(pos.z < mins.z) { mins.z -= (mins.z - pos.z); }

	}
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

void AABB3::Translate(const Matrix44& byMatrix)
{
	Matrix44 theMatrix = byMatrix;
	mins = TransformPoint(mins, theMatrix);
	maxs = TransformPoint(maxs, theMatrix);
}

std::vector<Vector3> AABB3::GetCornerPoints() const
{
	//--------------------------------------------------------------------------
	//	Points
	//		   6--------------7
	//		  /|             /|
	//		 / |            / |
	//		2--+-----------3  |
	//		|  |           |  |
	//		|  |     c     |  |
	//		|  |           |  |
	//		|  4-----------+--5
	//		| /            | /
	//		|/             |/
	//		0--------------1
	//
	// 0 being the mins, 7 the maxs
	//--------------------------------------------------------------------------

	std::vector<Vector3>	points;

	points.push_back( mins ); 
	points.push_back(	GetFrontBottomRightPoint()	);
	points.push_back(	GetFrontTopLeftPoint()		);
	points.push_back(	GetFrontTopRightPoint()		);
	points.push_back(	GetBackBottomLeftPoint()	);
	points.push_back(	GetBackBottomRightPoint()	);
	points.push_back(	GetBackTopLeftPoint()		);
	points.push_back( maxs );

	return points;

}

AABB3 AABB3::Centered(const Vector3& centerPos, const Vector3& bounds)
{
	AABB3 newAABB3;
	
	newAABB3.mins = centerPos - (bounds * .5f);
	newAABB3.maxs = centerPos + (bounds * .5f);

	return newAABB3;
}
