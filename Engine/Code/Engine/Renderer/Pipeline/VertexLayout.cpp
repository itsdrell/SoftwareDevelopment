#include "VertexLayout.hpp"

VertexLayout::VertexLayout(uint stride, const VertexAttributeT data[])
{
	m_stride = stride;

	uint idx = 0;
	std::string current = data[idx].name;

	while(current != "END")
	{
		m_attributes.push_back(data[idx]);

		idx++;
		current = data[idx].name;
	}
}

uint VertexLayout::GetAttributeCount()
{
	return (uint) m_attributes.size();
}

VertexAttributeT & VertexLayout::GetAttribute(int const idx)
{
	return m_attributes[idx];
}

VertexAttributeT::VertexAttributeT(std::string name, eRenderDataType type, uint count, bool normalized, size_t member_offset)
{
	this->name = name;
	this->type = type;
	this->elem_count = count;
	this->normalized = normalized;
	this->member_offset = member_offset;

	this->vertex_stride; //??????
}

