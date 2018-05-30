#pragma once
#include <vector>
#include "Mesh.hpp"
#include "..\Math\AABB2.hpp"
#include "..\Math\AABB3.hpp"


//////////////////////////////////////////////////////////////////////////
class MeshBuilder
{
public:

	MeshBuilder() {}
	void Begin(PrimitiveType theType, bool useIndices);
	void End(); 


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
	void AddPlane(const Vector3& center, const Vector3& dimensions, Rgba color = Rgba::WHITE);

	void AddPlane(const Vector3& center, const Vector3& dimensions, Vector3 right , Vector3 up , Rgba color = Rgba::WHITE);

	void AddCube(const Vector3& center, const Vector3& dimensions);

	void AddSkyBox();
	
	void AddUVSphere(const Vector3& position, float radius, uint wedges, uint slices, Rgba color = Rgba::WHITE);

	void AddQuad(const Vector3& position, AABB2& bounds);

	void AddMeshFromObjFile(std::string path);

	void AddDeformedSphere(const Vector3& position, float radius, float offsetRange, uint wedges, uint slices, Rgba color = Rgba::WHITE);


public:
	VertexMaster 						m_stamp; // this is a value. We overwrite it
	std::vector<VertexMaster>			m_vertices;
	std::vector<uint>					m_indices;

	// Values used to create the mesh at the end
	DrawInstruction						m_draw;


};

//////////////////////////////////////////////////////////////////////////
template< typename VERTTYPE >
Mesh* MeshBuilder::CreateMesh()
{
	Mesh *mesh = new Mesh(); 

	mesh->FromBuilderForType<VERTTYPE>(*this);

	// Flush
	m_vertices.clear();
	m_indices.clear();

	return mesh; 
}

//////////////////////////////////////////////////////////////////////////
// These are all the helper functions for creating meshes. 


/*Mesh* CreateAABB3(const Vector3& postion, AABB3& bounds);*/