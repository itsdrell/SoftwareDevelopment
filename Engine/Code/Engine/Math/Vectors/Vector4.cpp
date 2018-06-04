#include "Engine/Math/Vectors/Vector4.hpp"
#include "Engine\Core\General/EngineCommon.hpp"
#include "Engine/Core/Utils/StringUtils.hpp"

TODO("Make this and some utilities pls ");

Vector4::Vector4(float xx, float yy, float zz, float ww)
{
	this->x = xx;
	this->y = yy; 
	this->z = zz;
	this->w = ww;
}

Vector4::Vector4(const Vector3& xyz, float theW)
{
	x = xyz.x;
	y = xyz.y;
	z = xyz.z;

	w = theW;
}

void Vector4::SetFromText(std::string text)
{
	Strings result = SplitString(text,",");

	x = (float)atof(result.at(0).c_str());
	y = (float)atof(result.at(1).c_str());
	z = (float)atof(result.at(2).c_str());
	w = (float)atof(result.at(3).c_str());
}

std::string Vector4::ToString()
{
	std::string result = Stringf("(%g, %g, %g, %g)", x , y, z, w);

	return result;
}

Vector3 Vector4::xyz() const
{
	return Vector3(x,y,z);
}

float DotProduct(const Vector4& a, const Vector4& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}
