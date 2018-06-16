#include "Matrix44.hpp"
#include "Engine/Math/MathUtils.hpp"


Matrix44::Matrix44(const float* sixteenValuesBasisMajor)
{
	Ix = sixteenValuesBasisMajor[0];
	Iy = sixteenValuesBasisMajor[1];
	Iz = sixteenValuesBasisMajor[2];
	Iw = sixteenValuesBasisMajor[3];

	Jx= sixteenValuesBasisMajor[4];
	Jy= sixteenValuesBasisMajor[5];
	Jz= sixteenValuesBasisMajor[6];
	Jw= sixteenValuesBasisMajor[7];

	Kx = sixteenValuesBasisMajor[8];
	Ky = sixteenValuesBasisMajor[9];
	Kz = sixteenValuesBasisMajor[10];
	Kw = sixteenValuesBasisMajor[11];

	Tx = sixteenValuesBasisMajor[12];
	Ty = sixteenValuesBasisMajor[13];
	Tz = sixteenValuesBasisMajor[14];
	Tw = sixteenValuesBasisMajor[15];
}

Matrix44::Matrix44(const Vector2& iBasis, const Vector2 & jBasis, const Vector2 & translation)
{
	// Start with identity
	this->SetIdentity();

	// then set the basis
	Ix = iBasis.x;
	Iy = iBasis.y;

	Jx = jBasis.x;
	Jy = jBasis.y;

	Tx = translation.x;
	Ty = translation.y;
}

Matrix44::Matrix44()
{
	this->SetIdentity();
}

Matrix44::Matrix44(const Matrix44& copyOf)
	:Ix (copyOf.Ix)
	,Iy (copyOf.Iy)
	,Iz (copyOf.Iz)
	,Iw (copyOf.Iw)
	
	,Jx (copyOf.Jx)
	,Jy (copyOf.Jy)
	,Jz (copyOf.Jz)
	,Jw (copyOf.Jw)
	
	,Kx (copyOf.Kx)
	,Ky (copyOf.Ky)
	,Kz (copyOf.Kz)
	,Kw (copyOf.Kw)
	
	,Tx (copyOf.Tx)
	,Ty (copyOf.Ty)
	,Tz (copyOf.Tz)
	,Tw (copyOf.Tw)
{
}

Matrix44::Matrix44(const Vector3& iBasis, const Vector3& jBasis, const Vector3& kBasis, const Vector3& translation /*= Vector3::ZERO*/)
{
	this->SetIdentity();

	Ix = iBasis.x;
	Iy = iBasis.y;
	Iz = iBasis.z;

	Jx = jBasis.x;
	Jy = jBasis.y;
	Jz = jBasis.z;

	Kx = kBasis.x;
	Ky = kBasis.y;
	Kz = kBasis.z;

	Tx = translation.x;
	Ty = translation.y;
	Tz = translation.z;
}

Matrix44::Matrix44(const Vector4& iBasis, const Vector4& jBasis, const Vector4& kBasis, const Vector4& translation)
{
	Ix = iBasis.x;
	Iy = iBasis.y;
	Iz = iBasis.z;
	Iw = iBasis.w;

	Jx= jBasis.x;
	Jy= jBasis.y;
	Jz= jBasis.z;
	Jw= jBasis.w;

	Kx = kBasis.x;
	Ky = kBasis.y;
	Kz = kBasis.z;
	Kw = kBasis.w;

	Tx = translation.x;
	Ty = translation.y;
	Tz = translation.z;
	Tw = translation.w;
}

Matrix44::Matrix44(const Matrix33& a)
{
	Ix = a.Ix;
	Iy = a.Iy;
	Iz = a.Iz;
	Iw = 0.f;

	Jx= a.Jx;
	Jy= a.Jy;
	Jz= a.Jz;
	Jw= 0.f;

	Kx = a.Kx;
	Ky = a.Ky;
	Kz = a.Kz;
	Kw = 0.f;

	Tx = 0.f;
	Ty = 0.f;
	Tz = 0.f;
	Tw = 0.f;
}

Vector2 Matrix44::TransformPosition2D(const Vector2& position2D) const
{
	float newX = (Ix * position2D.x) + (Jx * position2D.y) + Tx;
	float newY = (Iy * position2D.x) + (Jy * position2D.y) + Ty;

	return Vector2(newX,newY);
}

