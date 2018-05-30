#include "Mesh.hpp"
#include "MeshBuilder.hpp"

Mesh::Mesh()
{

}

void Mesh::SetIndices(uint count, uint const * indices)
{
	m_ibo.m_indexCount = count;
	m_ibo.m_indexStride = sizeof(uint);

	uint size = (m_ibo.m_indexCount * m_ibo.m_indexStride);
	
	m_ibo.CopyToGPU(size,indices);
}

// void Mesh::SetVertices(uint count, Vertex3D_PCU const * vertices)
// {
// 	m_vbo.m_vertexCount = count;
// 	m_vbo.m_vertexStride = sizeof(Vertex3D_PCU);
// 
// 	uint size = m_vbo.m_vertexCount * m_vbo.m_vertexStride;
// 
// 	m_vbo.CopyToGPU(size,vertices);
// }


void Mesh::SetDrawInstruction(PrimitiveType type, bool usesIndices, uint startIndex, uint elementCount)
{
	m_drawInstruction.primitiveType = type;
	m_drawInstruction.elemCount = elementCount;
	m_drawInstruction.startIndex = startIndex;
	m_drawInstruction.usingIndices = usesIndices;
}

void Mesh::SetDrawInstruction(DrawInstruction theDraw)
{
	m_drawInstruction = theDraw;
}

