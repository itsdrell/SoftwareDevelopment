#pragma once
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/RendererTypes.hpp"
#include "../../Math/Geometry/AABB3.hpp"

//////////////////////////////////////////////////////////////////////////
class MeshBuilder;
enum PrimitiveType;

/////////////////////////////////////////
struct DrawInstruction
{
	bool usingIndices;

	uint startIndex;
	uint elemCount;

	PrimitiveType primitiveType;

};


/////////////////////////////////////////
// Just geometry
class Mesh
{
public:
	
	Mesh();

	template< typename VERTTYPE >
	void FromBuilderForType( MeshBuilder &mb );

	void SetIndices(uint count, uint const *indices);

	// to figure out how large it is its count * vertex_stride
	//void SetVertices(uint count, void const *data); // found this obsolete since we are using PCU
	//void SetVertices(uint count, Vertex3D_PCU const * vertices);

	void SetDrawInstruction(PrimitiveType type, bool usesIndices, uint startIndex, uint elementCount);
	void SetDrawInstruction(DrawInstruction theDraw);

	void DrawAsWireFrame() { m_drawInstruction.primitiveType = PRIMITIVE_LINES; }

	uint GetVertexStride() const { return m_layout->m_stride; }
	


	//////////////////////////////////////////////////////////////////////////
	template <typename VERTTYPE>
	void SetVertices( uint vcount, VERTTYPE const *vertices ) 
	{
		m_vbo.CopyToGPU( vcount * sizeof(VERTTYPE), vertices ); 
		m_layout = &VERTTYPE::LAYOUT; 
		// this is a form of duck-typing.  Works for any class that has a 
		// LAYOUT defined that is a VertexLayout;
	}

public:

	VertexBuffer 			m_vbo;
	IndexBuffer 			m_ibo;
	DrawInstruction 		m_drawInstruction;

	VertexLayout*			m_layout; // describes the vertices in the mesh

	AABB3					m_bounds;


};

//////////////////////////////////////////////////////////////////////////
template< typename VERTTYPE >
void Mesh::FromBuilderForType(MeshBuilder &mb)
{
	uint vcount = (uint) mb.m_vertices.size(); 
	VERTTYPE *temp = (VERTTYPE*)malloc( sizeof(VERTTYPE) * vcount ); 
	
	for (uint i = 0; i < vcount; ++i) 
	{
		// copy each vertex
		temp[i] = VERTTYPE( mb.GetVertex(i) ); 
	}
	
	//SetVertices<VERTTYPE>( vcount, temp );
	SetVertices(vcount,temp);
	
	// update indices as normal;
	if(mb.m_draw.usingIndices)
		SetIndices((uint) mb.m_indices.size(), mb.m_indices.data());
	

	SetDrawInstruction(mb.m_draw);

	// free our temp buffer
	free( temp ); 
}


