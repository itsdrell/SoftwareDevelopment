//-----------------------------------------------------------------------------------------------
// Texture.hpp
//
#pragma once
#include "Engine/Math/Vectors/IntVector2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <string>
#include <map>

class Image;

struct ScreenShot
{
	ScreenShot(unsigned char* data, const IntVector2& dim)
		: m_data(data)
		, m_dimensions(dim) {}
	
	unsigned char*		m_data;
	IntVector2			m_dimensions;
};

//---------------------------------------------------------------------------
class Texture
{
	friend class Renderer; // Textures are managed by a Renderer instance


private:
	// These do not use the image to create but straight from file to texture
	Texture( const std::string& imageFilePath , bool flip = true); // Use renderer->CreateOrGetTexture() instead!
	Texture();
	void PopulateFromData( unsigned char* imageData, const IntVector2& texelSize, int numComponents );
	
	uint CalculateMipCount(int maxDimension);

	// These two both use the Image class to create a texture
	Texture* CreateFromImage(Image imageToCreateFrom);
	Texture* CreateFromFile(const std::string& imageFilePath);

	bool CreateRenderTarget( int width, int height, eTextureFormat fmt );

	static Texture* CreateCompatible(Texture* theTextureToUse);

	bool CreatePNGFromTexture();
	

private:
	unsigned int								m_textureID;
	IntVector2									m_dimensions;
	eTextureFormat								m_format;

	unsigned char*								m_data;

public: 
	IntVector2 GetDimensions();
	unsigned int GetID() const {return m_textureID;}

};


void CreateScreenshotPNG(void* imageData);