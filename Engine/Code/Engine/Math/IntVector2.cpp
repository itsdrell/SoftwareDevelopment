#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector2.hpp"
#include <math.h>
#include "Engine/Core/Utils/StringUtils.hpp"

Vector2 IntVector2::GetAsVector2()
{
	float xx = (float)x;
	float yy = (float)y;

	return Vector2(xx,yy);
}

std::string IntVector2::ToString()
{
	std::string result = Stringf("(%i, %i)", x , y);

	return result;
}

//////////////////////////////////////////////////////////////////////////
// STATIC VARIABLES :D
const IntVector2 IntVector2::NORTH = IntVector2(0,1);
const IntVector2 IntVector2::SOUTH = IntVector2(0,-1);
const IntVector2 IntVector2::EAST = IntVector2(1,0);
const IntVector2 IntVector2::WEST = IntVector2(-1,0);
const IntVector2 IntVector2::ZERO = IntVector2(0,0);
//////////////////////////////////////////////////////////////////////////





//-----------------------------------------------------------------------------------------------
IntVector2::IntVector2( const IntVector2& copy )
	: x( copy.x )
	, y( copy.y )
{
}


//-----------------------------------------------------------------------------------------------
IntVector2::IntVector2( int initialX, int initialY )
	: x( initialX )
	, y( initialY )
{
}

void IntVector2::SetFromText(const char * text)
{
	Strings result = SplitString(text, ",");

	// Set Values
	x = (int)atof(result.at(0).c_str());
	y = (int)atof(result.at(1).c_str());
}


//-----------------------------------------------------------------------------------------------
const IntVector2 IntVector2::operator + ( const IntVector2& vecToAdd ) const
{
	int newX = this->x + vecToAdd.x;
	int newY = this->y + vecToAdd.y;

	return IntVector2( newX, newY );
}


//-----------------------------------------------------------------------------------------------
const IntVector2 IntVector2::operator-( const IntVector2& vecToSubtract ) const
{
	int newX = this->x - vecToSubtract.x;
	int newY = this->y - vecToSubtract.y;

	return IntVector2( newX, newY ); 
}


//-----------------------------------------------------------------------------------------------
const IntVector2 IntVector2::operator*( int uniformScale ) const
{
	int newX = this->x * uniformScale;
	int newY = this->y * uniformScale;

	return IntVector2( newX, newY ); 
}


//-----------------------------------------------------------------------------------------------
void IntVector2::operator+=( const IntVector2& vecToAdd )
{
	this->x = this->x + vecToAdd.x;
	this->y = this->y + vecToAdd.y; 
}


const IntVector2 IntVector2::operator+(const int numberToAdd) 
{
	x += numberToAdd;
	y += numberToAdd;

	return IntVector2(x,y);
}

//-----------------------------------------------------------------------------------------------
void IntVector2::operator-=( const IntVector2& vecToSubtract )
{
	this->x = this->x - vecToSubtract.x; 
	this->y = this->y - vecToSubtract.y; 
}


//-----------------------------------------------------------------------------------------------
void IntVector2::operator*=( const int uniformScale )
{
	this->x = this->x * uniformScale;
	this->y = this->y * uniformScale;
}



//-----------------------------------------------------------------------------------------------
void IntVector2::operator=( const IntVector2& copyFrom )
{
	this->x = copyFrom.x; 
	this->y = copyFrom.y; 
}


//-----------------------------------------------------------------------------------------------
bool IntVector2::operator==( const IntVector2& compare ) const
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
bool IntVector2::operator!=( const IntVector2& compare ) const
{
	// I could just call == and do the same thing but just so it's written out

	if(this->x == compare.x)
	{
		if(this->y == compare.y)
		{
			return false;
		}
	}


	return true; 
}

