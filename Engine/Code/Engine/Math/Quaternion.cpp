/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "Quaternion.hpp"
#include <corecrt_math.h>
#include "Matrices/Matrix33.hpp"
#include "Engine/Math/Matrices/Matrix44.hpp"
#include "../Core/Tools/ErrorWarningAssert.hpp"


/************************************************************************/
/*                                                                      */
/* DEFINES AND CONSTANTS                                                */
/*                                                                      */
/************************************************************************/
Quaternion const Quaternion::IDENTITY = Quaternion( 1.0f, 0.0f, 0.0f, 0.0f );

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

/************************************************************************/
/*                                                                      */
/* STRUCTS                                                              */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* CLASSES                                                              */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* LOCAL VARIABLES                                                      */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* GLOBAL VARIABLES                                                     */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* LOCAL FUNCTIONS                                                      */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* EXTERNAL FUNCTIONS                                                   */
/*                                                                      */
/************************************************************************/
//------------------------------------------------------------------------
Quaternion::Quaternion( float r, float ix, float iy, float iz ) 
{
	i = Vector3(ix, iy, iz);
	this->r = r;

}

//------------------------------------------------------------------------
Quaternion::Quaternion( float _r, Vector3 const &_i )
{
	r = _r;
	i = _i;
}

Quaternion::Quaternion(const Quaternion& copyOf)
{
	r = copyOf.r;
	i = copyOf.i;
}

//------------------------------------------------------------------------
float Quaternion::get_magnitude() const
{
	return sqrtf( get_magnitude2() );
}

//------------------------------------------------------------------------
float Quaternion::get_magnitude2() const 
{
	//return DotProduct( data, data );
	return DotProduct( GetAsVector4(), GetAsVector4() );

}

//------------------------------------------------------------------------
void Quaternion::normalize() 
{
	float const len_squared = get_magnitude2();

	// Already normalized.
	if (FloatEqual(len_squared, 1.0f)) {
		return;
	}

	if (r < .9999f) {
		float s = sqrtf( 1.0f - r * r );
		Vector3 vector = i / s;
		Normalize( vector );

		i = vector * s;
	} else {
		r = 1.0f;
		i = Vector3(0.0f);
	}
}

//------------------------------------------------------------------------
bool Quaternion::is_unit() const
{
	return FloatEqual( QuaternionDot(*this, *this), 1.0f );
}

Vector4 Quaternion::GetAsVector4() const
{
	return Vector4(r, i.x, i.y, i.z);
}

//------------------------------------------------------------------------
Quaternion Quaternion::get_conjugate() const
{
	//if(*this == Quaternion(r, (Vector3::ZERO)))
	//{
	//	return Quaternion(-r, -i);
	//}
	
	return Quaternion( r, -i );
}

//------------------------------------------------------------------------
Quaternion Quaternion::get_inverse() const
{
	// We only deal with unit quaternions in this engine!
	// ASSERT( is_unit() );
	return get_conjugate();
}

//------------------------------------------------------------------------
// Look at the matrix of a rotation around a vector.  
// Notices the values in it - those are the components 
// of a quaternion, or some combination thereof - so we can 
// refigure it into it; 
Matrix33 Quaternion::get_mat3() const
{
	// imaginary part
	float const x = i.x;
	float const y = i.y;
	float const z = i.z;

	// cache off some squares
	float const x2 = x * x;
	float const y2 = y * y;
	float const z2 = z * z;

	Vector3 row0 = Vector3( 1.0f - 2.0f * y2 - 2.0f * z2, 
		2.0f * x * y + 2.0f * r * z, 
		2.0f * x * z - 2.0f * r * y );
	Vector3 row1 = Vector3( 2 * x * y - 2.0f * r * z, 
		1.0f - 2.0f * x2 - 2.0f * z2, 
		2.0f * y * z + 2.0f * r * x );
	Vector3 row2 = Vector3( 2.0f * x * z + 2.0f * r * y, 
		2.0f * y * z - 2.0f * r * x, 
		1.0f - 2.0f * x2 - 2.0f * y2 );

	Matrix33 ret;

	ret.SetBasis( row0, row1, row2 );
	ret.SetBasis(Vector3(row0.x, row1.x, row2.x), Vector3(row0.y, row1.y, row2.y), Vector3(row0.z, row1.z, row2.z));
	return ret;
}

//------------------------------------------------------------------------
Matrix44 Quaternion::get_mat4() const
{
	return Matrix44( get_mat3() );
}

