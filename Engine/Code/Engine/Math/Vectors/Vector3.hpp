#pragma once
#include "Engine/Math/Vectors/Vector2.hpp"
#include "Engine/Math/Vectors/IntVector3.hpp"

class Vector3
{
	
public:
	~Vector3(){}
	Vector3(){}
	Vector3(const Vector3& copyfrom);
	Vector3(const Vector2& copyfrom); // Create from a vector2 !
	Vector3(const Vector2& copyFrom, float newZ);
	explicit Vector3(float initialX, float initialY, float initialZ);
	Vector3(float value);

	// Math
	float GetLength() const;
	Vector3 Normalize();
	Vector3 Cross(const Vector3 vectorToCross);
	bool IsNearZero();
	
	void SwapXAndZ();
	Vector2 GetXAndY() const;
	Vector2 xz() const;
	
	std::string ToString();

	// Vector3 specific
	const Vector3 operator+( const Vector3& vecToAdd ) const;		// vec3 + vec3
	const Vector3 operator-( const Vector3& vecToSubtract ) const;	// vec3 - vec3
	const Vector3 operator*( float uniformScale ) const;			// vec3 * float
	const Vector3 operator/( float inverseScale ) const;			// vec3 / float
	void operator+=( const Vector3& vecToAdd );						// vec3 += vec3
	void operator-=( const Vector3& vecToSubtract );				// vec3 -= vec3
	void operator*=( const float uniformScale );					// vec3 *= float
	void operator/=( const float uniformDivisor );					// vec3 /= float
	void operator=( const Vector3& copyFrom );						// vec3 = vec3
	bool operator==( const Vector3& compare ) const;				// vec3 == vec3
	bool operator!=( const Vector3& compare ) const;				// vec3 != vec3

	Vector3 operator-() const { return Vector3(-x,-y,-z); }

	// Vector3 with vector 2
	const Vector3 operator+( const Vector2& vecToAdd ) const;		// vec3 + vec2
	const Vector3 operator-( const Vector2& vecToSubtract ) const;	// vec3 - vec2
	void operator+=( const Vector2& vecToAdd );						// vec3 += vec2
	void operator-=( const Vector2& vecToSubtract );				// vec3 -= vec2
	void operator=( const Vector2& copyFrom );						// vec3 = vec3


	// Helper
	static Vector2 GetXAndY(Vector3 vectorToUse);
	IntVector3 GetAsIntVector3() const;

public:
	float		x;
	float		y;
	float		z;


	const static Vector3 ZERO;
	const static Vector3 ONE;
	const static Vector3 UP;
	const static Vector3 DOWN;
	const static Vector3 RIGHT;
	const static Vector3 LEFT;
	const static Vector3 FORWARD;
	const static Vector3 BACK;
};


// outside methods
Vector3 Normalize(Vector3& vectorToNormalize);
Vector3 Cross(const Vector3& a, const Vector3& b);
Vector3 SwapXAndZ(Vector3& theVector);
float DotProduct(const Vector3& a, const Vector3& b);