#pragma once
#include <vector>
#include "Engine/Renderer/RenderableComponents/Mesh.hpp"
#include "Engine\Math\Geometry/AABB2.hpp"
#include "Engine\Math\Geometry/AABB3.hpp"


//=============================================================
class Sprite;

//////////////////////////////////////////////////////////////////////////
class MeshBuilder
{
public:

	MeshBuilder();

	void Begin(PrimitiveType theType, bool useIndices);
	void End(); 
	void Clear();


	void SetColor(Rgba const &c);
	void SetUV(Vector2 const &uv);
	void SetUV(float x, float y);
	void SetNormal(const Vector3& theNormal);
	void SetTangents(const Vector3& theTangents);
	void SetBitangents(const Vector3& theBitangents);
	void SetNormalAndTangents(const Vector3& theNormal);
	uint PushVertex(Vector3 position);

	VertexMaster GetVertex(uint index) { return m_vertices.at(index); }
	
	// Add Face is three indices cause T R I A N G L E S
	void AddFace(uint a, uint b, uint c);
	void AddQuad(uint a, uint b, uint c, uint d);

	template< typename VERTTYPE >
	Mesh* CreateMesh();


	//////////////////////////////////////////////////////////////////////////
	// Make functions
	void AddPlane(const Vector3& center, const Vector3& dimensions, const AABB2& theUvs = AABB2(0.f, 1.f), Rgba color = Rgba::WHITE);

	void AddPlane(const Vector3& center, const Vector3& dimensions, Vector3 right , Vector3 up , Rgba color = Rgba::WHITE);

	void AddCube(const Vector3& center, const Vector3& dimensions, Rgba color = Rgba::WHITE);

	void AddSkyBox();
	
	void AddUVSphere(const Vector3& position, float radius, uint wedges, uint slices, Rgba color = Rgba::WHITE);

	void AddQuad(const Vector3& position, const AABB2& bounds, const AABB2& theUvs = AABB2(0.f, 1.f));

	void AddMeshFromObjFile(std::string path);

	void AddDeformedSphere(const Vector3& position, float radius, float offsetRange, uint wedges, uint slices, Rgba color = Rgba::WHITE);

	void AddPlaneFromFourPoints(const Vector3& bl, const Vector3& br, const Vector3& tr, const Vector3& tl);

	void AddFromSprite(const Vector2& pos, const Sprite& theSprite);

	// this is gonna be a box without the triangles cutting it up
	void Add3DBounds(const AABB3& theBounds);

	void Add2DPlane(AABB2 bounds, Rgba color = Rgba::WHITE);
	void Add2DPlane(AABB2& bounds, AABB2& uvs, const Rgba& color);
	void Add2DRandomColoredPlane( const AABB2& bounds, const AABB2& uvs);

	void Add2DText(Vector2 startPos, std::string text, float cellHeight, const Rgba& color, float aspectScale = 1.f, BitmapFont* font = nullptr);
	void Add2DRandomColoredText(Vector2 startPos, std::string text, float cellHeight, float aspectScale = 1.f, BitmapFont* font = nullptr);


	void AddFlatPlane(const Vector3& position, const AABB2& bounds, Rgba theColor = Rgba::WHITE, const AABB2& theUvs = AABB2(0.f, 1.f));

public:
	VertexMaster 						m_stamp; // this is a value. We overwrite it
	std::vector<VertexMaster>			m_vertices;
	std::vector<uint>					m_indices;

	// Values used to create the mesh at the end
	DrawInstruction						m_draw;

	AABB3								m_bounds;

};

//////////////////////////////////////////////////////////////////////////
template< typename VERTTYPE >
Mesh* MeshBuilder::CreateMesh()
{
	Mesh *mesh = new Mesh(); 

	mesh->m_bounds = m_bounds;
	
	mesh->FromBuilderForType<VERTTYPE>(*this);

	// Flush
	m_vertices.clear();
	m_indices.clear();
	m_bounds.Invalidate();

	return mesh; 
}

//////////////////////////////////////////////////////////////////////////
// These are all the helper functions for creating meshes. 


/*Mesh* CreateAABB3(const Vector3& postion, AABB3& bounds);*/