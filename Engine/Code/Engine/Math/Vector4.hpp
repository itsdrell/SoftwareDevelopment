#pragma once
#include <string>
#include "Vector3.hpp"

class Vector4
{
	
public:
	
	Vector4() {}
	Vector4(float xx, float yy, float zz, float ww);
	Vector4(const Vector3& xyz, float theW);

	void SetFromText(std::string text);
	std::string ToString();

	Vector3 xyz() const;

	

public:
	
	float x;
	float y;
	float z; 
	float w;
};


float DotProduct(const Vector4& a, const Vector4& b);