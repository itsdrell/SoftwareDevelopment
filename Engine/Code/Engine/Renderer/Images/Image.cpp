#include "Engine/Renderer/Images/Image.hpp"
#include "Engine/Core/General/Rgba.hpp"
#include "Engine/Renderer/external/stbi/stb_image.h"
#include "Engine/Math/MathUtils.hpp"


Image::~Image()
{

}

Image::Image(std::string filePath, bool flip)
{
	int numComponents = 0; // Filled in for us to indicate how many color/alpha components the image had (e.g. 3=RGB, 4=RGBA)
	int numComponentsRequested = 4; // don't care; we support 3 (RGB) or 4 (RGBA)
	m_path = filePath;

	// This flips my UVs which are current top left is 0,0 to bottom left being 0,0
	if(flip)
		stbi_set_flip_vertically_on_load(1);
	else
		stbi_set_flip_vertically_on_load(0);
	
	// default
	//stbi_set_flip_vertically_on_load(1);

	unsigned char* imageData = stbi_load( filePath.c_str(), &m_dimensions.x, &m_dimensions.y, &numComponents, numComponentsRequested );
	//m_colorsAsChar = stbi_load( filePath.c_str(), &m_dimensions.x, &m_dimensions.y, &numComponents, numComponentsRequested );
	PopulateColorVector(imageData,numComponentsRequested);
	//stbi_image_free( imageData );
}

Image::Image()
{

}

Image::Image(std::string name, const IntVector2& dimension, const Rgba& color)
{
	// this is creating an image with colors I give it (so no texture)
	m_dimensions = dimension;
	m_path = name;

	int amount = dimension.x * dimension.y;

	for(int i = 0; i < amount; i++)
	{
		m_colors.push_back(color);
	}
}

void Image::PopulateColorVector(unsigned char* imageData, int numComponentsRequested)
{	
	// RGBA
	if(numComponentsRequested == 4)
	{
		int amountOfRGBA = (m_dimensions.x * m_dimensions.y) * 4;
		m_colors.reserve((m_dimensions.x * m_dimensions.y));


		for(int i = 0; i < amountOfRGBA; i+=4)
		{
			unsigned char Rvalue = imageData[i];
			unsigned char Gvalue = imageData[i+1];
			unsigned char Bvalue = imageData[i+2];
			unsigned char Avalue = imageData[i+3];

			Rgba currentColor = Rgba(Rvalue,Gvalue,Bvalue,Avalue);

			m_colors.push_back(currentColor);
		}

		
	}

	// Just RGB
	if(numComponentsRequested == 3)
	{
		int amountOfRGBA = (m_dimensions.x * m_dimensions.y) * 3;
		m_colors.reserve((m_dimensions.x * m_dimensions.y));


		for(int i = 0; i < amountOfRGBA; i+=3)
		{
			unsigned char Rvalue = imageData[i];
			unsigned char Gvalue = imageData[i+1];
			unsigned char Bvalue = imageData[i+2];

			Rgba currentColor = Rgba(Rvalue,Gvalue,Bvalue);

			m_colors.push_back(currentColor);
		}

	}
	
}

IntVector2 Image::GetTilePosition(int indexToCheck)
{
	indexToCheck = ClampInt(indexToCheck,0,(int)m_colors.size());
	
	int x = indexToCheck % m_dimensions.x; // index mod width
	int y = (indexToCheck - x) / m_dimensions.x;

	return(IntVector2(x,y));
}

Rgba Image::GetColorAt(int x, int y) const
{
	int idx = y * m_dimensions.x + x;
	return m_colors.at(idx);
}

Rgba Image::GetColorAt(IntVector2 pos) const
{
	return GetColorAt(pos.x, pos.y);
}

unsigned char* Image::GetColorCharPointer() const
{
	return (unsigned char*) m_colors.data();
}

void* Image::GetBuffer(uint xOffset, uint yOffset) const
{
	//std::vector<Rgba> copy (m_colors.begin() + a, m_colors.begin() + b);
	//
	//return (void*) copy.data();

	//Rgba* startOffset = (Rgba*) m_colors.data() + (sizeof( Rgba ) * startPixel);
	//uint bytesToCopy = sizeof(Rgba) * (endPixel - startPixel);
	//
	//void* toReturn = malloc(bytesToCopy);
	//memcpy( toReturn, startOffset, bytesToCopy);
	//
	//return toReturn;\

	uint idx = m_dimensions.x * yOffset + xOffset;
	return (void*) (m_colors.data() + idx);
}

