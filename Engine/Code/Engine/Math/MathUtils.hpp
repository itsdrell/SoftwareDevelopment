#pragma once
#include "Engine/Core/Tools/ErrorWarningAssert.hpp"
#include "Engine/Core/Utils/StringUtils.hpp"
#include <initializer_list>

class Vector2;
class AABB2;
class Disc2;
class IntRange;
class FloatRange;
class IntVector2;
class Rgba;


#define PI 3.14159265359f

// Math heavy
float ConvertRadiansToDegrees(float radians);
float ConvertDegreesToRadians(float degrees);
float CosDegrees(float degrees);
float SinDegrees(float degrees);


// Polar
Vector2 PolarToCartesian(float radius, float degrees);
Vector3 PolarToCartesian( float radius, float rotation, float azimuth ); 
Vector3 PolarToCartesian( Vector3 spherical ); 
Vector3 CartesianToPolar( Vector3 position );


float GetRandomFloat(float min, float max);
int GetRandomIntRange(int minInclusive, int maxInclusive);
float GetRandomFloatZeroToOne();
int GetRandomIntLessThan(int maxNotInclusive);
Vector3 GetRandomDirection();

// Vector
// Gets the projected vector in the “projectOnto” direction, whose magnitude is the projected length of “vectorToProject” in that direction.
const Vector2 GetProjectedVector( const Vector2& vectorToProject, const Vector2& projectOnto );

// Returns the vector’s representation/coordinates in (i,j) space (instead of its original x,y space)
const Vector2 GetTransformedIntoBasis( const Vector2& originalVector,
	const Vector2& newBasisI, const Vector2& newBasisJ );

// Takes “vectorInBasis” in (i,j) space and returns the equivalent vector in [axis-aligned] (x,y) Cartesian space
const Vector2 GetTransformedOutOfBasis( const Vector2& vectorInBasis,
	const Vector2& oldBasisI, const Vector2& oldBasisJ );

// Decomposes “originalVector” into two component vectors, which add up to the original:
//   “vectorAlongI” is the vector portion in the “newBasisI” direction, and
//   “vectorAlongJ” is the vector portion in the “newBasisJ” direction.
void DecomposeVectorIntoBasis( const Vector2& originalVector,
	const Vector2& newBasisI, const Vector2& newBasisJ,
	Vector2& out_vectorAlongI, Vector2& out_vectorAlongJ );

Vector2 Reflect(const Vector2& incoming, const Vector2& surfaceNormal);

float GetDistance(Vector2 firstVector, Vector2 secondVector);
float GetDistance(Vector3& firstVector, Vector3& secondVector);
float GetDistanceSquared(Vector2 firstVector, Vector2 secondVector);

void GenerateArbitraryTangents(Vector3* tangent, Vector3* bitangent, const Vector3& normal);

// Circles and discs
bool DoAABBsOverlap( const AABB2& a, const AABB2& b );
bool DoDiscsOverlap( const Disc2& a, const Disc2& b );
bool DoDiscsOverlap( const Vector2& aCenter, float aRadius, const Vector2& bCenter, float bRadius );

// Corrective physics
Vector2 CorrectiveDiscVsDot(const Disc2& nonSolid, const Vector2& solid);
Vector2 CorrectiveDiscVsDisc(const Disc2& nonSolid, const Disc2& solid);
Vector2 CorrectiveDiscVsBox(const Disc2& nonSolid, const AABB2& solid);


bool CheckRandomChance( float chanceForSuccess );	// If 0.27f passed, returns true 27% of the time

// Clamping
float RangeMapFloat(float inValue, float inStart, float inEnd, float outStart, float outEnd);
float ClampFloat(float currentValue, float min, float max);
int ClampInt( int inValue, int min, int max );
float ClampFloatZeroToOne( float inValue );
float ClampFloatNegativeOneToOne( float inValue );
int RoundToNearestInt( float inValue );			// 0.5 rounds up to 1; -0.5 rounds up to 0


float GetFractionInRange( float inValue, float rangeStart, float rangeEnd );
float GetFractionOf(float value);

// Interpolate + Easing

// Finds the value at a certain % (fraction) in [rangeStart,rangeEnd].
// For example, at 75% (0.75) of the way through [2,6] is 5.
float Interpolate( float start, float end, float fractionTowardEnd );