Vector3 Matrix44::TransformPosition3D(const Vector3 & position) const
{
	float newX = (Ix * position.x) + (Jx * position.y) + (Kx * position.z) + Tx;
	float newY = (Iy * position.x) + (Jy * position.y) + (Ky * position.z) + Ty;
	float newZ = (Iz * position.x) + (Jz * position.y) + (Kz * position.z) + Tz;

	return Vector3(newX, newY, newZ);
}

Vector4 Matrix44::TransformHomogeneous(const Vector4 & position) const
{
	float newX = (Ix * position.x) + (Jx * position.y) + (Kx * position.z) + (Tx * position.w);
	float newY = (Iy * position.x) + (Jy * position.y) + (Ky * position.z) + (Ty * position.w);
	float newZ = (Iz * position.x) + (Jz * position.y) + (Kz * position.z) + (Tz * position.w);
	float newW = (Iw * position.x) + (Jw * position.y) + (Kw * position.z) + (Tw * position.w);
		
	return Vector4(newX, newY, newZ, newW);
}

Vector2 Matrix44::TransformDisplacement2D(const Vector2& displacement2D) const //direction
{
	float newX = (Ix * displacement2D.x) + (Jx * displacement2D.y);
	float newY = (Iy * displacement2D.x) + (Jy * displacement2D.y);

	return Vector2(newX,newY);
}

Vector3 Matrix44::GetForward() const
{
	return Vector3(Kx,Ky,Kz);
}

Vector3 Matrix44::GetRight() const
{
	return Vector3(Ix,Iy,Iz);
}

Vector3 Matrix44::GetUp() const
{
	return Vector3(Jx,Jy,Jz);
}

Vector3 Matrix44::GetPosition() const
{
	return Vector3(Tx,Ty,Tz);
}

Vector3 Matrix44::GetXRow() const
{
	return Vector3(Ix, Jx, Kx);
}

Vector3 Matrix44::GetYRow() const
{
	return Vector3(Iy, Jy, Ky);
}

Vector3 Matrix44::GetZRow() const
{
	return Vector3(Iz, Jz, Kz);
}

Matrix33 Matrix44::GetAsMatrix33() const
{
	return Matrix33(GetRight(), GetUp(), GetForward());
}

void Matrix44::GetValuesAsArray(double *outArray)
{
	
	outArray[0] =	(double)Ix;
	outArray[1] =	(double)Iy;
	outArray[2] =	(double)Iz;
	outArray[3] =	(double)Iw;
	
	outArray[4] =	(double)Jx;
	outArray[5] =	(double)Jy;
	outArray[6] =	(double)Jz;
	outArray[7] =	(double)Jw;
	
	outArray[8] =	(double)Kx;
	outArray[9] =	(double)Ky;
	outArray[10] =	(double)Kz;
	outArray[11] =	(double)Kw;
	
	outArray[12] =	(double)Tx;
	outArray[13] =	(double)Ty;
	outArray[14] =	(double)Tz;
	outArray[15] =	(double)Tw; 
	
}

void Matrix44::GetValuesAsArray(float* outArray)
{
	outArray[0] =	Ix;
	outArray[1] =	Iy;
	outArray[2] =	Iz;
	outArray[3] =	Iw;

	outArray[4] =	Jx;
	outArray[5] =	Jy;
	outArray[6] =	Jz;
	outArray[7] =	Jw;

	outArray[8] =	Kx;
	outArray[9] =	Ky;
	outArray[10] =	Kz;
	outArray[11] =	Kw;

	outArray[12] =	Tx;
	outArray[13] =	Ty;
	outArray[14] =	Tz;
	outArray[15] =	Tw; 
}

void Matrix44::operator=(const Matrix44& copyOf)
{
	Ix = copyOf.Ix;
	Iy = copyOf.Iy;
	Iz = copyOf.Iz;
	Iw = copyOf.Iw;
	   		   
	Jx = copyOf.Jx;
	Jy = copyOf.Jy;
	Jz = copyOf.Jz;
	Jw = copyOf.Jw;
	    	   
	Kx = copyOf.Kx;
	Ky = copyOf.Ky;
	Kz = copyOf.Kz;
	Kw = copyOf.Kw;
	   		   
	Tx = copyOf.Tx;
	Ty = copyOf.Ty;
	Tz = copyOf.Tz;
	Tw = copyOf.Tw;
}