//------------------------------------------------------------------------
Vector3 Quaternion::get_euler() const 
{
	// From http://www.darwin3d.com/gamedev/quat2eul.cpp

	Vector3 euler;
	/// Local Variables ///////////////////////////////////////////////////////////
	float matrix[3][3];
	float cx,sx;
	float cy,sy,yr;
	float cz,sz;
	///////////////////////////////////////////////////////////////////////////////
	// CONVERT QUATERNION TO MATRIX - I DON'T REALLY NEED ALL OF IT
	matrix[0][0] = 1.0f - (2.0f * i.y * i.y) - (2.0f * i.z * i.z);
	//	matrix[0][1] = (2.0f * quat->x * quat->y) - (2.0f * quat->w * quat->z);
	//	matrix[0][2] = (2.0f * quat->x * quat->z) + (2.0f * quat->w * quat->y);
	matrix[1][0] = (2.0f * i.x * i.y) + (2.0f * r * i.z);
	//	matrix[1][1] = 1.0f - (2.0f * quat->x * quat->x) - (2.0f * quat->z * quat->z);
	//	matrix[1][2] = (2.0f * quat->y * quat->z) - (2.0f * quat->w * quat->x);
	matrix[2][0] = (2.0f * i.x * i.z) - (2.0f * r * i.y);
	matrix[2][1] = (2.0f * i.y * i.z) + (2.0f * r * i.x);
	matrix[2][2] = 1.0f - (2.0f * i.x * i.x) - (2.0f * i.y * i.y);

	sy = -matrix[2][0];
	cy = sqrt(1 - (sy * sy));
	yr = (float)atan2(sy,cy);
	euler.y = (yr * 180.0f) / (float)PI;

	// AVOID DIVIDE BY ZERO ERROR ONLY WHERE Y= +-90 or +-270 
	// NOT CHECKING cy BECAUSE OF PRECISION ERRORS
	if (sy != 1.0f && sy != -1.0f)	
	{
		cx = matrix[2][2] / cy;
		sx = matrix[2][1] / cy;
		euler.x = ((float)atan2(sx,cx) * 180.0f) / (float)PI;	// RAD TO DEG

		cz = matrix[0][0] / cy;
		sz = matrix[1][0] / cy;
		euler.z = ((float)atan2(sz,cz) * 180.0f) / (float)PI;	// RAD TO DEG
	}
	else
	{
		// SINCE Cos(Y) IS 0, I AM SCREWED.  ADOPT THE STANDARD Z = 0
		// I THINK THERE IS A WAY TO FIX THIS BUT I AM NOT SURE.  EULERS SUCK
		// NEED SOME MORE OF THE MATRIX TERMS NOW
		matrix[1][1] = 1.0f - (2.0f * i.x * i.x) - (2.0f * i.z * i.z);
		matrix[1][2] = (2.0f * i.y * i.z) - (2.0f * r * i.x);
		cx = matrix[1][1];
		sx = -matrix[1][2];
		euler.x = ((float)atan2(sx,cx) * 180.0f) / (float)PI;	// RAD TO DEG

		cz = 1.0f;
		sz = 0.0f;
		euler.z = ((float)atan2(sz,cz) * 180.0f) / (float)PI;	// RAD TO DEG
	}
	
	return euler;
}

//------------------------------------------------------------------------
Quaternion Quaternion::FromMatrix( Matrix33 &mat ) 
{
	float m00 = mat.GetValueAt( 0, 0 );
	float m11 = mat.GetValueAt( 1, 1 );
	float m22 = mat.GetValueAt( 2, 2 );
	float tr = m00 + m11 + m22;

	float m21 = mat.GetValueAt( 2, 1 );
	float m12 = mat.GetValueAt( 1, 2 );
	float m02 = mat.GetValueAt( 0, 2 );
	float m20 = mat.GetValueAt( 2, 0 );
	float m10 = mat.GetValueAt( 1, 0 );
	float m01 = mat.GetValueAt( 0, 1 );

	Quaternion q;
	if (tr >= 0.0f) {
		float s = sqrtf(tr + 1.0f) * 2.0f;
		float is = 1.0f / s;
		q.r = .25f * s;
		q.i.x = (m21 - m12) * is;
		q.i.y = (m02 - m20) * is;
		q.i.z = (m10 - m01) * is;
	} else if ((m00 > m11) & (m00 > m22)) {
		float s = sqrtf( 1.0f + m00 - m11 - m22 ) * 2.0f;
		float is = 1.0f / s;
		q.r = (m21 - m12) * is;
		q.i.x = .25f * s;
		q.i.y = (m01 + m10) * is;
		q.i.z = (m02 + m20) * is;
	} else if (m11 > m22) {
		float s = sqrtf( 1.0f + m11 - m00 - m22 ) * 2.0f;
		float is = 1.0f / s;
		q.r = (m02 - m20) * is;
		q.i.x = (m01 + m10) * is;
		q.i.y = .25f * s;
		q.i.z = (m12 + m21) * is;
	} else {
		float s = sqrtf( 1.0f + m22 - m00 - m11 ) * 2.0f;
		float is = 1.0f / s;
		q.r = (m10 - m01) * is;
		q.i.x = (m02 + m20) * is;
		q.i.y = (m12 + m21) * is;
		q.i.z = .25f * s;
	}

	q.normalize();
	return q;
}

