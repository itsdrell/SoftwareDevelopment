#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/General/Rgba.hpp"
#include <math.h>
#include <stdlib.h> 



float RangeMapFloat(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
	if(inStart == inEnd)
	{
		return (outStart + outEnd) * .5f;
	}

	// Translate
	float inRange = inEnd - inStart;
	float outRange = outEnd - outStart;
	float inRelativeToStart = inValue - inStart;
	// Scale
	float fractionIntoRange = inRelativeToStart / inRange;
	float outRelativeToStart = fractionIntoRange * outRange;
	// Translate
	return outRelativeToStart + outStart;


}

float ClampFloat(float currentValue, float min, float max)
{
	if(currentValue < min)
		return min;
	if(currentValue > max)
		return max;

	return currentValue;
}

int ClampInt(int inValue, int min, int max)
{
	if(inValue < min)
		return min;
	if(inValue > max)
		return max;
	return inValue;
}

float ClampFloatZeroToOne(float inValue)
{
	if(inValue < 0.f)
		return 0.f;
	if(inValue > 1.f)
		return 1.f;
	return inValue;
}

float ClampFloatNegativeOneToOne(float inValue)
{
	if(inValue < -1.f)
		return -1.f;
	if(inValue > 1.f)
		return 1.f;
	return inValue;
}

int RoundToNearestInt(float inValue)
{
	// This creates a inValue.5 so that we can compare it to the middle
	// To determine if we need to go up or down :) 
	float flooredVersion = floorf(inValue);
	float test = flooredVersion + .5f;

	// Check
	if(inValue < test)
		return (int)flooredVersion;
	else
		return (int)(flooredVersion + 1.f);
	
}

float GetFractionInRange(float inValue, float rangeStart, float rangeEnd)
{
	float answer = RangeMapFloat(inValue,rangeStart,rangeEnd,0,1);
	return answer;
}

float GetFractionOf(float value)
{
	double param, fractPart, intPart;
	
	// This gives us a float value as well, but we don't actually use it
	// http://www.cplusplus.com/reference/cmath/modf/
	param = (double) value;
	fractPart = modf(param, &intPart);

	return (float)fractPart;
}

float Interpolate(float start, float end, float fractionTowardEnd)
{
	float answer = start * (1- fractionTowardEnd) + (fractionTowardEnd * end);
	return answer;
}

const Vector2 Interpolate(const Vector2 & start, const Vector2 & end, float fractionTowardEnd)
{
	float x = Interpolate(start.x , end.x, fractionTowardEnd);
	float y = Interpolate(start.y, end.y, fractionTowardEnd);
	return Vector2(x,y);
}

const FloatRange Interpolate(const FloatRange & start, const FloatRange & end, float fractionTowardEnd)
{
	float x = Interpolate(start.min , end.min, fractionTowardEnd);
	float y = Interpolate(start.max, end.max, fractionTowardEnd);
	return FloatRange(x,y);
}

const AABB2 Interpolate(const AABB2 & start, const AABB2 & end, float fractionTowardEnd)
{
	Vector2 mins = Interpolate(start.mins, end.mins, fractionTowardEnd);
	Vector2 maxs = Interpolate(start.maxs, end.maxs, fractionTowardEnd);
	
	return AABB2(mins,maxs);
}

const Disc2 Interpolate(const Disc2 & start, const Disc2 & end, float fractionTowardEnd)
{
	float radius = Interpolate(start.radius, end.radius,fractionTowardEnd);
	Vector2 pos = Interpolate(start.center,end.center,fractionTowardEnd);
	
	return Disc2(pos,radius);
}

int Interpolate(int start, int end, float fractionTowardEnd)
{
	float answer = ((end - start) *fractionTowardEnd + start);

	int floored = (int)(answer);

	if((answer - floored) >= .5f)
	{
		return (int)floored+1; // round up
	}
	else
	{
		return floored; // round down
	}

}

unsigned char Interpolate(unsigned char start, unsigned char end, float fractionTowardEnd)
{
	// Clamp start
	int a = ClampInt((int)(start),0,255);
	int b = ClampInt((int)(end),0,255);
	
	return (unsigned char)Interpolate(a,b,fractionTowardEnd);
}

const IntVector2 Interpolate(const IntVector2 & start, const IntVector2 & end, float fractionTowardEnd)
{
	int x = Interpolate(start.x,end.x,fractionTowardEnd);
	int y = Interpolate(start.y,end.y,fractionTowardEnd);
	
	return IntVector2(x,y);
}