Matrix44 Matrix44::operator*(Matrix44& theMatrix)
{
	Matrix44 thisMatrix = *this;

	theMatrix.Append(theMatrix);

	return thisMatrix;
}

void Matrix44::SetIdentity()
{
	// Set the diagonals
	Ix = Jy = Kz = Tw = 1.0f;

	// Set the rest to 0
	Iy = Iz = Iw = 0.0f;
	Jx = Jz = Jw = 0.0f;
	Kx = Ky = Kw = 0.0f;
	Tx = Ty = Tz = 0.0f;

}

void Matrix44::SetValues(const float* sixteenValuesBasisMajor)
{
	Ix = sixteenValuesBasisMajor[0];
	Iy = sixteenValuesBasisMajor[1];
	Iz = sixteenValuesBasisMajor[2];
	Iw = sixteenValuesBasisMajor[3];

	Jx= sixteenValuesBasisMajor[4];
	Jy= sixteenValuesBasisMajor[5];
	Jz= sixteenValuesBasisMajor[6];
	Jw= sixteenValuesBasisMajor[7];

	Kx = sixteenValuesBasisMajor[8];
	Ky = sixteenValuesBasisMajor[9];
	Kz = sixteenValuesBasisMajor[10];
	Kw = sixteenValuesBasisMajor[11];

	Tx = sixteenValuesBasisMajor[12];
	Ty = sixteenValuesBasisMajor[13];
	Tz = sixteenValuesBasisMajor[14];
	Tw = sixteenValuesBasisMajor[15];
}

void Matrix44::SetTranslation(const Vector3& theTranslation)
{
	Tx = theTranslation.x;
	Ty = theTranslation.y;
	Tz = theTranslation.z;
}

void Matrix44::Append(const Matrix44& matrixToAppend)
{
	Matrix44 old = Matrix44(&Ix); // Use the constructor that takes the memory pointer

	Ix = (old.Ix * matrixToAppend.Ix) + (old.Jx * matrixToAppend.Iy) + (old.Kx * matrixToAppend.Iz) + (old.Tx * matrixToAppend.Iw);
	Iy = (old.Iy * matrixToAppend.Ix) + (old.Jy * matrixToAppend.Iy) + (old.Ky * matrixToAppend.Iz) + (old.Ty * matrixToAppend.Iw);
	Iz = (old.Iz * matrixToAppend.Ix) + (old.Jz * matrixToAppend.Iy) + (old.Kz * matrixToAppend.Iz) + (old.Tz * matrixToAppend.Iw);
	Iw = (old.Iw * matrixToAppend.Ix) + (old.Jw * matrixToAppend.Iy) + (old.Kw * matrixToAppend.Iz) + (old.Tw * matrixToAppend.Iw);

	Jx = (old.Ix * matrixToAppend.Jx) + (old.Jx * matrixToAppend.Jy) + (old.Kx * matrixToAppend.Jz) + (old.Tx * matrixToAppend.Jw);
	Jy = (old.Iy * matrixToAppend.Jx) + (old.Jy * matrixToAppend.Jy) + (old.Ky * matrixToAppend.Jz) + (old.Ty * matrixToAppend.Jw);
	Jz = (old.Iz * matrixToAppend.Jx) + (old.Jz * matrixToAppend.Jy) + (old.Kz * matrixToAppend.Jz) + (old.Tz * matrixToAppend.Jw);
	Jw = (old.Iw * matrixToAppend.Jx) + (old.Jw * matrixToAppend.Jy) + (old.Kw * matrixToAppend.Jz) + (old.Tw * matrixToAppend.Jw);

	Kx = (old.Ix * matrixToAppend.Kx) + (old.Jx * matrixToAppend.Ky) + (old.Kx * matrixToAppend.Kz) + (old.Tx * matrixToAppend.Kw);
	Ky = (old.Iy * matrixToAppend.Kx) + (old.Jy * matrixToAppend.Ky) + (old.Ky * matrixToAppend.Kz) + (old.Ty * matrixToAppend.Kw);
	Kz = (old.Iz * matrixToAppend.Kx) + (old.Jz * matrixToAppend.Ky) + (old.Kz * matrixToAppend.Kz) + (old.Tz * matrixToAppend.Kw);
	Kw = (old.Iw * matrixToAppend.Kx) + (old.Jw * matrixToAppend.Ky) + (old.Kw * matrixToAppend.Kz) + (old.Tw * matrixToAppend.Kw);

	Tx = (old.Ix * matrixToAppend.Tx) + (old.Jx * matrixToAppend.Ty) + (old.Kx * matrixToAppend.Tz) + (old.Tx * matrixToAppend.Tw);
	Ty = (old.Iy * matrixToAppend.Tx) + (old.Jy * matrixToAppend.Ty) + (old.Ky * matrixToAppend.Tz) + (old.Ty * matrixToAppend.Tw);
	Tz = (old.Iz * matrixToAppend.Tx) + (old.Jz * matrixToAppend.Ty) + (old.Kz * matrixToAppend.Tz) + (old.Tz * matrixToAppend.Tw);
	Tw = (old.Iw * matrixToAppend.Tx) + (old.Jw * matrixToAppend.Ty) + (old.Kw * matrixToAppend.Tz) + (old.Tw * matrixToAppend.Tw);

}

