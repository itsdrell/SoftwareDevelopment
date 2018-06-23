#pragma once
#include "Engine\Math\Matrices/Matrix44.hpp"
#include "../../Math/Quaternion.hpp"
#include "EngineCommon.hpp"


//====================================================================================
// Forward Declare
//====================================================================================


//====================================================================================
// Type Defs + Defines
//====================================================================================

// so we can easily switch between the two
typedef		Matrix44		Rotator;
//typedef	Quaternion		Rotator;

//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================
struct transform_t 
{
	transform_t()
		: position(0.0f)
		, rotation(Rotator()) // use the default constructor
		, scale(1.0f) {}

	Vector3				position; 
	Rotator				rotation;
	Vector3				scale; 

	Matrix44 GetMatrix() const; 
	void SetMatrix( Matrix44 const &mat ); 

	void SetPosition( Vector3 pos ); 
	void Translate( Vector3 offset ); 
	Vector3 GetPosition() const; 

	void SetRotationEuler( Vector3 theEuler ); 
	void RotateByEuler( Vector3 theEuler ); 
	Vector3 GetEulerAngles() const; 

	void SetScale( Vector3 s ); 
	Vector3 GetScale() const; 

	// STATICS
	static transform_t const IDENTITY; 
};

//====================================================================================
// Classes
//====================================================================================
class Transform 
{
public:

	Transform();

	//--------------------------------------------------------------------------
	// LOCAL
	// these just call through to the the member
	// transform_t 
	Matrix44 GetLocalMatrix() const; 
	void SetLocalMatrix( Matrix44 const &mat ); 

	void SetLocalPosition( Vector3 pos ); 
	void TranslateLocal( Vector3 offset ); 
	Vector3 GetLocalPosition() const; 

	void SetLocalRotationEuler( Vector3 euler ); 
	void RotateLocalByEuler( Vector3 euler ); 
	Vector3 GetLocalEulerAngles() const; 

	void SetLocalScale( Vector3 s ); 
	Vector3 GetLocalScale() const; 

	Vector3 GetLocalRight() { return GetLocalMatrix().GetRight(); }
	Vector3 GetLocalUp() { return GetLocalMatrix().GetUp(); }

	void LookAtLocal( Vector3& localPos, Vector3 localUp = Vector3::UP);


	
	//--------------------------------------------------------------------------
	// WORLD 

	Matrix44 GetWorldMatrix() const;

	Vector3 GetWorldPosition() const;

	void SetWorldMatrix(Matrix44& theMatrix);

	void LookAtWorld( Vector3& worldPos, Vector3 worldUp = Vector3::UP);


	//---------------------------------------------------------------------------
	void SimpleMoveTowardPoint(Vector3& position, float speed, float ds);

	void RotateTowards(const Transform& target, float maxDegreesToTurn);

	//--------------------------------------------------------------------------
	void SetParentTransform(Transform& parent);

	void SetIsDirty() { m_isDirty = true; }

private:
	transform_t					m_local_transform; 

	Transform*					m_parentTransform;


	bool						m_isDirty;
	
};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [6/22/2018]
//====================================================================================
