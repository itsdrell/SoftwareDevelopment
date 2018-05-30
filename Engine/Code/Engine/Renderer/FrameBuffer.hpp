#pragma once
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/IntVector2.hpp"

class FrameBuffer
{
public:
	FrameBuffer(); 
	~FrameBuffer();

	// should just update members
	// finalize does the actual binding
	void SetColorTarget( Texture* color_target ); 
	void SetDepthStencilTarget( Texture* depth_target ); 

	int GetID() { return m_ID; }
	int GetDepthStencilTargetWidth();
	int GetDepthStencilTargetHeight();

	// setups the the GL frame buffer - called before us. 
	// TODO: Make sure this only does work if the targets
	// have changed.
	bool Finalize(); 

public:
	int			m_ID; 
	Texture*	m_color_target = nullptr; 
	Texture*	m_depth_stencil_target = nullptr; 
};