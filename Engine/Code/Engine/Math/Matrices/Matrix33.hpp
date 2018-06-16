#pragma once
#include "Engine/Core/General/EngineCommon.hpp"


class Matrix33
{
public:

	Matrix33();
	Matrix33(Vector3& orientation);
	Matrix33(Vector3& i, Vector3& j, Vector3& k);

	float GetValueAt(uint a, uint b);
	std::vector<float> GetValues();

	void SetBasis(const Vector3& i, const Vector3& j, const Vector3& k);


	static Matrix33 LookAt(const Vector3& theVector);

public:
	float	Ix, Iy, Iz,   Jx, Jy, Jz,   Kx, Ky, Kz;
};


Vector3 EulerFromMatrix(Matrix33& theMatrix);


// Collumn Major (squirrels way)
// Ix	Jx	Kx	
// Iy	Jy	Ky	
// Iz	Jz	Kz
