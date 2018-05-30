#pragma once
#include <string>
#include <vector>
#include "Engine/Core/EngineCommon.hpp"


//////////////////////////////////////////////////////////////////////////
// THERE ARE SOME WEIRD CIRCULAR INCLUDES DONT ADD ENGINE COMMON OR RENDER
// IN HERE
//////////////////////////////////////////////////////////////////////////
enum eRenderDataType;


//////////////////////////////////////////////////////////////////////////
struct VertexAttributeT
{
	
public:
	VertexAttributeT() { name = "END";} // null terminator
	VertexAttributeT(std::string name, eRenderDataType type, uint count, bool normalized, size_t member_offset);

public:
	std::string					name; // because we have to search for it
	eRenderDataType				type; // what? (abstract away GL from the header)
	uint						elem_count; // how many?
	bool						normalized; 
	uint						vertex_stride;
	size_t						member_offset; 
};

//////////////////////////////////////////////////////////////////////////
//A place to collect all the attributes.
class VertexLayout 
{
public:

	VertexLayout(uint stride, const VertexAttributeT data[]);

	uint						GetAttributeCount(); 
	VertexAttributeT&			GetAttribute( int const idx );  

public:
	// what are the members
	std::vector<VertexAttributeT>		m_attributes;

	// how large is the vertex in bytes
	uint m_stride; 
};

