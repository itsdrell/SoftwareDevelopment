#pragma once
#include "Engine/Math/Vectors/Vector3.hpp"
#include <vector>



//====================================================================================
class AABB3
{
public:

	AABB3()
	:	mins(INFINITY),
		maxs (-INFINITY)
	{}


	AABB3(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax);
	AABB3(const Vector3& mins, const Vector3& maxs);

	bool IsValid() const;
	void Invalidate();
	bool IsPointInside(Vector3& pos);

	void GrowToContain( Vector3& pos);


	Vector3 GetCenter() const;
	Vector3 GetDimensions() const;



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
	std::vector<Vector3> GetCornerPoints() const;
	
	Vector3 GetFrontBottomLeftPoint()	const { return GetCornerPoints().at(0); }
	Vector3 GetFrontBottomRightPoint()	const { return GetCornerPoints().at(1); }
	Vector3 GetFrontTopLeftPoint()		const { return GetCornerPoints().at(2); }
	Vector3 GetFrontTopRightPoint()		const { return GetCornerPoints().at(3); }
	Vector3 GetBackBottomLeftPoint()	const { return GetCornerPoints().at(4); }
	Vector3 GetBackBottomRightPoint()	const { return GetCornerPoints().at(5); }
	Vector3 GetBackTopLeftPoint()		const { return GetCornerPoints().at(6); }
	Vector3 GetBackTopRightPoint()		const { return GetCornerPoints().at(7); }


	// Makers
	static AABB3 Centered(const Vector3& centerPos, const Vector3& bounds);


public:
	Vector3 mins;
	Vector3 maxs;
};