const IntRange Interpolate(const IntRange & start, const IntRange & end, float fractionTowardEnd)
{
	int x = Interpolate(start.min,end.min,fractionTowardEnd);
	int y = Interpolate(start.max,end.max,fractionTowardEnd);
	
	return IntRange(x,y);
}

const Rgba Interpolate(const Rgba & start, const Rgba & end, float fractionTowardEnd)
{
	// Idk if this was tested..?
	unsigned char r = (unsigned char)(Interpolate(start.r,end.r,fractionTowardEnd));
	unsigned char g = (unsigned char)(Interpolate(start.g,end.g,fractionTowardEnd));
	unsigned char b = (unsigned char)(Interpolate(start.b,end.b,fractionTowardEnd));
	unsigned char a = (unsigned char)(Interpolate(start.a,end.a,fractionTowardEnd));
	
	return Rgba(r,g,b,a);
}

float Crossfade(float(*EaseFunctionA)(float), float(*EaseFunctionB)(float), float t)
{
	float a = EaseFunctionA(t);
	float b = EaseFunctionB(t);
	
	float answer = a + t *(b-a);

	return answer;
}

float SmoothStart2(float t)
{
	return (t*t);
}

float SmoothStart3(float t)
{
	return (t*t*t);
}

float SmoothStart4(float t)
{
	return (t*t*t*t);
}

float SmoothStop2(float t)
{
	float fakePow = ((1-t) * (1-t));
	return (1- fakePow);
}

float SmoothStop3(float t)
{
	float fakePow = ((1-t) * (1-t)* (1-t));
	return (1- fakePow);
}

float SmoothStop4(float t)
{
	float fakePow = ((1-t) * (1-t)* (1-t)* (1-t));
	return (1- fakePow);
}

float SmoothStep3(float t)
{
	return Crossfade(SmoothStart2,SmoothStop2,t);
}

float GetAngularDisplacement(float startDegrees, float endDegrees)
{
	float angularDisplacement = endDegrees - startDegrees;

	while(angularDisplacement > 180)
	{
		angularDisplacement -= 360;
	}

	while(angularDisplacement < -180)
	{
		angularDisplacement += 360;
	}
	
	
	return angularDisplacement;
}

float TurnToward(float currentDegrees, float goalDegrees, float maxTurnDegrees)
{
	

	float angularDisplacement = GetAngularDisplacement(currentDegrees,goalDegrees);
	
	if(angularDisplacement <= maxTurnDegrees)
	{
		return goalDegrees;
	}

	if(angularDisplacement > maxTurnDegrees) // positive direction
	{
		return currentDegrees + maxTurnDegrees;
	}

	if(angularDisplacement < maxTurnDegrees) // negative direction
	{
		return currentDegrees - maxTurnDegrees;
	}

	
	return goalDegrees;
}

float DotProduct(const Vector2& a, const Vector2& b)
{
	float answer = (a.x * b.x) + (a.y * b.y);
	return answer;
}

Vector3 GetWorldPositionFromACamerasDirections(const Vector3& currentPosition, const Vector3& cameraUp, const Vector3& cameraRight, const Vector3 cameraForward)
{
	Vector3 world_offset = 
		cameraRight * currentPosition.x 
		+ cameraUp* currentPosition.y 
		+ cameraForward * currentPosition.z;

	return world_offset;
}

bool AreBitsSet(unsigned char bitFlags8, unsigned char flagsToCheck)
{
	return (bitFlags8 & flagsToCheck) == flagsToCheck;
}

bool AreBitsSet(unsigned int bitFlags32, unsigned int flagsToCheck)
{
	return (bitFlags32 & flagsToCheck) == flagsToCheck;
}

void SetBits(unsigned char& bitFlags8, unsigned char flagsToSet)
{
	bitFlags8 |= flagsToSet;
}

void SetBits(unsigned int& bitFlags32, unsigned int flagsToSet)
{
	bitFlags32 |= flagsToSet;
}

void ClearBits(unsigned char& bitFlags8, unsigned char flagToClear)
{
	bitFlags8 &= ~flagToClear;
}

void ClearBits(unsigned int& bitFlags32, unsigned int flagToClear)
{
	bitFlags32 &= ~flagToClear;
}

bool NearlyEqual(float a, float b, float tolerance /*= .00001f*/)
{
	// (|A - B| < ErrorTolerance)

	float value = abs(a - b);

	return value < tolerance;
}

bool FloatEqual(float a, float b)
{
	return NearlyEqual( a, b, .000001f);
}

