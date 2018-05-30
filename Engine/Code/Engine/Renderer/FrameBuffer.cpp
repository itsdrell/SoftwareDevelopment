#include "FrameBuffer.hpp"
#include "Engine/Renderer/glfunctions.h"

FrameBuffer::FrameBuffer()
{
	glGenFramebuffers( 1, (GLuint*) &m_ID );
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers( 1, (GLuint*) &m_ID ); 
	m_ID = NULL; 
}

void FrameBuffer::SetColorTarget(Texture* color_target)
{
	m_color_target = color_target;
}

void FrameBuffer::SetDepthStencilTarget(Texture* depth_target)
{
	m_depth_stencil_target = depth_target;
}

int FrameBuffer::GetDepthStencilTargetWidth()
{
	return m_depth_stencil_target->GetDimensions().x;
}

int FrameBuffer::GetDepthStencilTargetHeight()
{
	return m_depth_stencil_target->GetDimensions().y;
}

bool FrameBuffer::Finalize()
{
	glBindFramebuffer( GL_FRAMEBUFFER, m_ID ); 

	// keep track of which outputs go to which attachments; 
	GLenum targets[1]; 

	// Bind a color target to an attachment point
	// and keep track of which locations to to which attachments. 
	glFramebufferTexture( GL_FRAMEBUFFER, 
		GL_COLOR_ATTACHMENT0 + 0, 
		m_color_target->GetID(), 
		0 ); 
	// 0 to to attachment 0
	targets[0] = GL_COLOR_ATTACHMENT0 + 0; 

	// Update target bindings
	glDrawBuffers( 1, targets ); 

	// Bind depth if available;
	if (m_depth_stencil_target == nullptr) {
		glFramebufferTexture( GL_FRAMEBUFFER, 
			GL_DEPTH_STENCIL_ATTACHMENT, 
			NULL, 
			0 ); 
	} else {
		glFramebufferTexture( GL_FRAMEBUFFER, 
			GL_DEPTH_STENCIL_ATTACHMENT, 
			m_depth_stencil_target->GetID(), 
			0 ); 
	}

	// Error Check - recommend only doing in debug
#if defined(_DEBUG)
	GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		//LogErrorf( "Failed to create framebuffer:  %u", status );
		return false;
	}
#endif

	return true;
}