void Matrix44::RotateDegrees2D(float rotationDegreesAboutZ)
{

	Matrix44 rotation = MakeRotationDegrees2D(rotationDegreesAboutZ);
	Append(rotation);
}

void Matrix44::Translate2D(const Vector2& translation)
{
	Matrix44 translationMatrix = MakeTranslation2D(translation);
	Append(translationMatrix);

}

void Matrix44::Translate3D(const Vector3& translation)
{
	Matrix44 translationMatrix = MakeTranslation3D(translation);
	Append(translationMatrix);
}

void Matrix44::ScaleUniform2D(float scaleXY)
{
	Matrix44 scale = MakeScaleUniform2D(scaleXY);
	Append(scale);

}

void Matrix44::Scale2D(float scaleX, float scaleY)
{

	Matrix44 scale = MakeScale2D(scaleX,scaleY);
	Append(scale);

}

void Matrix44::Transpose()
{
	Matrix44 matrixToInverse = Matrix44(*this);
	
	Iy = matrixToInverse.Jx;
	Iz = matrixToInverse.Kx;


	Jx = matrixToInverse.Iy;
	Jz = matrixToInverse.Ky;


	Kx = matrixToInverse.Iz;
	Ky = matrixToInverse.Jz;
}

Matrix44 Matrix44::MakeRotationDegreesAroundZ(float rotationDegrees)
{
	return MakeRotationDegrees2D(rotationDegrees);
}

Matrix44 Matrix44::MakeRotationDegreesAroundY(float rotationDegrees)
{
	//https://en.wikipedia.org/wiki/Rotation_matrix#In_three_dimensions
	
	Matrix44 result;

	float cosValue = CosDegrees(rotationDegrees);
	float sinValue = SinDegrees(rotationDegrees);

	result.Ix = cosValue;
	result.Iz = -sinValue;
	result.Kx = sinValue;
	result.Kz = cosValue;

	return result;
}

Matrix44 Matrix44::MakeRotationDegreesAroundX(float rotationDegrees)
{
	// https://en.wikipedia.org/wiki/Rotation_matrix#In_three_dimensions
	Matrix44 result;

	float cosValue = CosDegrees(rotationDegrees);
	float sinValue = SinDegrees(rotationDegrees);

	result.Jy = cosValue;
	result.Jz = sinValue;
	result.Ky = -sinValue;
	result.Kz = cosValue;

	return result;
}

Matrix44 Matrix44::MakeRotationDegrees2D(float rotationDegreesAboutZ)
{
	Matrix44 result;

	//result.RotateDegrees2D(rotationDegreesAboutZ);

	float cosValue = CosDegrees(rotationDegreesAboutZ);
	float sinValue = SinDegrees(rotationDegreesAboutZ);

	result.Ix = cosValue;
	result.Iy = sinValue;
	result.Jx = -sinValue;
	result.Jy = cosValue;

	return result;
}

Matrix44 Matrix44::MakeTranslation2D(const Vector2& translation)
{
	Matrix44 result;

	result.Tx = translation.x;
	result.Ty = translation.y;

	return result;
}

Matrix44 Matrix44::MakeTranslation3D(const Vector3& translation)
{
	Matrix44 result;

	result.Tx = translation.x;
	result.Ty = translation.y;
	result.Tz = translation.z;

	return result;
}

