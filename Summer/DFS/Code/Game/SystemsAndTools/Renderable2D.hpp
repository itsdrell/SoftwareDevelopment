#pragma once
#include "Engine\Math\Matrices\Matrix44.hpp"
#include "Engine\Math\Vectors\Vector2.hpp"
#include "Transform2D.hpp"


//=============================================================
// Forward Declare
//=============================================================
class Sprite;
class Material;

//=============================================================
// ENUMS
//=============================================================


//=============================================================
// Structs
//=============================================================


//=============================================================
// Classes
//=============================================================
class Renderable2D
{
public:
	Renderable2D();
	~Renderable2D() {};

	// Get + Setters
	void SetSprite( Sprite* sprite) { m_sprite = sprite; }
	Sprite* GetSprite() const { return m_sprite; }

	void SetMaterial( Material *mat ) { m_material = mat; } 
	Material* GetMaterial() { return m_material; } 

	void SetParentTransform( Transform2D& parent) { m_transform.SetParentTransform(parent); } 

	// Shouldn't need these since transform watchings the object and adjusts itself
	//void SetModelMatrix( Matrix44 model ) { m_transform.SetWorldm_modelMatrix = model; } 
	//void SetPosition(const Vector2& position);
	Matrix44 GetModelMatrix() const { return m_transform.GetWorldMatrix(); } 

	Vector3 GetPosition() { return GetModelMatrix().GetPosition(); }

	int GetLayer() { return m_layer; }

private:
	Transform2D		m_transform;
	Sprite*			m_sprite;
	Material*		m_material;

public:
	int			m_layer;

	// later could add lights but for now, no
};

//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
