#pragma once
#include "Engine\Math\Matrices\Matrix44.hpp"
#include "Engine\Math\Vectors\Vector2.hpp"
#include "Transform2D.hpp"


//=============================================================
// Forward Declare
//=============================================================
class Sprite;
class Material;
class Mesh;

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

	//#TODO Setters should delete previous things :(

	// Get + Setters
	void SetSprite( Sprite* sprite);
	Sprite* GetSprite() const { return m_sprite; }

	void SetMesh( Mesh*	mesh);
	Mesh* GetMesh() const {return m_mesh; }

	void SetMaterial( Material *mat ) { m_material = mat; } 
	Material* GetMaterial() { return m_material; } 

	void SetParentTransform( Transform2D& parent) { m_transform.SetParentTransform(parent); } 
	void SetLocalPosition(const Vector2& pos) {m_transform.SetLocalPosition(pos); }
	// Shouldn't need these since transform watchings the object and adjusts itself
	//void SetModelMatrix( Matrix44 model ) { m_transform.SetWorldm_modelMatrix = model; } 
	void SetPosition(const Vector2& position) { m_transform.SetLocalPosition(position); }
	Matrix44 GetModelMatrix() const { return m_transform.GetWorldMatrix(); } 

	Vector3 GetPosition() { return GetModelMatrix().GetPosition(); }

	void SetLayer(int theLayer) { m_layer = theLayer; }
	int GetLayer() { return m_layer; }

private:
	Transform2D		m_transform;
	Material*		m_material;
	
	//---------------------------------------------------------
	// Adding both a mesh and sprite so that if I wanted to make
	// a render able at the start that's batched I can
	Mesh*			m_mesh;
	Sprite*			m_sprite;

public:
	int				m_layer;
	bool			m_usesMesh = false;
	bool			m_hidden;

	// later could add lights but for now, no
};

//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
