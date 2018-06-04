#include "IntVector3.hpp"
#include "Engine/Math/Vectors/IntVector2.hpp"
#include "Engine/Math/Vectors/Vector3.hpp"
#include "Engine/Math/MathUtils.hpp"


IntVector3::IntVector3(const IntVector3 & copyFrom)
	: x( copyFrom.x )
	, y( copyFrom.y )
	, z(copyFrom.z)
{
}

IntVector3::IntVector3(int initialX, int initialY, int initialZ)
	: x (initialX)
	, y (initialY)
	, z (initialZ)
{
}

IntVector3::IntVector3(IntVector2 original, int zToAdd)
{
	x = original.x;
	y = original.y;
	z = zToAdd;
}

void IntVector3::SetFromText(const char* text)
{
	Strings result = SplitString(text, ",");

	// Set Values
	x = (int)atof(result.at(0).c_str());
	y = (int)atof(result.at(1).c_str());
	z = (int)atof(result.at(2).c_str());
}

const IntVector3 IntVector3::operator+(const IntVector3 & vecToAdd) const
{
	int xx = x + vecToAdd.x;
	int yy = y + vecToAdd.y;
	int zz = z + vecToAdd.z;
	
	return IntVector3(xx,yy,zz);
}

const IntVector3 IntVector3::operator-(const IntVector3 & vecToSubtract) const
{
	int xx = x - vecToSubtract.x;
	int yy = y - vecToSubtract.y;
	int zz = z - vecToSubtract.z;

	return IntVector3(xx,yy,zz);

}

const IntVector3 IntVector3::operator*(int uniformScale) const
{
	int xx = x * uniformScale;
	int yy = y * uniformScale;
	int zz = z * uniformScale;

	return IntVector3(xx,yy,zz);
}

void IntVector3::operator+=(const IntVector2& vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}

void IntVector3::operator+=(const IntVector3 & vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}

void IntVector3::operator-=(const IntVector3 & vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}

void IntVector3::operator*=(const int uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}

void IntVector3::operator=(const IntVector3 & copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

bool IntVector3::operator==(const IntVector3 & compare) const
{
	if(x == compare.x)
	{
		if(y == compare.y)
		{
			if(z == compare.z)
			{
				return true;
			}
		}
	}
	
	return false;
}

bool IntVector3::operator!=(const IntVector3 & compare) const
{
	return !(*this == compare);
}

void IntVector3::SwapYAndZ()
{
	int temp = y;
	y = z;
	z = temp;
}

Vector3 IntVector3::GetAsVector3() const
{
	return Vector3((float)x, (float)y, (float)z);
}

IntVector2 IntVector3::GetXAndY()
{
	return IntVector2(x,y);
}

IntVector2 IntVector3::GetXAndY(IntVector3 vectorToUse)
{
	return IntVector2(vectorToUse.x,vectorToUse.y);
}

std::string IntVector3::ToString()
{
	std::string result = Stringf("(%i, %i, %i)", x , y, z);

	return result;
}

