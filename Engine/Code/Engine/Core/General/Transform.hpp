#pragma once
#include "Engine\Math\Matrices/Matrix44.hpp"


// I do a split of struct (the SRT information)
// and the main Transform class since later on the 
// class will also support a Heirachy.  It is 
// also the class I'll do any matrix caching I may want to do on.

struct transform_t 
{
	transform_t()
		: position(0.0f)
		, euler(0.0f)
		, scale(1.0f) {}

	Vector3 position; 
	Vector3 euler; 
	Vector3 scale; 

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


//
class Transform 
{
public:

	Transform();

	//////////////////////////////////////////////////////////////////////////
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


	
	//////////////////////////////////////////////////////////////////////////
	// WORLD 

	Matrix44 GetWorldMatrix() const;

	Vector3 GetWorldPosition() const;


	//////////////////////////////////////////////////////////////////////////
	//  parent + children (D'awwww)
	void SetParentTransform(Transform& parent);
	//void AddChild(Transform& newchild);

	void SetIsDirty() { m_isDirty = true; }
	//void DirtyTheChildren();

private:
	transform_t					m_local_transform; 

	Transform*					m_parentTransform;

	//std::vector<Transform*>		m_childrenTransform;

	bool						m_isDirty;
	
};















/*
class Transform
{
public:

	Transform(const Vector3 thePosition = Vector3::ZERO, const Vector3 theRotation = Vector3::ZERO, const Vector3 theScale = Vector3::ONE);
	
	Matrix44 GetMatrix();
	
public:
	Vector3 position;
	Vector3 rotation; // euler for now, will switch to Quaternians 
	Vector3 scale;

};*/