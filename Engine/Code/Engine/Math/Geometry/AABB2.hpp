#pragma once
#include "Engine/Math/Vectors/Vector2.hpp"

class AABB2
{
public:

	// Constructors
	~AABB2();
	AABB2();
	AABB2(const AABB2& copy);
	explicit AABB2( float minX, float minY, float maxX, float maxY );
	explicit AABB2( const Vector2& mins, const Vector2& maxs );
	explicit AABB2( const Vector2& center, float radiusX, float radiusY );
	explicit AABB2( const float theMins, const float theMaxs);

	// Mutators
	void StretchToIncludePoint( float x, float y ); // note: stretch, not move…
	void StretchToIncludePoint( const Vector2& point ); // note: stretch, not move…
	void AddPaddingToSides( float xPaddingRadius, float yPaddingRadius );
	void Translate( const Vector2& translation ); // move the box; similar to +=
	void Translate( float translationX, float translationY );
	void SetFromText(const char* text);

	// Accessors/Queries
	bool IsPointInside( float x, float y ) const; // is “x,y” within box’s interior?
	bool IsPointInside( const Vector2& point ) const; // is “point” within box’s interior?
	Vector2 GetDimensions() const; // return a Vector2 of ( width, height )
	Vector2 GetCenter() const; // return the center position of the box
	float GetWidth() const;
	float GetHeight() const;

	// Operators
	void operator+=( const Vector2& translation ); // move (translate) the box
	void operator-=( const Vector2& antiTranslation );
	AABB2 operator+( const Vector2& translation ) const; // create a (temp) moved box
	AABB2 operator-( const Vector2& antiTranslation ) const;
	bool operator==( const AABB2& compare ) const;

	std::string ToString() const;


public:
	Vector2 mins;
	Vector2 maxs;
};