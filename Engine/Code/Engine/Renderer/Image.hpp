#pragma once
#include "Engine/Math/Vectors/IntVector2.hpp"
#include "../Core/General/EngineCommon.hpp"

class Rgba;

class Image
{
public:
	~Image();
	Image();
	Image(std::string filePath, bool flip = true);
	void PopulateColorVector(unsigned char* imageData, int numComponentsRequested);

	IntVector2 GetTilePosition(int indexToCheck);
	Rgba GetColorAt(int x, int y) const;
	unsigned char* GetColorCharPointer() const;
	
	void* GetBuffer(uint xOffset, uint yOffset) const;

	uint GetWidth() const { return (uint) m_dimensions.x; }
	uint GetHeight() const { return (uint) m_dimensions.y; }


public:
	IntVector2						m_dimensions;
	std::vector<Rgba>				m_colors;
	std::string						m_path;
};