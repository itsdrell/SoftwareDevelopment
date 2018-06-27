#pragma once
#include "Engine\Math\Matrices\Matrix44.hpp"


//=============================================================
// Forward Declare
//=============================================================


//=============================================================
// ENUMS
//=============================================================


//=============================================================
// Structs
//=============================================================
struct Transform_T
{
	Transform_T()
		: position(0.0f, 0.0f)
		, rotation(0.f)
		, scale(1.f, 1.f) {}

	//=============================================================
	Matrix44 GetMatrix() const;
	void SetMatrix( const Matrix44& theMatrix);


	void SetPosition( const Vector2& pos);
	void Translate( const Vector2& offset);
	Vector2 GetPosition() const;

	void SetRotation( float theRotation );
	float GetRotation();

	void SetScale( const Vector2& s);
	Vector2 GetScale() const;

	
	//=============================================================
	Vector2		position;
	float		rotation;
	Vector2		scale;


	static Transform_T const IDENTITY;
};

//=============================================================
// Classes
//=============================================================
class Transform2D
{
public:
	Transform2D();

	//=============================================================
	// Local
	Matrix44 GetLocalMatrix() const;
	void SetLocalMatrix( const Matrix44& mat );

	void SetLocalPosition( Vector2 pos); 
	void TranslateLocal( Vector2 offset );
	Vector2 GetLocalPosition() const;

	void SetLocalRotation( float rotation );
	float GetLocalRotation();
	
	void SetLocalScale(const Vector2& theScale);
	Vector2 GetLocalScale();

	Vector2 GetLocalRight() { return GetLocalMatrix().GetRight().xy(); }
	Vector2 GetLocalUp() { return GetLocalMatrix().GetUp().xy(); }

	//=============================================================
	// World
	 Matrix44 GetWorldMatrix() const;

	 Vector3 GetWorldPosition() const;


	 void SetParentTransform( Transform2D& parent) { m_parentTransform = &parent; }

private:
	Transform_T		m_localTransform;

	Transform2D*	m_parentTransform;

};

//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