//------------------------------------------------------------------------
Quaternion Quaternion::FromMatrix( Matrix44 const &mat )
{
	Matrix33 rot = mat.GetAsMatrix33();
	Quaternion newQaut = Quaternion::FromMatrix(rot);
	return newQaut;
}

//------------------------------------------------------------------------
Quaternion Quaternion::Around( Vector3 const &axis, float const angle ) 
{
	float const half_angle = .5f * angle;
	float const c = CosDegrees(half_angle);
	float const s = SinDegrees(half_angle);

	return Quaternion( c, axis * s );
}

//------------------------------------------------------------------------
Quaternion Quaternion::FromEuler( Vector3 const &euler, eRotationOrder const rot_order ) 
{
	//if (rot_order == ROTATE_DEFAULT) {
	//	return Quaternion::FromEuler(euler);
	//} else {
		//Quaternion rotx = Quaternion::Around( Vector3::RIGHT, euler.x * .5f );
		//Quaternion roty = Quaternion::Around( Vector3::UP, euler.y * .5f );
		//Quaternion rotz = Quaternion::Around( Vector3::FORWARD, euler.z * .5f );
		Quaternion rotx = Quaternion::Around( Vector3::RIGHT, euler.x);
		Quaternion roty = Quaternion::Around( Vector3::UP, euler.y  );
		Quaternion rotz = Quaternion::Around( Vector3::FORWARD, euler.z  );


		switch (rot_order) {
		case ROTATE_XYZ:
			return rotx * roty * rotz;
		case ROTATE_XZY:
			return rotx * rotz * roty;
		case ROTATE_YXZ:
			return roty * rotx * rotz;
		case ROTATE_YZX:
			return roty * rotz * rotx;
		case ROTATE_ZYX:
		default:
			return rotz * roty * rotx;
		}
	
}

//------------------------------------------------------------------------
// Quaternion Quaternion::FromEuler( Vector3 const &euler ) 
// {
// 	// If this changes - this method is no longer valid
// 	GUARANTEE_OR_DIE( ROTATE_DEFAULT == ROTATE_ZYX , "quaternian error");
// 
// 	Vector3 const he = euler * .5f;
// 	float cx = cosf( he.x );
// 	float sx = sinf( he.x );
// 	float cy = cosf( he.y );
// 	float sy = sinf( he.y );
// 	float cz = cosf( he.z );
// 	float sz = sinf( he.z );
// 
// 	float r =   cx*cy*cz + sx*sy*sz;
// 	float ix =  sx*cy*cz + cx*sy*sz;
// 	float iy =  cx*sy*cz - sx*cy*sz;
// 	float iz =  cx*cy*sz - sx*sy*cz;
// 
// 
// 	Quaternion q = Quaternion( r, Vector3(ix, iy, iz ));
// 	q.normalize();
// 
// 	GUARANTEE_OR_DIE( q.is_unit(), "Error" );
// 	return q;
// }

//------------------------------------------------------------------------
Quaternion Quaternion::LookAt( Vector3 const forward )
{
	Matrix33 lookat = Matrix33::LookAt( forward );
	return FromMatrix( lookat );
}


std::string Quaternion::ToString() const
{
	std::string result = Stringf("r : %f (%f , %f , %f )" , r, i.x, i.y, i.z);

	return result;
}

//------------------------------------------------------------------------
Quaternion QuaternionGetDifference( Quaternion const &a, Quaternion const &b ) 
{
	return a.get_inverse() * b;
}

