#include "Engine/Renderer/shaderprogram.hpp"
#include "Shader.hpp"
#include "../Core/General/EngineCommon.hpp"
#include "../Core/Tools/ErrorWarningAssert.hpp"
#include "ThirdParty/tinyxml/tinyxml2.h"
#include "Engine/Core/Utils/XmlUtilities.hpp"
#include "Engine/Core/Utils/StringUtils.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Sampler.hpp"

//////////////////////////////////////////////////////////////////////////
std::map<std::string, Shader*> Shader::s_shaders;

//////////////////////////////////////////////////////////////////////////
Shader::Shader(std::string path)
{
	m_program = new ShaderProgram();

	//////////////////////////////////////////////////////////////////////////
	tinyxml2::XMLDocument doc;
	doc.LoadFile(path.c_str());

	tinyxml2::XMLElement* rootElement = doc.RootElement();
	GUARANTEE_OR_DIE(rootElement != nullptr, "Could not read Shader: " + path);

	//////////////////////////////////////////////////////////////////////////
	// Get the values from the header
	m_name = ParseXmlAttribute(*rootElement,"name","ERROR");

	m_state.m_cullMode = ParseXmlAttribute(*rootElement,"cull", CULLMODE_NONE); // THIS COULD BE FRONT ONLY BUT W/E
	m_state.m_fillMode = ParseXmlAttribute(*rootElement, "fill", FILLMODE_SOLID);
	m_state.m_frontFace = ParseXmlAttribute(*rootElement, "frontface", WIND_COUNTER_CLOCKWISE);

	//////////////////////////////////////////////////////////////////////////
	tinyxml2::XMLElement* indexElement = rootElement->FirstChildElement();
	
	while(indexElement)
	{
		std::string current = indexElement->Name();

		//////////////////////////////////////////////////////////////////////////
		// Bind the shader PROGRAM info
		if(current == "program")
		{
			// Get the defines
			std::string theDefines = ParseXmlAttribute(*indexElement, "define", "");
			Strings allDefines = SplitString(theDefines,";");

			// See if we only have one file (you're welcome future me)
			tinyxml2::XMLElement* vertex = indexElement->FirstChildElement();
			std::string onlyOne = vertex->Name();
			
			if(onlyOne == "both")
			{
				std::string thePath = ParseXmlAttribute(*vertex, "file", "ERROR");
				m_program->LoadFromFiles(thePath.c_str(), allDefines);

			}
			else
			{
				// Get the vertex path
				std::string vertexPath = ParseXmlAttribute(*vertex,"file", "ERROR");

				// Get the fragment path
				tinyxml2::XMLElement* fragment = vertex->NextSiblingElement();
				std::string fragmentPath = ParseXmlAttribute(*fragment, "file", "ERROR");

				// Error checks
				if(vertexPath == "ERROR")
					ERROR_RECOVERABLE("Could not find the vertex path for: " + m_name);
				if(fragmentPath == "ERROR")
					ERROR_RECOVERABLE("Could not find the fragment path for: " + m_name);

				// Set the program
				m_program->LoadFromFiles(m_name,vertexPath,fragmentPath, allDefines);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// Bind all the render state info
		if(current == "blend")
		{
			tinyxml2::XMLElement* color = indexElement->FirstChildElement();
			m_state.m_colorBlendOp =   ParseXmlAttribute(*color, "op", COMPARE_ADD);
			m_state.m_colorSrcFactor = ParseXmlAttribute(*color, "src", BLEND_ONE);
			m_state.m_colorDstFactor = ParseXmlAttribute(*color, "dest", BLEND_ZERO);

			// this could be null so we need to check
			tinyxml2::XMLElement* alpha = color->NextSiblingElement();
			
			if(alpha == nullptr)
			{
				indexElement = indexElement->NextSiblingElement();
				continue;
			}

			m_state.m_alphaBlendOp = 	ParseXmlAttribute(*alpha, "op", COMPARE_ADD);
			m_state.m_alphaSrcFactor =	ParseXmlAttribute(*alpha, "src", BLEND_ONE);
			m_state.m_alphaDstFactor = 	ParseXmlAttribute(*alpha, "dest", BLEND_ONE);
		}

		//////////////////////////////////////////////////////////////////////////
		if(current == "depth")
		{
			m_state.m_depthWrite = ParseXmlAttribute(*indexElement, "write", true);
			m_state.m_depthCompare = ParseXmlAttribute(*indexElement, "test", COMPARE_LESS);
		}

		//////////////////////////////////////////////////////////////////////////
		if(current == "Texture")
		{
			// We shouldn't have to worry about bind here, since they will be in order
			// Material, it will matter since they are gonna overwrite it
			
			std::string thePath = ParseXmlAttribute(*indexElement, "path", "ERROR");
			std::string samplerType = ParseXmlAttribute(*indexElement, "sampler", "ERROR");

			std::string fullpath = "Data/" + thePath;

			m_textures.push_back(Renderer::GetInstance()->CreateOrGetTexture(fullpath));

			m_samplers.push_back(Material::CreateSamplerFromString(samplerType));
		}

		//////////////////////////////////////////////////////////////////////////
		if( current == "Property")
		{
			std::string type = ParseXmlAttribute(*indexElement, "type", "ERROR");
			std::string name = ParseXmlAttribute(*indexElement, "name", "ERROR");
			std::string value = ParseXmlAttribute(*indexElement, "value", "ERROR");

			// This will handle all the cases and add it (so this function doesn't get stupid long yw)
			Material::CreateAndAddProperty(type, name, value, &m_properties);
		}

		//////////////////////////////////////////////////////////////////////////
		if( current == "Sorting")
		{
			int sortOrder = ParseXmlAttribute(*indexElement, "sortOrder", 0);
			std::string queue = ParseXmlAttribute(*indexElement, "queue", "OPAQUE");

			m_renderQueue = RenderQueueFromString(queue);
			m_sortOrder = sortOrder;
		}

		//////////////////////////////////////////////////////////////////////////
		indexElement = indexElement->NextSiblingElement();
	}

}

void Shader::SetProgram(ShaderProgram* theProgram)
{
	m_program = theProgram;
}

void Shader::EnableBlending(eBlendOperation op, eBlendFactor src, eBlendFactor dst)
{
	m_state.m_colorBlendOp = op;
	m_state.m_colorSrcFactor = src;
	m_state.m_colorDstFactor = dst;
}

void Shader::DisableBlending()
{
	// Disabled is: 
	// Color: Compare Add  1, zero. 
	// Alpha: Compare ADD, 1, 1

	m_state.m_colorBlendOp = COMPARE_ADD;
	m_state.m_colorSrcFactor = BLEND_ONE;
	m_state.m_colorDstFactor = BLEND_ZERO;

	m_state.m_alphaBlendOp = COMPARE_ADD;
	m_state.m_colorSrcFactor = BLEND_ONE;
	m_state.m_colorDstFactor = BLEND_ONE;
}

void Shader::SetDepth(DepthBufferComparisons compare, bool write)
{
	m_state.m_depthCompare = compare;
	m_state.m_depthWrite = write;
}

void Shader::SetCullMode(eCullMode cullMode)
{
	m_state.m_cullMode = cullMode;
}

void Shader::SetFillMode(eFillMode fillMode)
{
	m_state.m_fillMode = fillMode;
}

void Shader::SetFrontFace(eWindOrder windOrder)
{
	m_state.m_frontFace = windOrder;
}

Shader* Shader::CreateOrGetShader(std::string path)
{
	//////////////////////////////////////////////////////////////////////////
	// See if we already have the shader, if so return that
	std::map<std::string,Shader*>::iterator shaderIterator;
	shaderIterator = Shader::s_shaders.find(path);
	if(shaderIterator != Shader::s_shaders.end()){ return shaderIterator->second; }

	//////////////////////////////////////////////////////////////////////////
	// If we didn't find it, lets make a new one
	Shader* newShader = new Shader(path);

	// We are storing based off path so that we can rebind shaders
	s_shaders.insert(std::pair<std::string,Shader*>(path, newShader));


	//////////////////////////////////////////////////////////////////////////
	return newShader;
}


void Shader::ReloadShaders()
{
	for (std::map<std::string,Shader*>::iterator it=s_shaders.begin(); it!=s_shaders.end(); ++it)
	{
		std::string path = it->first;
		it->second = new Shader(path);
	}
}

void Shader::AddShader(std::string name, Shader* newShader)
{
	s_shaders.insert(std::pair<std::string,Shader*>(name, newShader));
}

//////////////////////////////////////////////////////////////////////////
RenderState::RenderState()
{
	m_cullMode = CULLMODE_BACK;			
	m_fillMode = FILLMODE_SOLID;			
	m_frontFace = WIND_COUNTER_CLOCKWISE;		

	m_depthCompare = COMPARE_LESS;		
	m_depthWrite = true;       

	m_colorBlendOp = COMPARE_ADD;	
	m_colorSrcFactor = BLEND_ONE;	
	m_colorDstFactor = BLEND_ZERO;	

	m_alphaBlendOp = COMPARE_ADD;	
	m_alphaSrcFactor = BLEND_ONE;	
	m_alphaDstFactor = BLEND_ONE;	
}

RenderQueue RenderQueueFromString(std::string theValue)
{
	if( theValue == "OPAQUE") { return RENDER_QUEUE_OPAQUE; }
	if( theValue == "ALPHA") { return RENDER_QUEUE_ALPHA; }

	ERROR_RECOVERABLE("Could not find the right render queue for the shader");

	return RENDER_QUEUE_OPAQUE;
}
