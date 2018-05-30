#pragma once
#if !defined( __RENDER_TEXTURECUBE__ )
#define __RENDER_TEXTURECUBE__

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/

#include "../Core/EngineCommon.hpp"
#include "RendererTypes.hpp"


/************************************************************************/
/*                                                                      */
/* TYPES                                                                */
/*                                                                      */
/************************************************************************/
class Image;  // core/image/image.h

//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
//------------------------------------------------------------------------
class TextureCube 
{

public:
	TextureCube();
	virtual ~TextureCube();

	void cleanup();

	// make a cube map from 6 images
	// +x -x +y -y +z -z
	bool make_from_images( Image const *images );

	// todo - may want this to take tile offsets
	// into the image since I can't assume the same
	// plus sign format my demo one is in
	bool make_from_image( Image const &image );
	bool make_from_image( char const *filename ); 

	// cube maps should be square on each face
	inline uint get_width() const { return m_size; }
	inline uint get_height() const { return m_size; }

	inline bool is_valid() const { return (m_handle != NULL); }

public:
	// gl
	inline uint get_handle() const { return m_handle; }

public:
	uint m_size;          // cube maps are equal length on all sizes
	eTextureFormat m_format; 

	// GL 
	uint m_handle; 

};


#endif 