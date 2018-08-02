//-----------------------------------------------------------------------------------------------
// Texture.cpp
//
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Engine/Renderer/Images/Textures/Texture.hpp"
#include "Engine/Renderer/external/stbi/stb_image.h"
#include "Engine/Core/General/EngineCommon.hpp"
#include "Engine/Renderer/Images/Image.hpp"
#include "Engine/Renderer/glfunctions.h"
#include "Engine/ThirdParty/stbi/stb_image_write.h"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Platform/Time.hpp"
#include "Engine/Async/Threading.hpp"


//-----------------------------------------------------------------------------------------------
// Called only by the Renderer.  Use renderer->CreateOrGetTexture() to instantiate textures.
//
Texture::Texture( const std::string& imageFilePath, bool flip)
	: m_textureID( 0 )
	, m_dimensions( 0, 0 )
{
	GL_CHECK_ERROR();
	
	int numComponents = 0; // Filled in for us to indicate how many color/alpha components the image had (e.g. 3=RGB, 4=RGBA)
	int numComponentsRequested = 0; // don't care; we support 3 (RGB) or 4 (RGBA)

	// This flips my UVs which are current top left is 0,0 to bottom left being 0,0
	if(flip)
		stbi_set_flip_vertically_on_load(1);
	else
		stbi_set_flip_vertically_on_load(0);

	// Load (and decompress) the image RGB(A) bytes from a file on disk, and create an OpenGL texture instance from it
	unsigned char* imageData = stbi_load( imageFilePath.c_str(), &m_dimensions.x, &m_dimensions.y, &numComponents, numComponentsRequested );
	
	PopulateFromData( imageData, m_dimensions, numComponents );
	stbi_image_free( imageData );

}


Texture::Texture()
{
	m_textureID = 0; 
	m_dimensions = IntVector2(0,0);
}

//-----------------------------------------------------------------------------------------------
// Creates a texture identity on the video card, and populates it with the given image texel data
//
void Texture::PopulateFromData( unsigned char* imageData, const IntVector2& texelSize, int numComponents )
{
	GL_CHECK_ERROR();

	m_dimensions = texelSize;

	// Enable texturing
	//glEnable( GL_TEXTURE_2D );

	// Tell OpenGL that our pixel data is single-byte aligned
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );						GL_CHECK_ERROR();

	// Ask OpenGL for an unused texName (ID number) to use for this texture
	glGenTextures( 1, (GLuint*) &m_textureID );						GL_CHECK_ERROR();

	// Tell OpenGL to bind (set) this as the currently active texture
	glBindTexture( GL_TEXTURE_2D, m_textureID );					GL_CHECK_ERROR();

	// Set texture clamp vs. wrap (repeat)
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ); // GL_CLAMP or GL_REPEAT (WAS GL_CLAMP)
	GL_CHECK_ERROR();

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ); // GL_CLAMP or GL_REPEAT
	GL_CHECK_ERROR();

	// Set magnification (texel > pixel) and minification (texel < pixel) filters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); // one of: GL_NEAREST, GL_LINEAR
	GL_CHECK_ERROR();

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR
	GL_CHECK_ERROR();

	GLenum bufferFormat = GL_RGBA8; // the format our source pixel data is in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
	if( numComponents == 3 )
		bufferFormat = GL_RGB8;

	GLenum channels = GL_RGBA;
	if(numComponents == 3)
		channels = GL_RGB;

	uint mipCount = CalculateMipCount( Max(m_dimensions.x, m_dimensions.y));

	// edge case
	if(mipCount == 0)
		mipCount = 1;

	GLenum internalFormat = bufferFormat; // the format we want the texture to be on the card; allows us to translate into a different texture format as we upload to OpenGL

	//glTexImage2D(			// Upload this pixel data to our new OpenGL texture
	//	GL_TEXTURE_2D,		// Creating this as a 2d texture
	//	0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
	//	internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
	//	m_dimensions.x,			// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,11], and B is the border thickness [0,1]
	//	m_dimensions.y,			// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,11], and B is the border thickness [0,1]
	//	0,					// Border size, in texels (must be 0 or 1, recommend 0)
	//	bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
	//	GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color channel/component)
	//	imageData );		// Address of the actual pixel data bytes/buffer in system memory
	// Copy the texture - first, get use to be using texture unit 0 for this;
	

	// create the gpu buffer
	// note: only this is needed for render targets
	glTexStorage2D( GL_TEXTURE_2D,
		mipCount,               // number of levels (mip-layers)
		internalFormat, // how is the memory stored on the GPU
		m_dimensions.x, 
		m_dimensions.y ); // dimenions
	
	GL_CHECK_ERROR();
						 // copies cpu memory to the gpu - needed for texture resources
	glTexSubImage2D( GL_TEXTURE_2D,
		0,             // mip layer we're copying to
		0, 0,          // offset
		m_dimensions.x, 
		m_dimensions.y, // dimensions
		channels,      // which channels exist in the CPU buffer
		GL_UNSIGNED_BYTE,     // how are those channels stored
		imageData ); // cpu buffer to copy;

	GL_CHECK_ERROR();

	glActiveTexture( GL_TEXTURE0 );								GL_CHECK_ERROR();
	glBindTexture( GL_TEXTURE_2D, m_textureID );				GL_CHECK_ERROR();
	glGenerateMipmap( GL_TEXTURE_2D );							GL_CHECK_ERROR();

}