Matrix44 Matrix44::MakeScaleUniform2D(float scaleXY)
{
	Matrix44 result;
	
	result.Ix = scaleXY;
	result.Jy = scaleXY;
	
	return result;
}

Matrix44 Matrix44::MakeScale2D(float scaleX, float scaleY)
{
	Matrix44 result;

	result.Ix = scaleX;
	result.Jy = scaleY;

	return result;
}

Matrix44 Matrix44::MakeOrtho2D(const Vector2& mins, const Vector2& maxs)
{
	Matrix44 result = Matrix44(); // Get Identity 

	// Setting the diagonal
	result.Ix = (2)/(maxs.x - mins.x);
	result.Jy = (2)/(maxs.y - mins.y);
	//result.Kz = (2)/(maxs.z - mins.z);

	// Set the T column
	result.Tx = -(maxs.x + mins.x) / (maxs.x - mins.x);
	result.Ty = -(maxs.y + mins.y) / (maxs.y - mins.y);
	//result.Tz = -(maxs.z + mins.z) / (maxs.z - mins.z);
	result.Tw = 1.f;

	// The rest are all zeros

	return result;
}

Matrix44 Matrix44::MakeOrtho3D(const Vector3& mins, const Vector3& maxs)
{
	Matrix44 result = Matrix44(); // Get Identity 

							// Setting the diagonal
	result.Ix = (2)/(maxs.x - mins.x);
	result.Jy = (2)/(maxs.y - mins.y);
	result.Kz = (2)/(maxs.z - mins.z);

	// Set the T column
	result.Tx = -(maxs.x + mins.x) / (maxs.x - mins.x);
	result.Ty = -(maxs.y + mins.y) / (maxs.y - mins.y);
	result.Tz = -(maxs.z + mins.z) / (maxs.z - mins.z);
	result.Tw = 1.f;

	// The rest are all zeros

	return result;
}


Matrix44 Matrix44::LookAt(Vector3 position, Vector3 target, Vector3 up /*= Vector3::UP*/)
{

	// What forseth showed us in class
	Vector3 direction = target - position;
	Vector3 forward = direction.Normalize();
	Vector3 right = Cross(up,forward);
	Vector3 normalizeRight = right.Normalize();
	Vector3 newUp = Cross(forward,normalizeRight);

	// Create a matrix
	return Matrix44 (normalizeRight, newUp, forward, position);
}

// This does a look at and an inverse
Matrix44 Matrix44::MakeView(Vector3 position, Vector3 target, Vector3 up /*= Vector3::UP*/)
{
	//	Formula (THIS MAY BE A RIGHT HANDED BASIS)
	//	https://msdn.microsoft.com/en-us/library/windows/desktop/bb281710(v=vs.85).aspx
	// 		zaxis = normal(cameraTarget - cameraPosition)
	// 		xaxis = normal(cross(cameraUpVector, zaxis))
	// 		yaxis = cross(zaxis, xaxis)
	// 
	// 		xaxis.x           yaxis.x           zaxis.x          0
	// 		xaxis.y           yaxis.y           zaxis.y          0
	// 		xaxis.z           yaxis.z           zaxis.z          0
	// 		-dot(xaxis, cameraPosition)  -dot(yaxis, cameraPosition)  -dot(zaxis, cameraPosition)  1

	Matrix44 view;
	
	Vector3 z = target - position;
	Vector3 zaxis = Normalize(z);
	Vector3 cross = Cross(up,zaxis);
	Vector3 xaxis = Normalize(cross);
	Vector3 yaxis = Cross(zaxis,xaxis);

	view.Ix = xaxis.x;
	view.Iy = yaxis.x;
	view.Iz = zaxis.x;
	view.Iw = 0;
	
	view.Jx = xaxis.y;
	view.Jy = yaxis.y;
	view.Jz = zaxis.y;
	view.Jw = 0;

	view.Kx = xaxis.z;
	view.Ky = yaxis.z;
	view.Kz = zaxis.z;
	view.Kw = 0;
	
	view.Tx = -DotProduct(xaxis,position);
	view.Ty = -DotProduct(yaxis,position);
	view.Tz = -DotProduct(zaxis,position);
	view.Tw = 1;


	return view;
}

