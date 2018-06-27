#include "Engine/Math/Vectors/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Tools/ErrorWarningAssert.hpp"
#include "Engine/Core/Utils/StringUtils.hpp"
#include "Engine/Math/Vectors/IntVector2.hpp"
#include <math.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

//////////////////////////////////////////////////////////////////////////
// STATIC VARIABLES :D
const Vector2 Vector2::NORTH =	Vector2(0.f,1.f);
const Vector2 Vector2::SOUTH =	Vector2(0.f,-1.f);
const Vector2 Vector2::EAST =	Vector2(1.f,0.f);
const Vector2 Vector2::WEST =	Vector2(-1.f,0.f);
const Vector2 Vector2::ZERO =	Vector2(0.f,0.f);
const Vector2 Vector2::ONE =	Vector2(1.f,1.f);
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------------
Vector2::Vector2( const Vector2& copy )
   	: x( copy.x )
	, y( copy.y )
{
}


//-----------------------------------------------------------------------------------------------
Vector2::Vector2( float initialX, float initialY )
	: x( initialX )
	, y( initialY )
{
}

Vector2::Vector2(float valueForBoth)
{
	x = valueForBoth;
	y = valueForBoth;
}

void Vector2::SetFromText(const char * text)
{
	Strings result = SplitString(text,",");
	
	x = (float)atof(result.at(0).c_str());
	y = (float)atof(result.at(1).c_str());
}


//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator + ( const Vector2& vecToAdd ) const
{
	float newX = this->x + vecToAdd.x;
	float newY = this->y + vecToAdd.y;
	
	return Vector2( newX, newY );
}


//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator-( const Vector2& vecToSubtract ) const
{
	float newX = this->x - vecToSubtract.x;
	float newY = this->y - vecToSubtract.y;
	
	return Vector2( newX, newY ); 
}


//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator*( float uniformScale ) const
{
	float newX = this->x * uniformScale;
	float newY = this->y * uniformScale;
	
	return Vector2( newX, newY ); 
}


//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator/( float inverseScale ) const
{
	float scale = 1.0f / inverseScale;
	
// 	float newX = this->x / inverseScale;
// 	float newY = this->y / inverseScale;

	float newX = this->x * scale;
	float newY = this->y * scale;
	
	return Vector2( newX, newY ); 
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator+=( const Vector2& vecToAdd )
{
	this->x = this->x + vecToAdd.x;
	this->y = this->y + vecToAdd.y; 
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator-=( const Vector2& vecToSubtract )
{
	this->x = this->x - vecToSubtract.x; 
	this->y = this->y - vecToSubtract.y; 
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator*=( const float uniformScale )
{
	this->x = this->x * uniformScale;
	this->y = this->y * uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator/=( const float uniformDivisor )
{
	// if its a constant its as cheap just to divide by it...?
	
	this->x = this->x / uniformDivisor;
	this->y = this->y / uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator=( const Vector2& copyFrom )
{
	this->x = copyFrom.x; 
	this->y = copyFrom.y; 
}


//-----------------------------------------------------------------------------------------------
const Vector2 operator*( float uniformScale, const Vector2& vecToScale )
{
	float scale = uniformScale;

	float newX = vecToScale.x * scale;
	float newY = vecToScale.y * scale;
	
	return Vector2( newX, newY );
}


//-----------------------------------------------------------------------------------------------
bool Vector2::operator==( const Vector2& compare ) const
{
	
	if(this->x == compare.x)
	{
		if(this->y == compare.y)
		{
			return true;
		}
	}
	
	
	return false; 
}


//-----------------------------------------------------------------------------------------------
bool Vector2::operator!=( const Vector2& compare ) const
{
	// I could just call == and do the same thing but just so it's written out

	if(this->x == compare.x)
	{
		if(this->y == compare.y)
		{
			return false;
		}
	}
	
	
	return true; // #MP1Fixme
}


//-------------------------------------------------------------------------------------------------
// Mutators / Inque

float Vector2::GetLength() const
{
	float answer = sqrtf((x*x) + y*y);
	return answer;
}

float Vector2::GetLengthSquared() const
{
	float answer = GetLength() *GetLength();
	return answer;
}

float Vector2::NormalizeAndGetLength()
{
	float theLength = GetLength();

	x /= theLength;
	y /= theLength;
	

	return theLength;
}

Vector2 Vector2::GetNormalized() const
{
	//First we need to find the distance
	float dis = GetLength();

	// todo check to see if length > 0
	if(dis == 0){return Vector2(0.f,0.f);}

	// Then we divide x,y by distance
	float xx = x/dis;
	float yy = y/dis;
	
	Vector2 answer = Vector2(xx,yy);
	return answer;
}

float Vector2::GetOrientationDegrees() const
{
	
	float answer = ConvertRadiansToDegrees(atan2f(y,x)); // might want to add this to math library as Atan2Degrees(y,x)
	return answer;
}

// This is static...?
Vector2 Vector2::MakeDirectionAtDegrees(float degrees, float length)
{
	Vector2 answer = length * Vector2(CosDegrees(degrees),SinDegrees(degrees));
	return answer;
}

IntVector2 Vector2::GetVector2AsInt() const
{
	return IntVector2((int)x, (int)y);
}

std::string Vector2::ToString()
{
	std::string result = Stringf("(%g, %g)", x , y);

	return result;
}

Vector2 Vector2::GetMidPoint(const Vector2& a, const Vector2& b)
{
	return Vector2(((a.x + b.x) * .5f),((a.y + b.y) * .5f));
}

Vector2 Vector2::RangeMap(const Vector2& inValue, const Vector2& inStart, const Vector2& inEnd, const Vector2& outStart, const Vector2& outEnd)
{
	float newX = RangeMapFloat(inValue.x, inStart.x, inEnd.x, outStart.x, outEnd.x);
	float newY = RangeMapFloat(inValue.y, inStart.y, inEnd.y, outStart.y, outEnd.y);

	return Vector2(newX, newY);
}

