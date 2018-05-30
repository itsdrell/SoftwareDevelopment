#pragma once
#include <string>

//////////////////////////////////////////////////////////////////////////
class Vector3;
class IntVector2;

//////////////////////////////////////////////////////////////////////////
class IntVector3
{
public:
	~IntVector3() {}											// destructor: do nothing (for speed)
	IntVector3() {}											// default constructor: do nothing (for speed)
	IntVector3( const IntVector3& copyFrom );						// copy constructor (from another vec2)
	explicit IntVector3( int initialX, int initialY , int initialZ);		// explicit constructor (from x, y)
	IntVector3(IntVector2 original, int zToAdd);
	void SetFromText( const char* text ); // XML

										  // Operators
	const IntVector3 operator+( const IntVector3& vecToAdd ) const;		
	const IntVector3 operator-( const IntVector3& vecToSubtract ) const;	
	const IntVector3 operator*( int uniformScale ) const;
	void operator+=(const IntVector2& vecToAdd);
	void operator+=( const IntVector3& vecToAdd );						
	void operator-=( const IntVector3& vecToSubtract );				
	void operator*=( const int uniformScale );					
	void operator=( const IntVector3& copyFrom );						
	bool operator==( const IntVector3& compare ) const;				
	bool operator!=( const IntVector3& compare ) const;				

	void SwapYAndZ(); // for map to world
	Vector3 GetAsVector3() const;
	IntVector2 GetXAndY();

	std::string ToString();

	static IntVector2 GetXAndY(IntVector3 vectorToUse); // in case we are adding two

public:
	int x;
	int y;
	int z;


};