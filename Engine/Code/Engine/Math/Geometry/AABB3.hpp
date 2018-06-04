#pragma once
#include "Engine/Math/Vectors/Vector3.hpp"


class AABB3
{
public:
	AABB3() {}
	AABB3(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax);
	AABB3(const Vector3& mins, const Vector3& maxs);

	Vector3 GetCenter() const;
	Vector3 GetDimensions() const;


public:
	Vector3 mins;
	Vector3 maxs;
};