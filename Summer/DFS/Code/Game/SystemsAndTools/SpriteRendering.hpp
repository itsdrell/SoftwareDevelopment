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

	// Each frame we are making a mesh using sprites
	// but some drawcalls save the mesh (like map)
	// so we don't want to delete that so here is a flag
	bool		m_deleteTemporaryMesh = true;
};

//=============================================================
// Classes
//=============================================================
class SpriteRendering
{
public:
	SpriteRendering() {}
	~SpriteRendering() {}

	void Render( Scene2D* scene ) const;

	void RenderSceneForCamera(Camera* cam, Scene2D* scene) const;

	// Make a sorting thing
	void Sort(std::vector<DrawCall2D>* dc, Camera& theCam) const;

};

//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