bool Quadratic(Vector2* out, float a, float b, float c)
{
	// Guide : https://www.programiz.com/cpp-programming/examples/quadratic-roots
	
	float discriminant, x1,x2;
	
	discriminant = b*b - 4*a*c;

	// Roots are real and different
	if (discriminant > 0) 
	{
		x1 = (-b + sqrt(discriminant)) / (2*a);
		x2 = (-b - sqrt(discriminant)) / (2*a);

		// make sure the smallest one is first
		if(x1 < x2)
		{
			out->x = x1;
			out->y = x2;
		}
		else
		{
			out->x = x2;
			out->y = x1;
		}
		
		return true;

	} // Roots are real and same
	else if (discriminant == 0) 
	{
		
		x1 = (-b + sqrt(discriminant)) / (2*a);
		
		out->x = x1;
		out->y = x1;

		return true;
	}
	
	// Else the roots aren't real numbers so we return false
	return false;
}

Vector2 PolarToCartesian(float radius, float degrees)
{
	float x = radius * CosDegrees(degrees);
	float y = radius * SinDegrees(degrees);

	return Vector2(x,y);
}

Vector3 PolarToCartesian(float radius, float rotation, float azimuth)
{
	return PolarToCartesian(Vector3(radius,rotation,azimuth));
}

Vector3 PolarToCartesian(Vector3 spherical)
{	
	// Math reference
	// Azimus is vertical movement (also called phi)
	// Rot is horizontal movement (called theta)
	// x = rcos(phi) * cos(theta)
	// y = sin(phi)
	// z = rcos(phi) * sin(theta)
	
	// Tested in Tactics orbit cam in function SetSphericalCoordinate()
	
	// just to make it cleaner
	float radius = spherical.x;
	float rotation = spherical.y;
	float azimuth = spherical.z;


	Vector3 result;
	result.x = radius * CosDegrees(rotation) * CosDegrees(azimuth);
	result.y = radius * SinDegrees(azimuth);
	result.z = radius * CosDegrees(azimuth) * SinDegrees(rotation);


	return result;

}

Vector3 CartesianToPolar(Vector3 position)
{
	UNIMPLEMENTED();
	return position;

}


float ConvertRadiansToDegrees(float radians)
{
	// (180/pi) * radians)
	float answer = (180.f / PI) * radians;

	return answer;
}

float ConvertDegreesToRadians(float degrees)
{
	// (pi / 180) * degrees
	float answer = (PI / 180.f) * degrees;

	return answer;
}

float CosDegrees(float degrees)
{
	float answer = cosf(ConvertDegreesToRadians(degrees)); 

	return answer;
}

float SinDegrees(float degrees)
{
	float answer = sinf(ConvertDegreesToRadians(degrees));

	return answer;
}

//-----------------------------------------------------------------------------------
// Randoms
//---------------------------------------------------------------------------------------
float GetRandomFloat(float min, float max)
{
	float random = ((float) rand()) / (float) RAND_MAX;
	float differenceBetween = max - min;
	float temp = random * differenceBetween;
	return min + temp;
}

int GetRandomIntRange(int minInclusive, int maxInclusive)
{
	int theRange = (maxInclusive - minInclusive) + 1;
	int answer = minInclusive + GetRandomIntLessThan(theRange);
	return answer;
}

float GetRandomFloatZeroToOne()
{
	return (float)rand() / (float)RAND_MAX;
}

int GetRandomIntLessThan(int maxNotInclusive)
{
	return rand() % maxNotInclusive;
}

Vector3 GetRandomDirection()
{
	// Direction is unit length
	float x = GetRandomFloat(-1.f, 1.f);
	float y = GetRandomFloat(-1.f, 1.f);
	float z = GetRandomFloat(-1.f, 1.f);

	return Vector3(x,y,z);
}

const Vector2 GetProjectedVector(const Vector2 & vectorToProject, const Vector2 & projectOnto)
{
	Vector2 normalProject = projectOnto;
	
	// MAKE SURE ITS NORMALIZED
	if(projectOnto.GetLength() != 1.f){normalProject = projectOnto.GetNormalized();}

	float dot = DotProduct(vectorToProject,normalProject);
	Vector2 answer = normalProject * dot;

	return answer;
}

const Vector2 GetTransformedIntoBasis(const Vector2 & originalVector, const Vector2 & newBasisI, const Vector2 & newBasisJ)
{
	/*Vector2 answer = (originalVector.x * newBasisI) + (originalVector.y * newBasisJ);*/
	float i = DotProduct(originalVector,newBasisI) / newBasisI.GetLengthSquared();
	float j = DotProduct(originalVector,newBasisJ) / newBasisJ.GetLengthSquared();

	return Vector2(i,j);
}