Matrix44 Matrix44::PerspectiveProjection(float fov_degrees, float aspect, float nz, float fz)
{
	float d = 1.0f / tanf(ConvertDegreesToRadians(fov_degrees));
	float q = 1.0f / (fz - nz); 

	Vector4 i = Vector4( d / aspect, 0,    0,						0 );  
	Vector4 j = Vector4( 0,          d,    0,						0 ); 
	Vector4 k = Vector4( 0,          0,    (nz + fz) * q,			1.f);
	Vector4 t = Vector4( 0,          0,    -2.0f * nz * fz * q ,	0 ); 

	Matrix44 result = Matrix44( i, j, k, t ); 

	return result;
}

Matrix44 Matrix44::MakeMatrix(const Vector4& iBasis, const Vector4& jBasis, const Vector4& kBasis, const Vector4& translation)
{
	Matrix44 test;
	
	test.Ix = iBasis.x;
	test.Iy = iBasis.y;
	test.Iz = iBasis.z;
	test.Iw = iBasis.w;

	test.Jx= jBasis.x;
	test.Jy= jBasis.y;
	test.Jz= jBasis.z;
	test.Jw= jBasis.w;
	
	test.Kx = kBasis.x;
	test.Ky = kBasis.y;
	test.Kz = kBasis.z;
	test.Kw = kBasis.w;
	
	test.Tx = translation.x;
	test.Ty = translation.y;
	test.Tz = translation.z;
	test.Tw = translation.w;

	return test;
}


//------------------------------------------------------------------------
// Forseth Lifted from GLU
void Matrix44::Invert()
{
	float data[16];
	GetValuesAsArray(data);
	
	float inv[16];
	float det;
	float m[16];
	uint i;

	for (i = 0; i < 16; ++i) {
		m[i] = data[i];
	}

	inv[0] = m[5]  * m[10] * m[15] - 
		m[5]  * m[11] * m[14] - 
		m[9]  * m[6]  * m[15] + 
		m[9]  * m[7]  * m[14] +
		m[13] * m[6]  * m[11] - 
		m[13] * m[7]  * m[10];

	inv[4] = -m[4]  * m[10] * m[15] + 
		m[4]  * m[11] * m[14] + 
		m[8]  * m[6]  * m[15] - 
		m[8]  * m[7]  * m[14] - 
		m[12] * m[6]  * m[11] + 
		m[12] * m[7]  * m[10];

	inv[8] = m[4]  * m[9] * m[15] - 
		m[4]  * m[11] * m[13] - 
		m[8]  * m[5] * m[15] + 
		m[8]  * m[7] * m[13] + 
		m[12] * m[5] * m[11] - 
		m[12] * m[7] * m[9];

	inv[12] = -m[4]  * m[9] * m[14] + 
		m[4]  * m[10] * m[13] +
		m[8]  * m[5] * m[14] - 
		m[8]  * m[6] * m[13] - 
		m[12] * m[5] * m[10] + 
		m[12] * m[6] * m[9];

	inv[1] = -m[1]  * m[10] * m[15] + 
		m[1]  * m[11] * m[14] + 
		m[9]  * m[2] * m[15] - 
		m[9]  * m[3] * m[14] - 
		m[13] * m[2] * m[11] + 
		m[13] * m[3] * m[10];

	inv[5] = m[0]  * m[10] * m[15] - 
		m[0]  * m[11] * m[14] - 
		m[8]  * m[2] * m[15] + 
		m[8]  * m[3] * m[14] + 
		m[12] * m[2] * m[11] - 
		m[12] * m[3] * m[10];

	inv[9] = -m[0]  * m[9] * m[15] + 
		m[0]  * m[11] * m[13] + 
		m[8]  * m[1] * m[15] - 
		m[8]  * m[3] * m[13] - 
		m[12] * m[1] * m[11] + 
		m[12] * m[3] * m[9];

	inv[13] = m[0]  * m[9] * m[14] - 
		m[0]  * m[10] * m[13] - 
		m[8]  * m[1] * m[14] + 
		m[8]  * m[2] * m[13] + 
		m[12] * m[1] * m[10] - 
		m[12] * m[2] * m[9];

	inv[2] = m[1]  * m[6] * m[15] - 
		m[1]  * m[7] * m[14] - 
		m[5]  * m[2] * m[15] + 
		m[5]  * m[3] * m[14] + 
		m[13] * m[2] * m[7] - 
		m[13] * m[3] * m[6];

	inv[6] = -m[0]  * m[6] * m[15] + 
		m[0]  * m[7] * m[14] + 
		m[4]  * m[2] * m[15] - 
		m[4]  * m[3] * m[14] - 
		m[12] * m[2] * m[7] + 
		m[12] * m[3] * m[6];

	inv[10] = m[0]  * m[5] * m[15] - 
		m[0]  * m[7] * m[13] - 
		m[4]  * m[1] * m[15] + 
		m[4]  * m[3] * m[13] + 
		m[12] * m[1] * m[7] - 
		m[12] * m[3] * m[5];

	inv[14] = -m[0]  * m[5] * m[14] + 
		m[0]  * m[6] * m[13] + 
		m[4]  * m[1] * m[14] - 
		m[4]  * m[2] * m[13] - 
		m[12] * m[1] * m[6] + 
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] + 
		m[1] * m[7] * m[10] + 
		m[5] * m[2] * m[11] - 
		m[5] * m[3] * m[10] - 
		m[9] * m[2] * m[7] + 
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] - 
		m[0] * m[7] * m[10] - 
		m[4] * m[2] * m[11] + 
		m[4] * m[3] * m[10] + 
		m[8] * m[2] * m[7] - 
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] + 
		m[0] * m[7] * m[9] + 
		m[4] * m[1] * m[11] - 
		m[4] * m[3] * m[9] - 
		m[8] * m[1] * m[7] + 
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] - 
		m[0] * m[6] * m[9] - 
		m[4] * m[1] * m[10] + 
		m[4] * m[2] * m[9] + 
		m[8] * m[1] * m[6] - 
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
	
	// if this is 0 it wont work meaning the matrix cant be solved
	det = 1.0f / det;

	for (i = 0; i < 16; i++) {
		data[i] = (float)(inv[i] * det);
	}

	SetValues(data);
}