uint Texture::CalculateMipCount(int maxDimension)
{
	//X = ln(maxDimension) / ln(2).

	float value = logf((float)maxDimension) / logf(2.0f);
	
	return (uint)value;
}

Texture* Texture::CreateFromImage(Image imageToCreateFrom)
{
	GL_CHECK_ERROR();
	
	m_dimensions = imageToCreateFrom.m_dimensions;

	// The image class calls the flip command so we don't have to worry about it!
	unsigned char* imageBuffer = imageToCreateFrom.GetColorCharPointer();
	PopulateFromData((unsigned char* ) imageBuffer, m_dimensions, 4 );

	GL_CHECK_ERROR();
	return this;
}

// this calls the Create From Image constructor
Texture* Texture::CreateFromFile(const std::string& imageFilePath)
{
	Image* newImage = new Image(imageFilePath);
	
	Texture* newTexture = CreateFromImage(*newImage);

	//delete newImage;

	return newTexture;
}

bool Texture::CreatePNGFromTexture()
{
	// example found here http://docs.gl/gl4/glGetTexImage
	glBindTexture( GL_TEXTURE_2D, m_textureID );    // bind our texture to our current texture unit (0)

	const int size = m_dimensions.x * m_dimensions.y * 4; //4 because texels rgba
	m_data = (unsigned char*)malloc(size);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE,m_data);
	
	ScreenShot* ss = new ScreenShot(m_data, m_dimensions);
	ThreadCreateAndDetach((thread_cb) CreateScreenshotPNG, (void*)ss);

	//////////////////////////////////////////////////////////////////////////
	// Create the png
	//stbi_flip_vertically_on_write(1);
	//
	//// char const *filename, int w, int h, int comp, const void *data, int stride_in_bytes
	//int flag = stbi_write_png(nameOfPath.c_str(), m_dimensions.x, m_dimensions.y, 4, m_data, 0); // using zero auto formats

	return (bool) true;
}

void CreateScreenshotPNG(void* imageData)
{
	ScreenShot* shot = (ScreenShot*) imageData;
	
	std::string path = "..\\Run_Win32\\Screenshots\\";
	std::string timeStamp = CurrentDateTime();

	std::string filename = path+timeStamp + ".png";
	
	stbi_flip_vertically_on_write(1);

	// char const *filename, int w, int h, int comp, const void *data, int stride_in_bytes
	stbi_write_png(filename.c_str(), shot->m_dimensions.x, shot->m_dimensions.y, 4, shot->m_data, 0); // using zero auto formats

	delete shot->m_data;
	delete shot;
}

bool Texture::CreateRenderTarget(int width, int height, eTextureFormat fmt)
{

	GL_CHECK_ERROR();

	// generate the link to this texture
	glGenTextures( 1, &m_textureID );							GL_CHECK_ERROR();
	if (m_textureID == NULL) {
		return false; 
	}

	// TODO - add a TextureFormatToGLFormats( GLenum*, GLenum*, GLenum*, eTextureFormat )
	//        when more texture formats are required; 
	GLenum internal_format = GL_RGBA8; 
	GLenum channels = GL_RGBA;  
	GLenum pixel_layout = GL_UNSIGNED_BYTE;  
	if (fmt == TEXTURE_FORMAT_D24S8) {
		internal_format = GL_DEPTH_STENCIL; 
		channels = GL_DEPTH_STENCIL; 
		pixel_layout = GL_UNSIGNED_INT_24_8;  
	}

	// Copy the texture - first, get use to be using texture unit 0 for this; 
	glActiveTexture( GL_TEXTURE0 );								GL_CHECK_ERROR();
	glBindTexture( GL_TEXTURE_2D, m_textureID );				GL_CHECK_ERROR();

												 // Copy data into it;
	glTexImage2D( GL_TEXTURE_2D, 0, 
		internal_format, // what's the format OpenGL should use
		width, 
		height,        
		0,             // border, use 0
		channels,      // how many channels are there?
		pixel_layout,  // how is the data laid out
		nullptr );     // don't need to pass it initialization data

	GL_CHECK_ERROR(); 

	//////////////////////////////////////////////////////////////////////////
	// cleanup after myself and unset it
	glBindTexture( GL_TEXTURE_2D, NULL );						GL_CHECK_ERROR();

	// Save this all off
	m_dimensions.x = width;  
	m_dimensions.y = height; 

	m_format = fmt; // I save the format with the texture
					// for sanity checking.

	// great, success
	return true; 
}

Texture* Texture::CreateCompatible(Texture* theTextureToUse)
{
	Texture* textureToReturn = new Texture();

	textureToReturn->m_dimensions = theTextureToUse->m_dimensions;

	const int size = textureToReturn->m_dimensions.x * textureToReturn->m_dimensions.y * 4; //4 because texels rgba
	textureToReturn->m_data = (unsigned char*)malloc(size);

	// may need to populate the data and create an actual texture..?
	textureToReturn->PopulateFromData(textureToReturn->m_data,textureToReturn->m_dimensions,4);

	return textureToReturn;
}

IntVector2 Texture::GetDimensions()
{
	return m_dimensions;
}

