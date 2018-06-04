#pragma once
#include "Engine/Core/General/EngineCommon.hpp"


class Matrix33
{
public:

	Matrix33();


	float GetValueAt(uint a, uint b) const;
	void SetBasis(const Vector3& a, const Vector3& b, const Vector3& c);


	static Matrix33 LookAt(const Vector3& theVector);

};


Vector3 EulerFromMatrix(const Matrix33& theMatrix);


// Collumn Major (squirrels way)
// Ix	Jx	Kx	
// Iy	Jy	Ky	
// Iz	Jz	Kz
