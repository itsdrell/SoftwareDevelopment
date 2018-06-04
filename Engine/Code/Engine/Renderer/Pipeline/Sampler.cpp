#pragma warning(disable: 4311)
#pragma warning(disable: 4302)
#include "Engine/Renderer/Pipeline/Sampler.hpp"
#include "Engine/Renderer/glfunctions.h"

Sampler::Sampler() 
	: m_sampler_handle(NULL)
{}

Sampler::~Sampler()
{
	Destroy();
}

bool Sampler::Create()
{
	// create the sampler handle if needed; 
	if (m_sampler_handle == NULL) {
		glGenSamplers( 1, (GLuint*)&m_sampler_handle ); 
		if (m_sampler_handle == NULL) {
			return false; 
		}
	}

	// setup wrapping
	glSamplerParameteri( (GLuint)m_sampler_handle, GL_TEXTURE_WRAP_S, GL_REPEAT );  
	glSamplerParameteri( (GLuint)m_sampler_handle, GL_TEXTURE_WRAP_T, GL_REPEAT );  
	glSamplerParameteri( (GLuint)m_sampler_handle, GL_TEXTURE_WRAP_R, GL_REPEAT );  

	// filtering; 
	glSamplerParameteri( (GLuint)m_sampler_handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glSamplerParameteri((GLuint) m_sampler_handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	return true; 
}

void Sampler::Destroy()
{
	if (m_sampler_handle != NULL) {
		glDeleteSamplers( 1,(GLuint*) &m_sampler_handle ); 
		m_sampler_handle = NULL; 
	}
} 