void Matrix44::InvertOrthonormal()
{	
	Vector3 translation = GetPosition();
	SetTranslation( Vector3(0.0f, 0.0f, 0.0f) );
	
	Transpose();

	// get the reverse translation in respect to the inverted rotation
// 	Vector4 reverseTranslation = TransformHomogeneous(Vector4(-translation, 0.0f)); // was (vec4(-translation, 0.0f) * (*this)).xyz()
// 	translation = reverseTranslation.xyz();
// 	SetTranslation( translation );

	SetTranslation(Vector3(
		DotProduct(-translation,GetXRow()),
		DotProduct(-translation, GetYRow()),
		DotProduct(-translation, GetZRow())));

	
}

// Collumn Major (squirrels way)
// Ix	Jx	Kx	Tx
// Iy	Jy	Ky	Ty
// Iz	Jz	Kz	Tz
// Iw	Jw	Kw	Tw




//////////////////////////////////////////////////////////////////////////
Matrix44 InvertFast(const Matrix44& matrixToInverse)
{
	// LEGACY CODE
	
	// This is called transposing a matrix. 
	//It's where a matrix swaps its rows and columns
	Matrix44 result = matrixToInverse;
	result.InvertOrthonormal();
	return result;

// 	Matrix44 transposed = matrixToInverse;
// 
// 	transposed.Iy = matrixToInverse.Jx;
// 	transposed.Iz = matrixToInverse.Kx;
// 	
// 	
// 	transposed.Jx = matrixToInverse.Iy;
// 	transposed.Jz = matrixToInverse.Ky;
// 	
// 	
// 	transposed.Kx = matrixToInverse.Iz;
// 	transposed.Ky = matrixToInverse.Jz;
// 
// 	
// 	// Inverse the transform
// // 	transposed.Tx = -1.f * matrixToInverse.Tx;
// // 	transposed.Ty = -1.f * matrixToInverse.Ty;
// // 	transposed.Tz = -1.f * matrixToInverse.Tz;
// // 	transposed.Tw = 1.f;
// 
// 	return transposed;
}

Matrix44 Invert(const Matrix44& matrixToInverse)
{
	Matrix44 result = matrixToInverse;
	result.Invert();
	
	return result;
}

Vector2 TransformPoint(Vector2 & pos, Matrix44 & mat)
{
	return mat.TransformPosition2D(pos);
}

Vector3 TransformPoint(Vector3& pos, Matrix44& mat)
{
	return mat.TransformPosition3D(pos);
}

Vector4 TransformPoint(Vector4 & pos, Matrix44 & mat)
{
	return mat.TransformHomogeneous(pos);
}