const Vector2 Interpolate( const Vector2& start, const Vector2& end, float fractionTowardEnd );
Vector3 Interpolate( const Vector3& start, const Vector3& end, float fractionTowardEnd );
const FloatRange Interpolate( const FloatRange& start, const FloatRange& end, float fractionTowardEnd );
const AABB2 Interpolate( const AABB2& start, const AABB2& end, float fractionTowardEnd );
const Disc2 Interpolate( const Disc2& start, const Disc2& end, float fractionTowardEnd );

int Interpolate( int start, int end, float fractionTowardEnd );
unsigned char Interpolate( unsigned char start, unsigned char end, float fractionTowardEnd );
const IntVector2 Interpolate( const IntVector2& start, const IntVector2& end, float fractionTowardEnd );
const IntRange Interpolate( const IntRange& start, const IntRange& end, float fractionTowardEnd );
const Rgba Interpolate( const Rgba& start, const Rgba& end, float fractionTowardEnd );

float	Crossfade(float (*EaseFunctionA)(float), float (*EaseFunctionB)(float),float t);
float	SmoothStart2( float t ); // 2nd-degree smooth start (a.k.a. “quadratic ease in”)
float	SmoothStart3( float t ); // 3rd-degree smooth start (a.k.a. “cubic ease in”)
float	SmoothStart4( float t ); // 4th-degree smooth start (a.k.a. “quartic ease in”)
float	SmoothStop2( float t ); // 2nd-degree smooth start (a.k.a. “quadratic ease out”)
float	SmoothStop3( float t ); // 3rd-degree smooth start (a.k.a. “cubic ease out”)
float	SmoothStop4( float t ); // 4th-degree smooth start (a.k.a. “quartic ease out”)
float	SmoothStep3( float t ); // 3rd-degree smooth start/stop (a.k.a. “smoothstep”)


// Finds the “angular displacement” (or signed angular distance) from startDegrees to endDegrees.
// This function always takes the smartest/shortest way around, so going from 5 to 355 yields -10.
float GetAngularDisplacement( float startDegrees, float endDegrees );

// I face currentDegrees and want to turn “toward” goalDegrees, by up to maxTurnDegrees.
// Note: must always take the shortest/smartest way around, and will never overshoot.
float TurnToward( float currentDegrees, float goalDegrees, float maxTurnDegrees );

float DotProduct( const Vector2& a, const Vector2& b );


Vector3 GetWorldPositionFromACamerasDirections(const Vector3& currentPosition, const Vector3& cameraUp, const Vector3& cameraRight, 
	const Vector3 cameraForward = Vector3(0.f,0.f,1.f));

// Bit wise stuff
bool AreBitsSet( unsigned char bitFlags8, unsigned char flagsToCheck );
bool AreBitsSet( unsigned int bitFlags32, unsigned int flagsToCheck );
void SetBits( unsigned char& bitFlags8, unsigned char flagsToSet );
void SetBits( unsigned int& bitFlags32, unsigned int flagsToSet );
void ClearBits( unsigned char& bitFlags8, unsigned char flagToClear );
void ClearBits( unsigned int& bitFlags32, unsigned int flagToClear );

bool NearlyEqual(float a, float b, float tolerance = .00001f);
bool FloatEqual(float a, float b);
bool IsNearZero(float a);


//////////////////////////////////////////////////////////////////////////
// QUADRATIC

bool Quadratic( Vector2* out, float a, float b, float c );

//////////////////////////////////////////////////////////////////////////
// TEMPLATES

// Ben Porters Choose function use as ({a,b,c})
template <typename T>
T choose(std::initializer_list<T> values)
{
	return *(values.begin() + GetRandomIntRange(0, (int) values.size()-1));
}

// This only works if T has the < operator
template <typename T>
bool isGreaterThan(std::initializer_list<T> values){
	
	for(int i = 1; i < values.size(); i++)
	{				
		if(*values.begin() < *(values.begin() + i))
			return false;
	}
	return true;
}

// This only works if T has the > operator
template <typename T>
bool isLessThan(std::initializer_list<T> values){

	for(int i = 1; i < values.size(); i++)
	{				
		if(*values.begin() > *(values.begin() + i))
			return false;
	}
	return true;
}

template <typename T>
T Max(T a, T b)
{
	if(b > a)
		return b;
	else
		return a;
}

template <typename T>
T Min(T a, T b)
{
	if(b < a)
		return b;
	else
		return a;
}