//------------------------------------------------------------------------
float QuaternionAngleInRadians( Quaternion const &a, Quaternion const &b )
{
	// so just trying to get real part of the difference
	// inverse of a real part is the real part, so that doesn't change
	// but the imaginary part negates.
	// so need to compute that.
	float new_real = a.r * b.r - DotProduct(-a.i, b.i);
	// new_real = cosf(half_angle);
	// half_angle = acosf(new_real);
	// angle = 2 * half_angle;

	float angle = 2.0f * acosf(new_real);
	return angle;
}

//------------------------------------------------------------------------
float QuaternionDot( Quaternion const &a, Quaternion const &b ) 
{
	return DotProduct( a.GetAsVector4(), b.GetAsVector4() );
}


//------------------------------------------------------------------------
// Works on normalized quaternion - returns a non-normalized quaternion
Quaternion QuaternionLog( Quaternion const &q ) 
{
	// Purely just the real part
	if (q.r >= .9999f) {
		return Quaternion( 0, 0, 0, 0 );
	}

	float half_angle = acosf(q.r);
	float s = sinf( half_angle );

	Vector3 n =  q.i / s;
	return Quaternion( 0.0f, n * half_angle );
}

//------------------------------------------------------------------------
// Works on quaternions of the form [0, a * i]
Quaternion QuaternionExp( Quaternion const &q ) 
{
	//float half_angle = Magnitude( q.i );
	float half_angle = q.i.GetLength();
	
	Vector3 vec = Vector3(0.0f);
	float r = cosf(half_angle);
	
	if (half_angle > 0.00001f) 
	{
		Vector3 n = q.i / half_angle;
		vec = n * sinf(half_angle);
	}

	return Quaternion( r, vec );
}

//------------------------------------------------------------------------
Quaternion QuaternionScale( Quaternion const &q, float s ) 
{
	return Quaternion( s * q.r, q.i * s);
}

//------------------------------------------------------------------------
Quaternion QuaternionPow( Quaternion const &q, float e ) 
{
	Quaternion ret = QuaternionExp( QuaternionScale( QuaternionLog( q ), e ) );
	ret.normalize();
	return ret;
}

//------------------------------------------------------------------------
Quaternion Slerp( Quaternion const &a, Quaternion const &b, float const t_val ) 
{
	float time = ClampFloat(t_val, 0.f, 1.f);
	float cos_angle = QuaternionDot( a, b );


	Quaternion start;
	if (cos_angle < 0.0f) {
		// If it's negative - it means it's going the long way
		// flip it.
		start = -a;
		cos_angle = -cos_angle;
	} else {
		start = a;
	}

	float f0, f1;
	if (cos_angle >= .9999f) {
		// very close - just linearly interpolate.
		f0 = 1.0f - time;
		f1 = time;
	} else {
		float sin_angle = sqrtf( 1.0f - cos_angle * cos_angle );
		float angle = atan2f( sin_angle, cos_angle );

		float den = 1.0f / sin_angle;
		f0 = sinf( (1.0f - time) * angle ) * den;
		f1 = sinf( time * angle ) * den;
	}

	Quaternion r0 = QuaternionScale( start, f0 );
	Quaternion r1 = QuaternionScale( b, f1 );
	return Quaternion( r0.r + r1.r, r0.i + r1.i );
}

//------------------------------------------------------------------------
Quaternion Lerp( Quaternion const &a, Quaternion const &b, float const &t ) 
{
	return Slerp( a, b, t );
	/*
	quaternion diff = QuaternionGetDifference( a, b );
	return a * QuaternionPow( diff, t );
	*/
}

//------------------------------------------------------------------------
Quaternion QuaternionRotateTorward( Quaternion const &start, Quaternion const &end, float maxAngelFloat ) 
{
	float max_angle_radians = ConvertDegreesToRadians(maxAngelFloat);
		
	float angle = acosf( QuaternionDot( start, end ) );
	if (angle < 0.0f) {
		angle = -angle;
	}

	if (FloatEqual(angle, 0.0f)) {
		return end;
	}

	// This was Clamo01
	float t = ClampFloat( max_angle_radians / angle , 0.f, 1.f);
	return Slerp( start, end, t );
}

//------------------------------------------------------------------------
bool NearEqual( Quaternion const &a, Quaternion const &b )
{
	float angle = QuaternionAngleInRadians(a, b);
	return angle < 0.001f;
}


//------------------------------------------------------------------------
//template <>
//bool Stream::write( Quaternion const &q )
//{
//	return write_array<float>( q.data.data, 4 );
//}
//
////------------------------------------------------------------------------
//template <>
//bool Stream::read( Quaternion *q )
//{
//	return read_array<float>( q->data.data, 4 );
//}