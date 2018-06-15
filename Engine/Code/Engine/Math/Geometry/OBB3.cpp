#include "OBB3.hpp"
#include "AABB3.hpp"
#include "..\Matrices\Matrix44.hpp"

bool OBB3::IsContained(const Vector3 & pos)
{
	Matrix44 invert = GetInverseMatrix();
	Vector4 homogenousPos = TransformPoint(Vector4( pos, 1.0f), invert);

	Vector3 local_pos = homogenousPos.xyz(); 
	
	Vector3 compare = Vector3::ONE;
	return Abs(local_pos) <= compare; 
	
}

Matrix44 OBB3::GetMatrix()
{
	return Matrix44(right, up, forward, center);
}

Matrix44 OBB3::GetInverseMatrix()
{
	Matrix44 current = GetMatrix();
	current.Invert();
	return current;
}

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
bool OBB3::GetCorners(Vector3 *out)
{
	AABB3 bounds = AABB3::Centered(Vector3::ZERO, Vector3(2.0f));

	Matrix44 space = Matrix44(right,up,forward,center);

	out[0] = TransformPoint( bounds.GetBackBottomLeftPoint(),  space);
	out[1] = TransformPoint( bounds.GetBackBottomRightPoint(), space);
	out[2] = TransformPoint( bounds.GetBackBottomRightPoint(), space);
	out[3] = TransformPoint( bounds.GetBackBottomRightPoint(), space);
	out[4] = TransformPoint( bounds.GetFrontBottomRightPoint(), space);
	out[5] = TransformPoint( bounds.GetFrontBottomRightPoint(), space);
	out[6] = TransformPoint( bounds.GetFrontBottomRightPoint(), space);
	out[7] = TransformPoint( bounds.GetFrontBottomRightPoint(), space);

	return true;
}
