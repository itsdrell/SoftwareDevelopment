#include "Vector3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vectors/IntVector3.hpp"
#include "Game/Main/EngineBuildPreferences.hpp"

//////////////////////////////////////////////////////////////////////////
// Static variables
const Vector3 Vector3::ZERO = Vector3(0.f,0.f,0.f);
const Vector3 Vector3::ONE = Vector3(1.f,1.f,1.f);

#ifdef XRIGHT_YUP_ZFORWARD

const Vector3 Vector3::UP = Vector3(0.f,1.f,0.f); // Y is up in my engine (unity)
const Vector3 Vector3::DOWN = Vector3(0.f,-1.f,0.f);
const Vector3 Vector3::RIGHT = Vector3(1.f,0.f,0.f);
const Vector3 Vector3::LEFT = Vector3(-1.f,0.f,0.f);
const Vector3 Vector3::FORWARD = Vector3(0.f,0.f,1.f);
const Vector3 Vector3::BACK = Vector3(0.f,0.f,-1.f);

#endif

#ifdef XFORWARD_YLEFT_ZUP
const Vector3 Vector3::UP =			Vector3(0.f,	0.f,	1.f); 
const Vector3 Vector3::DOWN =		Vector3(0.f,	0.f	,   -1.f);
const Vector3 Vector3::RIGHT =		Vector3(0.f,	-1.f,	0.f);
const Vector3 Vector3::LEFT =		Vector3(0.f,	1.f,	0.f);
const Vector3 Vector3::FORWARD =	Vector3(1.f,	0.f,	0.f);
const Vector3 Vector3::BACK =		Vector3(-1.f,	0.f,	0.f);

#endif
//////////////////////////////////////////////////////////////////////////

Vector3::Vector3(const Vector3& copyfrom)
{
	x = copyfrom.x;
	y = copyfrom.y;
	z = copyfrom.z;
}

Vector3::Vector3(float initialX, float initialY, float initialZ)
{
	x = initialX;
	y = initialY;
	z = initialZ;
}

Vector3::Vector3(const Vector2& copyfrom)
{
	x = copyfrom.x;
	y = copyfrom.y;
	z = 0;
}

Vector3::Vector3(float value)
{
	x = value;
	y = value;
	z = value;
}

Vector3::Vector3(const Vector2& copyFrom, float newZ)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = newZ;
}

float Vector3::GetLength() const
{
	return sqrtf((x*x) + y*y + z*z);
}

Vector3 Vector3::Normalize()
{
	//First we need to find the distance
	float dis = GetLength();

	// todo check to see if length > 0
	if(dis == 0){return Vector3::ZERO;}

	// Then we divide x,y by distance
	float xx = x/dis;
	float yy = y/dis;
	float zz = z/dis;

	return Vector3(xx,yy,zz);
}

Vector3 Vector3::Cross(const Vector3 vectorToCross)
{
	//Vector3 U <x,y,z>
	//Vector3 V <a,b,c>
	//	U x V = <yc - zb, za-xc , xb-ya>

	float xx = (y*vectorToCross.z) - (z * vectorToCross.y);
	float yy = (z * vectorToCross.x) - (x * vectorToCross.z);
	float zz = (x * vectorToCross.y) - (y * vectorToCross.x);

	return Vector3(xx,yy,zz);
}

bool Vector3::IsNearZero()
{
	if(NearlyEqual(x, 0.f) == false) { return false; }
	if(NearlyEqual( y, 0.f ) == false) { return false; }
	if(NearlyEqual(z, 0.f) == false) { return false; }

	return true;
}

void Vector3::SwapXAndZ()
{
	float temp = x;
	x = z;
	z = temp;
}

const Vector3 Vector3::operator+(const Vector3& vecToAdd) const
{
	float xx = x + vecToAdd.x;
	float yy = y + vecToAdd.y;
	float zz = z + vecToAdd.z;

	return Vector3(xx,yy,zz);
}

const Vector3 Vector3::operator-(const Vector3& vecToSubtract) const
{
	float xx = x - vecToSubtract.x;
	float yy = y - vecToSubtract.y;
	float zz = z - vecToSubtract.z;

	return Vector3(xx,yy,zz);
}

bool Vector3::operator<( Vector3 &b )
{
	return (x < b.x) && (y < b.y) && (z < b.z); 
}

bool Vector3::operator<=( Vector3 &b )
{
	return (x <= b.x) && (y <= b.y) && (z <= b.z); 
}

Vector2 Vector3::xy(Vector3 vectorToUse)
{
	return Vector2(vectorToUse.x,vectorToUse.y);
}

Vector2 Vector3::xy() const
{
	return Vector2(x,y);
}

Vector2 Vector3::xz() const
{
	Vector2 result = Vector2(x,z);

	return result;
}

