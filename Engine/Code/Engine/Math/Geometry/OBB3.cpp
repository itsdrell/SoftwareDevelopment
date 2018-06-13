#include "OBB3.hpp"
#include "AABB3.hpp"
#include "..\Matrices\Matrix44.hpp"

bool OBB3::IsContained(const Vector3 & pos)
{
	
	
	return false;
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

	// Make a matrix class that takes a Vec
	//out[0] = TransformPoint( bounds.GetBackBottomLeft(),  space);
	//out[1] = TransformPoint( bounds.GetBackBottomRight(), space);
	//out[2] = TransformPoint( bounds.GetBackBottomRight(), space);
	//out[3] = TransformPoint( bounds.GetBackBottomRight(), space);
	//out[4] = TransformPoint( bounds.GetBackBottomRight(), space);
	//out[5] = TransformPoint( bounds.GetBackBottomRight(), space);
	//out[6] = TransformPoint( bounds.GetBackBottomRight(), space);
	//out[7] = TransformPoint( bounds.GetBackBottomRight(), space);

	return false;
}
