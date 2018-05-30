#pragma once
#include "Engine/Renderer/glfunctions.h"
#include "Engine/Core/EngineCommon.hpp"


class RenderBuffer
{
public:
	RenderBuffer();   // initialize data
	~RenderBuffer();  // cleanup OpenGL resource 

					  // copies data to the GPU
	bool CopyToGPU( size_t const byte_count, void const *data ); 

public:
	GLuint handle;       // OpenGL handle to the GPU buffer, defualt = NULL; 
	size_t buffer_size;  // how many bytes are in this buffer, default = 0
};



//////////////////////////////////////////////////////////////////////////
class VertexBuffer : public RenderBuffer
{
public:
	VertexBuffer() {}
	VertexBuffer(uint vertexCount, uint vertexStride);

public:
	// meta
	uint m_vertexCount;  
	uint m_vertexStride; 
};



//////////////////////////////////////////////////////////////////////////
class IndexBuffer : public RenderBuffer
{
public:
	IndexBuffer() {}
	IndexBuffer(uint indexCount, uint indexStride);

public:
	uint m_indexCount; 
	uint m_indexStride; 
};