#pragma once
#include "Engine/Math/Vectors/Vector2.hpp"
#include "Engine/Math/Vectors/Vector3.hpp"
#include "Engine/Math/Vectors/Vector4.hpp"
#include "Matrix33.hpp"

class Matrix44
{
public:
	
	Matrix44(); // default-construct to Identity matrix (via variable initialization)
	Matrix44(const Matrix44& copyOf);
	Matrix44(const Matrix33& a);
	explicit Matrix44( const float* sixteenValuesBasisMajor ); // float[16] array in order Ix, Iy...
	explicit Matrix44( const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation=Vector2(0.f,0.f) );
	explicit Matrix44(const Vector3& iBasis, const Vector3& jBasis, const Vector3& kBasis, const Vector3& translation = Vector3::ZERO);
	explicit Matrix44(const Vector4& iBasis, const Vector4& jBasis, const Vector4& kBasis, const Vector4& translation);

	// Operations
	Vector2 TransformPosition2D( const Vector2& position2D ) const; // Written assuming z=0, w=1
	Vector3 TransformPosition3D( const Vector3& position) const;
	Vector4 TransformHomogeneous(const Vector4& position) const;
	Vector2 TransformDisplacement2D( const Vector2& displacement2D ) const; // Written assuming z=0, w=0
	
	// Accessors
	Vector3 GetForward() const;
	Vector3 GetRight() const;
	Vector3 GetUp() const;
	Vector3 GetPosition() const;

	Vector3 GetXRow() const;
	Vector3 GetYRow() const;
	Vector3 GetZRow() const;

	Matrix33 GetAsMatrix33() const;
	
	void GetValuesAsArray(double* outArray);
	void GetValuesAsArray(float* outArray);

	// Operators
	void operator=( const Matrix44& copyOf );	
	Matrix44 operator*( Matrix44& theMatrix );

	// Mutators
	void SetIdentity();
	void SetValues( const float* sixteenValuesBasisMajor ); // float[16] array in order Ix, Iy...
	void SetTranslation(const Vector3& theTranslation);
	
	void Append( const Matrix44& matrixToAppend ); // a.k.a. Concatenate (right-multiply)
	void RotateDegrees2D( float rotationDegreesAboutZ ); // 
	void Translate2D( const Vector2& translation );
	void Translate3D( const Vector3& translation);
	void ScaleUniform2D( float scaleXY );
	void Scale2D( float scaleX, float scaleY );
	void Transpose();
	void Invert();
	void InvertOrthonormal();

	// Producers
	static Matrix44 MakeRotationDegreesAroundZ(float rotationDegrees);
	static Matrix44 MakeRotationDegreesAroundY(float rotationDegrees);
	static Matrix44 MakeRotationDegreesAroundX(float rotationDegrees);
	static Matrix44 MakeRotationDegrees2D( float rotationDegreesAboutZ );
	static Matrix44 MakeTranslation2D( const Vector2& translation );
	static Matrix44 MakeTranslation3D( const Vector3& translation );
	static Matrix44 MakeScaleUniform2D( float scaleXY );
	static Matrix44 MakeScale2D( float scaleX, float scaleY );
	static Matrix44 MakeOrtho2D( const Vector2& mins, const Vector2& maxs );
	static Matrix44 MakeOrtho3D( const Vector3& mins, const Vector3& maxs);
	static Matrix44 LookAt(Vector3 position, Vector3 target, Vector3 up = Vector3::UP);
	static Matrix44 MakeView(Vector3 position, Vector3 target, Vector3 up = Vector3::UP);
	static Matrix44 PerspectiveProjection( float fov_degrees, float aspect, float nz, float fz );
	static Matrix44 MakeMatrix(const Vector4& iBasis, const Vector4& jBasis, const Vector4& kBasis, const Vector4& translation);

	//--------------------------------------------------------------------------
	// Rotator functions
	Matrix44 GetAsMatrix() const { return *this; }
	Matrix44 FromMatrix(const Matrix44& matrix) const { return matrix; } // for rotator
	Matrix44 FromEuler(const Vector3& euler) const;
	void RotateByEuler(const Vector3& euler); 
	Vector3 GetEuler() const;

public:
	
	// Collumn Major (squirrels way)
	// Ix	Jx	Kx	Tx
	// Iy	Jy	Ky	Ty
	// Iz	Jz	Kz	Tz
	// Iw	Jw	Kw	Tw

	float	Ix, Iy, Iz, Iw,   Jx, Jy, Jz, Jw,   Kx, Ky, Kz, Kw,   Tx, Ty, Tz, Tw; // i, j, k and translation

	

};

//====================================================================================
// Standard C
//====================================================================================
Matrix44 InvertFast(const Matrix44& matrixToInverse);
Matrix44 Invert(const Matrix44& matrixToInverse);

//--------------------------------------------------------------------------
Vector2 TransformPoint(Vector2 pos, Matrix44& mat);
Vector3 TransformPoint(Vector3 pos, Matrix44& mat);
Vector4 TransformPoint(Vector4 pos, Matrix44& mat);

Matrix44 Interpolate(const Matrix44& a, const Matrix44& b, float t);