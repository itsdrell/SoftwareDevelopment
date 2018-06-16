#pragma once
#include "Engine/Math/Matrices/Matrix44.hpp"
#include "../../Core/General/Transform.hpp"

//////////////////////////////////////////////////////////////////////////
class Mesh;
class Material;


//////////////////////////////////////////////////////////////////////////
class Renderable 
{
public:
	Renderable(){}
	~Renderable();
	
	// Get + Setters
	void SetMesh( Mesh *mesh ) { m_mesh = mesh; } 
	Mesh* GetMesh() const { return m_mesh; }

	void SetMaterial( Material *mat ) { m_material = mat; } 
	Material* GetMaterial() { return m_material; } 

	void SetModelMatrix( Matrix44 model ) { m_transform.SetWorldMatrix(model); } 
	void SetPosition(const Vector3& position);
	Matrix44 GetModelMatrix() const { return m_transform.GetWorldMatrix(); } 

	Vector3 GetPosition() { return m_transform.GetWorldPosition(); }
	bool UsesLight();


public:
	Transform		m_transform;
	Mesh*			m_mesh; 
	Material*		m_material; 

	bool			m_usesLight;
}; 