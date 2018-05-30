#pragma once
#include <string>


class IntVector2;

//-----------------------------------------------------------------------------------------------
class Vector2
{
public:
	// Construction/Destruction
	~Vector2() {}											// destructor: do nothing (for speed)
	Vector2() {}											// default constructor: do nothing (for speed)
	Vector2( const Vector2& copyFrom );						// copy constructor (from another vec2)
	explicit Vector2( float initialX, float initialY );		// explicit constructor (from x, y)
	void SetFromText( const char* text ); // XML

	// Mutators
	float GetLength() const;
	float GetLengthSquared() const; // faster than GetLength() since it skips the sqrtf()
	float NormalizeAndGetLength(); // set my new length to 1.0f; keep my direction
	Vector2 GetNormalized() const; // return a new vector, which is a normalized copy of me
	float GetOrientationDegrees() const; // return 0 for east (5,0), 90 for north (0,8), etc.
	static Vector2 MakeDirectionAtDegrees( float degrees, float length = 1.0f ); // create vector at angle
	IntVector2 GetVector2AsInt();

	std::string ToString();

	// helper
	static Vector2 GetMidPoint(const Vector2& a, const Vector2& b);
	
	// Operators
	const Vector2 operator+( const Vector2& vecToAdd ) const;		// vec2 + vec2
	const Vector2 operator-( const Vector2& vecToSubtract ) const;	// vec2 - vec2
	const Vector2 operator*( float uniformScale ) const;			// vec2 * float
	const Vector2 operator/( float inverseScale ) const;			// vec2 / float
	void operator+=( const Vector2& vecToAdd );						// vec2 += vec2
	void operator-=( const Vector2& vecToSubtract );				// vec2 -= vec2
	void operator*=( const float uniformScale );					// vec2 *= float
	void operator/=( const float uniformDivisor );					// vec2 /= float
	void operator=( const Vector2& copyFrom );						// vec2 = vec2
	bool operator==( const Vector2& compare ) const;				// vec2 == vec2
	bool operator!=( const Vector2& compare ) const;				// vec2 != vec2

	Vector2 operator-() const { return Vector2(-x,-y); }


	friend const Vector2 operator*( float uniformScale, const Vector2& vecToScale );	// float * vec2

public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x;
	float y;
	

	// DIRECTION NORMALS
	const static Vector2 NORTH; 
	const static Vector2 SOUTH;
	const static Vector2 EAST;
	const static Vector2 WEST;
	const static Vector2 ZERO;
	const static Vector2 ONE;
};


