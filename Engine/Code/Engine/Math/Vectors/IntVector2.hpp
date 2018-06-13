#pragma once
#include <string>


class Vector2;

class IntVector2
{
public:
	~IntVector2() {}											// destructor: do nothing (for speed)
	IntVector2() {}											// default constructor: do nothing (for speed)
	IntVector2( const IntVector2& copyFrom );						// copy constructor (from another vec2)
	explicit IntVector2( int initialX, int initialY );		// explicit constructor (from x, y)
	IntVector2( int xy );
	void SetFromText( const char* text ); // XML

	// Operators
	const IntVector2 operator+( const IntVector2& vecToAdd ) const;
	const IntVector2 operator+(const int numberToAdd);
	const IntVector2 operator-( const IntVector2& vecToSubtract ) const;	
	const IntVector2 operator*( int uniformScale ) const;			
	void operator+=( const IntVector2& vecToAdd );						
	void operator-=( const IntVector2& vecToSubtract );				
	void operator*=( const int uniformScale );					
	void operator=( const IntVector2& copyFrom );						
	bool operator==( const IntVector2& compare ) const;				
	bool operator!=( const IntVector2& compare ) const;				

	Vector2 GetAsVector2();

	std::string ToString();

public:
	int x;
	int y;


	// DIRECTION NORMALS
	const static IntVector2 NORTH; 
	const static IntVector2 SOUTH;
	const static IntVector2 EAST;
	const static IntVector2 WEST;
	const static IntVector2 ZERO;
	
};