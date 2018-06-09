#pragma once
#include "Engine\Math\Matrices\Matrix44.hpp"


//=============================================================
// Forward Declare
//=============================================================
class Mesh;
class Material;
class Scene2D;
class Camera;
class Sprite;

//=============================================================
// ENUMS
//=============================================================


//=============================================================
// Structs
//=============================================================
struct DrawCall2D
{
	DrawCall2D() {}

	Matrix44	m_model;
	Material*	m_material;
	
	Mesh*		m_mesh;
	Sprite*		m_sprite;

	int			m_sort;
};

//=============================================================
// Classes
//=============================================================
class SpriteRendering
{
public:
	SpriteRendering() {}

	void Render( Scene2D* scene ) const;

	void RenderSceneForCamera(Camera* cam, Scene2D* scene) const;

	// Make a sorting thing

};

//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================