const Vector2 GetTransformedOutOfBasis(const Vector2 & vectorInBasis, const Vector2 & oldBasisI, const Vector2 & oldBasisJ)
{
	return vectorInBasis.x * oldBasisI + vectorInBasis.y * oldBasisJ;
	
}

void DecomposeVectorIntoBasis(const Vector2 & originalVector, const Vector2 & newBasisI, const Vector2 & newBasisJ, Vector2 & out_vectorAlongI, Vector2 & out_vectorAlongJ)
{
	float strengthInIDirection = DotProduct(originalVector,newBasisI.GetNormalized());
	float strengthInJDirection = DotProduct(originalVector,newBasisJ.GetNormalized());

	out_vectorAlongI = newBasisI * strengthInIDirection;
	out_vectorAlongJ = newBasisJ * strengthInJDirection;
}

Vector2 Reflect(const Vector2 & incoming, const Vector2 & surfaceNormal)
{	
	/*Vector2 v = incoming.GetNormalized();*/
	float dot = DotProduct(surfaceNormal, incoming);
	Vector2 vn = ( dot / surfaceNormal.GetLength()) * surfaceNormal.GetNormalized();
	Vector2 result = incoming - 2.f * vn;
	/*float strengthofVn = DotProduct(incoming,surfaceNormal); 

	Vector2 vn = strengthofVn * surfaceNormal;

	Vector2 vt = incoming - vn;

	Vector2 reflection = vt - vn;
	*/
	//Vector2 answer = incoming + reflection;

	return result;
}

//-----------------------------------------------------------------------------
// DISTANCE
float GetDistance(Vector2 firstVector, Vector2 secondVector)
{
	// This can be optimized later if needed, I prefer breaking it down this way
	float firstStep = (secondVector.x - firstVector.x) * (secondVector.x - firstVector.x);
	float secondStep = (secondVector.y - firstVector.y) * (secondVector.y - firstVector.y);
	float answer = sqrtf((secondStep + firstStep));

	return answer;
}

float GetDistance(Vector3& firstVector, Vector3& secondVector)
{
	// This can be optimized later if needed, I prefer breaking it down this way
	float firstStep = (secondVector.x - firstVector.x) * (secondVector.x - firstVector.x);
	float secondStep = (secondVector.y - firstVector.y) * (secondVector.y - firstVector.y);
	float thirdStep = (secondVector.z - firstVector.z) * (secondVector.z - firstVector.z);

	float answer = sqrtf((secondStep + firstStep + thirdStep));

	return answer;
}

float GetDistanceSquared(Vector2 firstVector, Vector2 secondVector)
{
	return GetDistance(firstVector,secondVector) * GetDistance(firstVector,secondVector);
}

void GenerateArbitraryTangents(Vector3* tangent, Vector3* bitangent, const Vector3& normal)
{
	Vector3 right = Cross(normal, Vector3::UP);

	// if its zero we need another one
	if(right.IsNearZero())
	{
		right = Cross(normal, Vector3::RIGHT);
	}

	right.Normalize();

	Vector3 up = Cross(right, normal);
	up.Normalize();

	// return
	*tangent = right;
	*bitangent = up;
}

//---------------------------------------------------------------------------------
// Overlapping
bool DoAABBsOverlap(const AABB2& a, const AABB2& b)
{
// 	bool answer = false;
// 
// 	if(a.IsPointInside(b.maxs.x, b.maxs.y) && a.IsPointInside(b.mins.x, b.mins.y) && a.IsPointInside(b.maxs.x, b.mins.y) && a.IsPointInside(b.mins.x, b.maxs.y))
// 	{
// 		return true;
// 	}

	// if a.maxs.x > b.maxs.x || if a.mins.x > b.mins.x

	if(a.maxs.x < b.mins.x || a.mins.x > b.maxs.x){return false;}
	if(a.maxs.y < b.mins.y || a.mins.y > b.maxs.y){return false;}


	return true;
}

bool DoDiscsOverlap(const Disc2& a, const Disc2& b)
{
	
	return DoDiscsOverlap(a.center,a.radius,b.center,b.radius);
}

bool DoDiscsOverlap(const Vector2& aCenter, float aRadius, const Vector2& bCenter, float bRadius)
{
	float radiusRange = aRadius + bRadius;
	float dis = (aCenter - bCenter).GetLength();
// 	if((aCenter.x + bCenter.x) < radiusRange || (aCenter.y + bCenter.y) < radiusRange)
// 	{
// 		return true;
// 	}

	if(dis < radiusRange)
	{
		return true;
	}


	return false;
}

