#include "Engine/Math/Matrices/Matrix33.hpp"
#include "Matrix44.hpp"


Matrix33::Matrix33()
{
	Ix = 1.f;
	Iy = 0.f;
	Iz = 0.f;

	Jx = 0.f;
	Jy = 1.f;
	Jz = 0.f;

	Kx = 0.f;
	Ky = 0.f;
	Kz = 1.f;
}

Matrix33::Matrix33(Vector3& orientation)
{
	// heading, pitch, bank
	float sh,ch, sp,cp, sb,cb;
	
	sh = sinf(orientation.x);
	ch = cosf(orientation.x);

	sp = sinf(orientation.y);
	cp = cosf(orientation.y);

	sb = sinf(orientation.z);
	cb = cosf(orientation.z);

	// These were in column major in the book.
	Ix = ch * cb + sh * sp * sb;
	Jx = -ch * sb + sh * sp * cb;
	Kx = sh * cp;

	Iy = sb * cp;
	Jy = cb * cp;
	Ky = -sp;

	Iz = -sh * cb + ch * sp * sb;
	Jz = sb * sh + ch * sp * cb;
	Kz = ch * cp;
}

Matrix33::Matrix33(Vector3& i, Vector3& j, Vector3& k)
{
	SetBasis(i,j,k);
}

float Matrix33::GetValueAt(uint a, uint b)
{
	std::vector<float> values = GetValues();

	int idx = b * 3 + a;

	return values.at(idx);

}

std::vector<float> Matrix33::GetValues()
{
	std::vector<float> result;

	result.push_back(Ix);
	result.push_back(Iy);
	result.push_back(Iz);

	result.push_back(Jx);
	result.push_back(Jy);
	result.push_back(Jz);

	result.push_back(Kx);
	result.push_back(Ky);
	result.push_back(Kz);

	return result;
}

void Matrix33::SetBasis(const Vector3& i, const Vector3& j, const Vector3& k)
{
	Ix = i.x;
	Iy = i.y;
	Iz = i.z;

	Jx = j.x;
	Jy = j.y;
	Jz = j.z;

	Kx = k.x;
	Ky = k.y;
	Kz = k.z;
}

Matrix33 Matrix33::LookAt(const Vector3& theVector)
{
	return Matrix44::LookAt(Vector3::ZERO, theVector).GetAsMatrix33();
}

Vector3 EulerFromMatrix(Matrix33& theMatrix)
{
	// https://www.learnopencv.com/rotation-matrix-to-euler-angles/
	
	Vector3 result;

	float sy = sqrtf((theMatrix.GetValueAt(0,0) * theMatrix.GetValueAt(0,0) + theMatrix.GetValueAt(1,0), theMatrix.GetValueAt(1,0)));

	bool singular = sy < 1e-6;

	if(!singular)
	{
		result.x = atan2f(theMatrix.GetValueAt(2,1) , theMatrix.GetValueAt(2,2));
		result.y = atan2f(-theMatrix.GetValueAt(2,0), sy);
		result.z = atan2f(theMatrix.GetValueAt(1,2), theMatrix.GetValueAt(1,1));
	}
	else
	{
		result.x = atan2f(-theMatrix.GetValueAt(1,2), theMatrix.GetValueAt(1,1));
		result.y = atan2f(-theMatrix.GetValueAt(2,0), sy);
		result.z = 0;
	}

	

	return result;
}
