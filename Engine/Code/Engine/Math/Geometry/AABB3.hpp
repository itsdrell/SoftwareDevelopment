#pragma once
#include "Engine/Math/Vectors/Vector3.hpp"

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
	bool IsContained(const Vector3& pos);


	Vector3 GetCenter() const;
	Vector3 GetDimensions() const;



	// Makers
	static AABB3 Centered(const Vector3& centerPos, const Vector3& bounds);


public:
	Vector3 mins;
	Vector3 maxs;
};