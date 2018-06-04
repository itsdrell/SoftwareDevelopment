#pragma once
#include "Engine/Renderer/RendererTypes.hpp"
#include <map>

//////////////////////////////////////////////////////////////////////////
class ShaderProgram;
class MaterialProperties;
class Texture;
class Sampler;

//////////////////////////////////////////////////////////////////////////
enum RenderQueue
{
	// This is in order it needs to be rendered 
	RENDER_QUEUE_OPAQUE,
	RENDER_QUEUE_ALPHA
};
RenderQueue RenderQueueFromString(std::string theValue);

//////////////////////////////////////////////////////////////////////////
struct RenderState 
{
	RenderState();
	
	// Raster State Control
	eCullMode					m_cullMode;			// CULL_BACK
	eFillMode					m_fillMode;			// FILL_SOLID
	eWindOrder					m_frontFace;			// WIND_COUNTER_CLOCKWISE

	// Depth State Control
	DepthBufferComparisons		m_depthCompare;		// COMPARE_LESS
	bool						m_depthWrite;          // true

	// Blends
	// Disabled is: 
	// Color: Compare Add  1, zero. 
	// Alpha: Compare ADD, 1, 1
	eBlendOperation					m_colorBlendOp;		// COMPARE_ADD
	eBlendFactor				m_colorSrcFactor;		// BLEND_ONE
	eBlendFactor				m_colorDstFactor;		// BLEND_ZERO

	eBlendOperation					m_alphaBlendOp;		// COMPARE_ADD
	eBlendFactor				m_alphaSrcFactor;		// BLEND_ONE
	eBlendFactor				m_alphaDstFactor;		// BLEND_ONE
}; 


//////////////////////////////////////////////////////////////////////////
// This is the part of Shader.hpp
// Shader is just a combination of a program and render state
class Shader
{
	// so it can add shaders
	friend class BuiltInShaders;

public:

	Shader(){}
	Shader(std::string path);

	// none of these call GL calls, just sets the internal state
	void SetProgram( ShaderProgram* theProgram); 

	void EnableBlending( eBlendOperation op, eBlendFactor src, eBlendFactor dst ); 
	void DisableBlending();

	void SetDepth( DepthBufferComparisons compare, bool write ); 
	void DisableDepth() { SetDepth( COMPARE_ALWAYS, false ); }

	void SetCullMode( eCullMode cullMode); 
	void SetFillMode( eFillMode fillMode); 
	void SetFrontFace( eWindOrder windOrder); 

	static Shader* CreateOrGetShader(std::string path);
	static void ReloadShaders();

private:
	static void AddShader(std::string name, Shader* newShader);

public:
	std::string			m_name;
	ShaderProgram*		m_program; 
	RenderState			m_state; 

	// SORTING
	int					m_sortOrder;
	RenderQueue			m_renderQueue;

	// Default properties for materials (XML)
	std::vector<Texture*>				m_textures;
	std::vector<Sampler*>				m_samplers;
	std::vector<MaterialProperties*>	m_properties;

	static std::map<std::string, Shader*>	s_shaders;
}; 
