#pragma warning(disable: 4311)
#pragma warning(disable: 4302)
#include "Engine/Renderer/Pipeline/Sampler.hpp"
#include "Engine/Renderer/glfunctions.h"
#include "../../Core/General/EngineCommon.hpp"

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


	TODO("Make different versions");
	// filtering; 
	//glSamplerParameteri( (GLuint)m_sampler_handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	//glSamplerParameteri((GLuint) m_sampler_handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST );


	// this two are for mip maps
	glSamplerParameteri(  (GLuint)m_sampler_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );         // Default: GL_LINEAR
	glSamplerParameteri(  (GLuint)m_sampler_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR );         // Default: GL_LINEAR

	// Limit where on the LOD I can actually fetch (if you want to control it through the sampler)
	// defaults to -1000.0f to 1000.0f
	glSamplerParameterf(  (GLuint)m_sampler_handle, GL_TEXTURE_MIN_LOD, (float)-1000.0f ); 
	glSamplerParameterf(  (GLuint)m_sampler_handle, GL_TEXTURE_MAX_LOD, (float)1000.0f ); 

	return true; 
}

void Sampler::Destroy()
{
	if (m_sampler_handle != NULL) {
		glDeleteSamplers( 1,(GLuint*) &m_sampler_handle ); 
		m_sampler_handle = NULL; 
	}
} 