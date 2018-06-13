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

	Vector3 corners[8];
	//ndc.GetCorners(corners);

	Matrix44 inverse = Invert(mat);

	Vector3 worldCorners[8];
	for(uint i = 0; i < 8; i++)
	{
		Vector4 ndcPos = Vector4(corners[i], 1);
		//Vector4 KindaWorldPos = TransformHomogeneous(ndcPos, inverse);

		//worldCorners[i] = KindaWorldPos.xyz() / KindaWorldPos.w;
	}

	Frustrum frustrum;
	frustrum.planes[LEFT] = Plane(worldCorners[0], worldCorners[4], worldCorners[2]);

	// Update these
	frustrum.planes[LEFT] = Plane(worldCorners[0], worldCorners[4], worldCorners[2]);
	frustrum.planes[LEFT] = Plane(worldCorners[0], worldCorners[4], worldCorners[2]);
	frustrum.planes[LEFT] = Plane(worldCorners[0], worldCorners[4], worldCorners[2]);
	frustrum.planes[LEFT] = Plane(worldCorners[0], worldCorners[4], worldCorners[2]);
	frustrum.planes[LEFT] = Plane(worldCorners[0], worldCorners[4], worldCorners[2]);

	return frustrum;
}
