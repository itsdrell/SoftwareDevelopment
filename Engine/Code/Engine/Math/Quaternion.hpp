#pragma once
#include "Vectors/Vector3.hpp"
#include "Vectors/Vector4.hpp"
#include "Matrices/Matrix44.hpp"
#include "MathUtils.hpp"


// "DAMN NEAR EVERYTHING IS FROM" - Forseth;
// 3D Math Primer for Graphics and Game Development
// by Fletcher Dunn and Ian Parberry

#pragma once
#if !defined( __CORE_MATH_Quaternion__ )
#define __CORE_MATH_Quaternion__

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/


/************************************************************************/
/*                                                                      */
/* DEFINES AND CONSTANTS                                                */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* MACROS                                                               */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* TYPES                                                                */
/*                                                                      */
/************************************************************************/

enum eRotationOrder
{
	ROTATE_XYZ, 
	ROTATE_YXZ,
	ROTATE_YZX,
	ROTATE_ZYX,
	ROTATE_XZY,
	ROTATE_DEFAULT = ROTATE_ZYX
};

/************************************************************************/
/*                                                                      */
/* STRUCTS                                                              */
/*                                                                      */
/************************************************************************/
struct Quaternion;

//------------------------------------------------------------------------
// forward decl ops
inline Quaternion operator-( Quaternion const &a );
inline Quaternion operator*( Quaternion const &a, Quaternion const &b ); 
inline Vector3 operator*( Vector3 const &v, Quaternion const &q );
inline Vector4 operator*( Vector4 const &v, Quaternion const &q );


//------------------------------------------------------------------------
//------------------------------------------------------------------------
struct Quaternion 
{
// 	union 
// 	{
// 		struct 
// 		{
// 			Vector3 i;
// 			float r;
// 		};
// 		
// 		Vector4 data;
// 	};

	float r;
	Vector3 i;

	Quaternion() 
		: i(0.0f)
		, r(1.0f) {}
	
	Quaternion( float _x, float _y, float _z, float _w );
	Quaternion( float _r, Vector3 const & _i );
	Quaternion(const Quaternion& copyOf);

	void normalize();
	bool is_unit() const;
	Vector4 GetAsVector4() const ;

	float get_magnitude() const;
	float get_magnitude2() const;

	Quaternion get_conjugate() const;
	Quaternion get_inverse() const;

	Vector3 GetEuler() const;
	Matrix33 get_mat3() const;
	Matrix44 get_mat4() const;


	inline void invert() { *this = get_inverse(); } 

	inline Vector3 get_right() const { return Vector3::RIGHT * (*this); }
	inline Vector3 get_up() const { return Vector3::UP * (*this); }
	inline Vector3 get_forward() const { return Vector3::FORWARD * (*this); }

	
	//--------------------------------------------------------------------------
	// For Rotator
	Quaternion FromMatrix(const Matrix44& matrix) { return Quaternion::MakeFromMatrix(matrix); }
	Quaternion FromEuler(const Vector3& euler) { return Quaternion::MakeFromEuler(euler, ROTATE_ZYX); }
	Matrix44 GetAsMatrix() { return get_mat4(); }
	void RotateByEuler(const Vector3& euler);

	//------------------------------------------------------------------------
	// STATIC
	//------------------------------------------------------------------------
	static Quaternion const IDENTITY;

	static Quaternion MakeFromMatrix( Matrix33 &mat ); 
	static Quaternion MakeFromMatrix( Matrix44 const &mat ); 

	static Quaternion Around( Vector3 const &axis, float const angle_radians );
	//static Quaternion FromEuler( Vector3 const &euler );
	//static inline Quaternion FromEuler( float x, float y, float z ) { return FromEuler( Vector3( x, y, z ) ); }
	static Quaternion MakeFromEuler( Vector3 const &euler, eRotationOrder const order = ROTATE_ZYX );

	static Quaternion LookAt( Vector3 const forward );

	std::string ToString() const;
};

// another name for this is a Quaternion
typedef Quaternion rotation_fl;

/************************************************************************/
/*                                                                      */
/* CLASSES                                                              */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* GLOBAL VARIABLES                                                     */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* FUNCTION PROTOTYPES                                                  */
/*                                                                      */
/************************************************************************/
Quaternion QuaternionGetDifference( Quaternion const &a, Quaternion const &b ); 
float QuaternionAngleInRadians( Quaternion const &a, Quaternion const &b );

float QuaternionDot( Quaternion const &a, Quaternion const &b );

// Works on normalized Quaternion - returns a non-normalized Quaternion
Quaternion QuaternionLog( Quaternion const &q );

// Works on Quaternions of the form [0, a * i]
Quaternion QuaternionExp( Quaternion const &q );

Quaternion QuaternionScale( Quaternion const &q, float s ); 
Quaternion QuaternionPow( Quaternion const &q, float e );
Quaternion Slerp( Quaternion const &a, Quaternion const &b, float const t ); 
Quaternion QuaternionRotateTorward( Quaternion const &start, Quaternion const &end, float maxAngelFloat );


Quaternion Lerp( Quaternion const &a, Quaternion const &b, float const &t );

bool NearEqual( Quaternion const &a, Quaternion const &b );

// template <>
// bool Stream::write( Quaternion const &q );
// 
// template <>
// bool Stream::read( Quaternion *q );

/************************************************************************/
/* OPERATORS                                                            */
/************************************************************************/
//------------------------------------------------------------------------
inline Quaternion operator-( Quaternion const &a ) 
{
	return Quaternion( -a.r, -a.i );
}

//------------------------------------------------------------------------
inline Quaternion operator*( Quaternion const &a, Quaternion const &b ) 
{
	// Taken from 3D Math Primer - it's the reverse definition since
	// my engine also uses Row Major matrices by default, and I want to 
	// keep with that.
	//Quaternion ret;
	//ret.r = a.r * b.r - DotProduct(a.i, b.i);
	//ret.i = b.i * a.r 
	//	+ a.i * b.r 
	//	+ Cross( b.i, a.i );
	//
	//return ret;

	Quaternion result;

	result.r =		(a.r * b.r)		-	(a.i.x * b.i.x) -	(a.i.y * b.i.y) -	(a.i.z * b.i.z);
	result.i.x =	(a.r * b.i.x)	+	(a.i.x * b.r)	+	(a.i.z * b.i.y) -	(a.i.y * b.i.z);
	result.i.y =	(a.r * b.i.y)	+	(a.i.y * b.r) +		(a.i.x * b.i.z) -	(a.i.z * b.i.x);
	result.i.z =	(a.r * b.i.z)	+	(a.i.z * b.r) +		(a.i.y * b.i.x) -	(a.i.x * b.i.y);

	

	return result;
}

//------------------------------------------------------------------------
inline Vector3 operator*( Vector3 const &v, Quaternion const &q ) 
{
	Quaternion p = Quaternion( 0.0f, v );
	//Quaternion result = q.get_inverse() * p * q;
	Quaternion result = q * p * q.get_inverse();

	return result.i;
}

//------------------------------------------------------------------------
inline Vector4 operator*( Vector4 const &v, Quaternion const &q ) 
{
	Vector3 result = v.xyz() * q;
	return Vector4( result, v.w );
}

//------------------------------------------------------------------------
inline bool operator==( Quaternion const &a, Quaternion const &b ) 
{
	return (FloatEqual(a.r, b.r) && (a.i == b.i))
		|| (FloatEqual(a.r, -b.r) && (a.i == -b.i));
}

//------------------------------------------------------------------------
inline bool operator!=( Quaternion const &a, Quaternion const &b ) 
{
	return !(a == b);
}


#endif 