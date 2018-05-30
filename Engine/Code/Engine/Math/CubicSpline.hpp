#pragma once
#include <vector>
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////
// Standalone curve utility functions
//
// Note: Quadratic Bezier functions (only) specify an actual midpoint, not a Bezier guide point
/////////////////////////////////////////////////////////////////////////////////////////////////

// This is the one that goes A->B->C 
template< typename T >
T EvaluateQuadraticBezier( const T& startPos, const T& actualCurveMidPos, const T& endPos, float t );

template< typename T >
T EvaluateCubicBezier( const T& startPos, const T& guidePos1, const T& guidePos2, const T& endPos, float t );

template< typename T >
T EvaluateCubicHermite( const T& startPos, const T& startVel, const T& endPos, const T& endVel, float t );


/////////////////////////////////////////////////////////////////////////////////////////////////
// CubicSpline2D
// 
// Cubic Hermite/Bezier spline of Vector2 positions / velocities
/////////////////////////////////////////////////////////////////////////////////////////////////
class CubicSpline2D
{
public:
	CubicSpline2D() {}
	explicit CubicSpline2D( const Vector2* positionsArray, int numPoints, const Vector2* velocitiesArray=nullptr );
	~CubicSpline2D() {}

	// Mutators
	void		AppendPoint( const Vector2& position, const Vector2& velocity=Vector2::ZERO );
	void		AppendPoints( const Vector2* positionsArray, int numPoints, const Vector2* velocitiesArray=nullptr );
	void		InsertPoint( int insertBeforeIndex, const Vector2& position, const Vector2& velocity=Vector2::ZERO );
	void		RemovePoint( int pointIndex );
	void		RemoveAllPoints();
	void		SetPoint( int pointIndex, const Vector2& newPosition, const Vector2& newVelocity );
	void		SetPosition( int pointIndex, const Vector2& newPosition );
	void		SetVelocity( int pointIndex, const Vector2& newVelocity );
	void		SetCardinalVelocities( float tension=0.f, const Vector2& startVelocity=Vector2::ZERO, const Vector2& endVelocity=Vector2::ZERO );

	// Accessors
	int				GetNumPoints() const { return (int) m_positions.size(); }
	const Vector2	GetPosition( int pointIndex );
	const Vector2	GetVelocity( int pointIndex );
	int				GetPositions( std::vector<Vector2>& out_positions ) const;
	int				GetVelocities( std::vector<Vector2>& out_velocities ) const;
	Vector2			EvaluateAtCumulativeParametric( float t ) const;
	Vector2			EvaluateAtNormalizedParametric( float t ) const;

protected:
	std::vector<Vector2>	m_positions;
	std::vector<Vector2>	m_velocities;
};

template<typename T>
inline T EvaluateQuadraticBezier(const T & startPos, const T & actualCurveMidPos, const T & endPos, float t)
{
	// youre gonna have to find the mid point 
	// the mid point is find mid point of (a to c) then get a displacement to the actual curve and do that twice and you get B
	T midPoint = (startPos + endPos) * .5f;
	
	T displacement = actualCurveMidPos - midPoint;

	T b = displacement * 2.f;

	T e = Interpolate(startPos,b,t);
	T f = Interpolate(b,endPos,t);
	T p = Interpolate(e,f,t);


	return p;
}

template<typename T>
inline T EvaluateCubicBezier(const T & startPos, const T & guidePos1, const T & guidePos2, const T & endPos, float t)
{
	T a = startPos;
	T b = guidePos1;
	T c = guidePos2;
	T d = endPos;
	
	T e = Interpolate(a,b,t);
	T f = Interpolate(b,c,t);
	T g = Interpolate(c,d,t);
	T q = Interpolate(e,f,t);
	T r = Interpolate(f,g,t);
	T p = Interpolate(q,r,t);
	
	return p;
}

template<typename T>
inline T EvaluateCubicHermite(const T & startPos, const T & startVel, const T & endPos, const T & endVel, float t)
{
	T a = startPos;
	T d = endPos;
	// U is the startVel at A, V is the end velocity at D
	T b = startPos + (startVel / 3); // B = A + (U/3)
	T c = endPos - (endVel / 3); // C = D - (V/3)
	
	return EvaluateCubicBezier(a,b,c,d,t);
}
