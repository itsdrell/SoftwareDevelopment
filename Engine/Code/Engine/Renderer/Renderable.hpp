#pragma once
#include "Engine/Math/Matrix44.hpp"

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

	void SetModelMatrix( Matrix44 model ) { m_modelMatrix = model; } 
	void SetPosition(const Vector3& position);
	Matrix44 GetModelMatrix() const { return m_modelMatrix; } 

	Vector3 GetPosition() { return m_modelMatrix.GetPosition(); }
	bool UsesLight();


public:
	Matrix44		m_modelMatrix; // the thing that owns this will have the transform
	Mesh*			m_mesh; 
	Material*		m_material; 

	bool			m_usesLight;
}; 