Vector2 CorrectiveDiscVsDot(const Disc2& nonSolid, const Vector2& solid)
{
	//if(nonSolid.IsPointInside(solid) == false){return nonSolid.center;} // The point can't collide if it isn't inside

	Vector2 displacement = (nonSolid.center - solid); // Find the displacement Vector
	
	Vector2 direction = displacement.GetNormalized(); // Get the direction

	float distance = displacement.GetLength(); // Find the distance between

	float overlap = nonSolid.radius - distance ; // Find the Overlap

	Vector2 deltaPosition = (direction * overlap); // Find the correction for both directions

	// Updates the current position
	Vector2 corrected = nonSolid.center + deltaPosition;

	return corrected; // return new position
}

Vector2 CorrectiveDiscVsDisc(const Disc2& nonSolid, const Disc2& solid)
{
	// THIS IS WRONG FIX LATER
	
	if(DoDiscsOverlap(nonSolid,solid) == false){return nonSolid.center;} // no correction
	
	Vector2 displacement = (nonSolid.center - solid.center);

	Vector2 direction = displacement.GetNormalized();

	float distance = displacement.GetLength();

	float overlap = (nonSolid.radius + solid.radius) - distance;

	Vector2 deltaPosition = (direction * overlap); // Find the correction for both directions

	float directionInDegrees = direction.GetOrientationDegrees();
	// Push only one direction based off degree
	if(directionInDegrees < 45.f && (360 - directionInDegrees) >= 315.f){deltaPosition.y = 0.f;} // Push from right
	if(directionInDegrees > 45.f && directionInDegrees <= 135.f){deltaPosition.x = 0.f;} // up
	if(directionInDegrees > 135.f && directionInDegrees <= 225.f){deltaPosition.y = 0.f;} // left
	if(directionInDegrees > 225.f && directionInDegrees <= 315.f){deltaPosition.x = 0.f;} // bottom


	Vector2 corrected = nonSolid.center + deltaPosition;

	return corrected;
	
}

Vector2 CorrectiveDiscVsBox(const Disc2& nonSolid, const AABB2& solid)
{
// 	 Just getting it working, gonna need to do a "are they even colliding" check eventually for opt
// 	 WIll be SO bad whenever multiple entities are calling this

	Vector2 displacement = (nonSolid.center - solid.GetCenter()); // Find the displacement Vector

	Vector2 direction = displacement.GetNormalized(); // Get the direction

	float distancef = displacement.GetLength(); // Find the distance between

	float overlap = (nonSolid.radius + (solid.maxs.x - solid.mins.x) *.5f) - distancef ; // Find the Overlap

	Vector2 deltaPosition = (direction * overlap); // Find the correction for both directions

	float directionInDegrees = direction.GetOrientationDegrees();

	if(directionInDegrees < 0){directionInDegrees+= 360.f;}

	Vector2 endPosition = nonSolid.center;
	float distanceBetween = nonSolid.radius + (solid.maxs.x - solid.mins.x)*.5f;
	
	// Push only one direction based off degree
	float distance = nonSolid.center.x - solid.GetCenter().x;
	if(directionInDegrees < 45.f || directionInDegrees > 315.f)
	{
		if(distance < distanceBetween) 
		{
			endPosition.x = solid.maxs.x + nonSolid.radius;
		}
		return endPosition;
	} // right
	if(directionInDegrees > 135.f && directionInDegrees < 225.f)
	{
		distance = -distance;
		if(distance < distanceBetween) 
		{
			endPosition.x = solid.mins.x - nonSolid.radius;
		}
		return endPosition;
	} // left


	distance = nonSolid.center.y - solid.GetCenter().y;
	if(directionInDegrees > 45.f && directionInDegrees < 135.f)
	{
		//float distance = nonSolid.center.y - solid.GetCenter().y;
		if(distance < distanceBetween) 
		{
			endPosition.y = solid.maxs.y + nonSolid.radius;
		}
		return endPosition;
	} // up
	if(directionInDegrees > 225.f && directionInDegrees < 315.f)
	{
		distance = -distance;
		if(distance < distanceBetween) 
		{
			endPosition.y = solid.mins.y - nonSolid.radius;
		}
		return endPosition;
	} // bottom
	
	// Updates the current position
	//Vector2 corrected = nonSolid.center - deltaPosition;

	//return corrected; // return new position
	return endPosition;
}

bool CheckRandomChance(float chanceForSuccess)
{
	float diceRoll = GetRandomFloat(0.f,100.f);

	if(chanceForSuccess >= diceRoll)
		return true;

	return false;
}

