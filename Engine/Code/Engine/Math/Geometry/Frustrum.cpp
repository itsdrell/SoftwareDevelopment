#include "Frustrum.hpp"
#include "..\..\Core\General\EngineCommon.hpp"
#include "AABB3.hpp"

bool Frustrum::IsContained(const Vector3 & pos) const
{
	for(uint i = 0; i < NUM_OF_FRUSTRUM_SIDES; ++i)
	{
		if(!planes[i].IsInFront(pos))
		{
			return false;
		}
	}

	return true;
	
}

Frustrum Frustrum::FromMatrix(Matrix44& mat)
{
	
	AABB3 ndc = AABB3::Centered( Vector3::ZERO, Vector3(2.0f));

	std::vector<Vector3> corners = ndc.GetCornerPoints();

	Matrix44 inverse = Invert(mat);

	Vector3 worldCorners[8];
	for(uint i = 0; i < 8; i++)
	{
		Vector4 ndcPos = Vector4(corners[i], 1);
		Vector4 KindaWorldPos = TransformPoint(ndcPos, inverse);

		worldCorners[i] = KindaWorldPos.xyz() / KindaWorldPos.w;
	}

	Frustrum frustrum;
	frustrum.planes[LEFT] = Plane(worldCorners[0], worldCorners[4], worldCorners[2]);

	// These are probably wrong 
	frustrum.planes[RIGHT] = Plane(worldCorners[1], worldCorners[5], worldCorners[3]);
	frustrum.planes[BOTTOM] = Plane(worldCorners[0], worldCorners[1], worldCorners[5]);
	frustrum.planes[TOP] = Plane(worldCorners[2], worldCorners[3], worldCorners[7]);
	frustrum.planes[BACK] = Plane(worldCorners[5], worldCorners[4], worldCorners[7]);
	frustrum.planes[FRONT] = Plane(worldCorners[0], worldCorners[1], worldCorners[2]);

	return frustrum;
}

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