std::string Vector3::ToString() const
{
	std::string result = Stringf("(%g, %g, %g)", x , y, z);

	return result;
}

const Vector3 Vector3::operator*(float uniformScale) const
{
	float xx = x * uniformScale;
	float yy = y * uniformScale;
	float zz = z * uniformScale;

	return Vector3(xx,yy,zz);
}

const Vector3 Vector3::operator/(float inverseScale) const
{
	float scale = 1.0f / inverseScale;
	
	float xx = x * scale;
	float yy = y * scale;
	float zz = z * scale;

	return Vector3(xx,yy,zz);
	
}

void Vector3::operator+=(const Vector3& vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}

void Vector3::operator-=(const Vector3& vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}

void Vector3::operator*=(const float uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}

void Vector3::operator/=(const float uniformDivisor)
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
}

void Vector3::operator=(const Vector3 & copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

bool Vector3::operator==(const Vector3 & compare) const
{
	if(x != compare.x){return false;}
	if(y != compare.y){return false;}
	if(z != compare.z){return false;}
	
	return true;
}

bool Vector3::operator!=(const Vector3 & compare) const
{
	if(x != compare.x){return true;}
	if(y != compare.y){return true;}
	if(z != compare.z){return true;}

	return false;
}

// WITH VEC 2
const Vector3 Vector3::operator+(const Vector2 & vecToAdd) const
{
	float xx = x + vecToAdd.x;
	float yy = y + vecToAdd.y;
	
	return Vector3(xx,yy,0);
}

const Vector3 Vector3::operator-(const Vector2 & vecToSubtract) const
{
	float xx = x - vecToSubtract.x;
	float yy = y - vecToSubtract.y;

	return Vector3(xx,yy,0);
}

void Vector3::operator+=(const Vector2 & vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}

void Vector3::operator-=(const Vector2 & vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}

void Vector3::operator=(const Vector2& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	
	z = 0; // We could later keep the z as it was...?
}

IntVector3 Vector3::GetAsIntVector3() const
{
	IntVector3 result;

	result.x = (int)(floorf(x));
	result.y = (int)(floorf(y));
	result.z = (int)(floorf(z));

	return result;
}


//////////////////////////////////////////////////////////////////////////
// Outside functions 
Vector3 Normalize(const Vector3& vectorToNormalize)
{
	//First we need to find the distance
	float dis = vectorToNormalize.GetLength();

	// todo check to see if length > 0
	if(dis == 0){return Vector3::ZERO;}

	// Then we divide x,y by distance
	float xx = vectorToNormalize.x/dis;
	float yy = vectorToNormalize.y/dis;
	float zz = vectorToNormalize.z/dis;

	return Vector3(xx,yy,zz);
}

Vector3 Cross(const Vector3& a, const Vector3& b)
{
	//Vector3 U <x,y,z>
	//Vector3 V <a,b,c>
	//	U x V = <yc - zb, za-xc , xb-ya>

	float xx = (a.y*b.z) - (a.z * b.y);
	float yy = (a.z * b.x) - (a.x * b.z);
	float zz = (a.x * b.y) - (a.y * b.x);

	return Vector3(xx,yy,zz);
}

Vector3 SwapXAndZ(Vector3& theVector)
{
	float tempx = theVector.x;
	float newX = theVector.z;
	float newZ = tempx;

	return Vector3(newX, theVector.y, newZ);
}

float DotProduct(const Vector3& a, const Vector3& b)
{
	// (vector1.X * vector2.X) + (vector1.Y * vector2.Y) + (vector1.Z * vector2.Z)
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);

}

Vector3 Abs(Vector3& a)
{
	float x = abs(a.x);
	float y = abs(a.y);
	float z = abs(a.z);

	return Vector3(x,y,z);
}

Vector3 Average(const Vector3& a, const Vector3& b)
{
	//  #TODO could later be an initializer list so it could take as many points
	
	Vector3 sum = a + b;
	Vector3 av = sum * .5f;

	return av;
}

Vector3 Slerp(const Vector3 & a, const Vector3 & b, float t)
{
	float al = a.GetLength();
	float bl = b.GetLength();

	float len = Interpolate( al, bl, t );
	Vector3 u = SlerpUnit( a / al, b / bl, t ); 
	return u * len;
}

Vector3 SlerpUnit(const Vector3 & a, const Vector3 & b, float t)
{
	float cosangle = ClampFloat(DotProduct(a, b), -1.0f, 1.0f);
	float angle = acosf(cosangle);
	
	if (angle < EPSILON) 
	{
		return Interpolate( a, b, t );
	} 
	else 
	{
		float pos_num = sinf( t * angle );
		float neg_num = sinf( (1.0f - t) * angle );
		float den = sinf(angle);

		return (a * (neg_num / den)) + (b * (pos_num / den));
	}
}
