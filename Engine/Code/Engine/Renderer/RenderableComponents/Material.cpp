#include "Material.hpp"
#include "Engine/Renderer/glfunctions.h"
#include "Engine/Renderer/Pipeline/Sampler.hpp"
#include "Engine/Renderer/RenderableComponents/Shader.hpp"
#include "Engine/Renderer/Images/Textures/Texture.hpp"
#include "Engine/ThirdParty/tinyxml/tinyxml2.h"
#include "Engine/Core/Utils/XmlUtilities.hpp"
#include "Engine/Core/Tools/ErrorWarningAssert.hpp"

//////////////////////////////////////////////////////////////////////////

std::map<std::string, Material*> Material::s_materials;

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PROPERTIES
void MaterialPropertiesFloat::Bind(int programHandle)
{
	GLuint idx = glGetUniformLocation( programHandle, m_name.c_str());
	
	if(idx >= 0)
		glUniform1fv(idx, 1, &m_data);
}

void MaterialPropertiesVector4::Bind(int programHandle)
{
	float value[4] 
	{
		m_data.x,
		m_data.y,
		m_data.z,
		m_data.w
	};

	GLuint idx = glGetUniformLocation( programHandle, m_name.c_str());

	if(idx >= 0)
		glUniform4fv(idx, 1, value);
}

void MaterialPropertiesRgba::Bind(int programHandle)
{
	Vector4 asFloat = m_data.GetAsNormalizedVector4();
	
	float value[4] 
	{
		asFloat.x,
		asFloat.y,
		asFloat.z,
		asFloat.w
	};
	
	GLuint idx = glGetUniformLocation( programHandle, m_name.c_str());

	if(idx >= 0)
		glUniform4fv(idx, 1, value);
}


void MaterialPropertiesSpecular::Bind(int programHandle)
{
	// Sets the data to the UBO which is later bound in the renderpath
	programHandle = programHandle;
	Renderer::GetInstance()->SetSpecularConstants(m_specularAmount, m_specularPower);
}

//////////////////////////////////////////////////////////////////////////
// MATERIAL
Material::Material(Shader * shader)
{
	m_shader = shader;
	m_isResource = true;
}

Material::Material(Material& copyFrom)
{
	m_isResource = false;

	m_name = copyFrom.m_name;

	m_shader = copyFrom.m_shader;
	m_properties = copyFrom.m_properties;
	m_textures = copyFrom.m_textures;
	m_samplers = copyFrom.m_samplers;
	//m_blocks = copyFrom.m_blocks;
}

Material::Material(std::string path)
{
	// If we are reading from file, then this is a resource
	m_isResource = true;

	std::string shaderPath = "Data/Shaders/";
	std::string texturePath = "Data/"; // just in case we are getting from a model file
	
	//////////////////////////////////////////////////////////////////////////
	tinyxml2::XMLDocument doc;
	doc.LoadFile(path.c_str());

	tinyxml2::XMLElement* rootElement = doc.RootElement();
	GUARANTEE_OR_DIE(rootElement != nullptr, "Could not read Material: " + path);

	//////////////////////////////////////////////////////////////////////////
	// Get the values from the header
	m_name = ParseXmlAttribute(*rootElement,"name","ERROR");

	//////////////////////////////////////////////////////////////////////////
	tinyxml2::XMLElement* indexElement = rootElement->FirstChildElement();

	while(indexElement)
	{
		std::string current = indexElement->Name();

		//////////////////////////////////////////////////////////////////////////
		if(current == "Shader")
		{
			std::string spath = ParseXmlAttribute(*indexElement, "name", "ERROR");
			std::string fullPath = shaderPath + spath + ".shader";

			m_shader = Shader::CreateOrGetShader(fullPath);

			// Get any default values from the shader
			m_textures = m_shader->m_textures;
			m_properties = m_shader->m_properties;
			m_samplers = m_shader->m_samplers;
		}

		//////////////////////////////////////////////////////////////////////////
		if(current == "Textures")
		{
			tinyxml2::XMLElement* textureIndex = indexElement->FirstChildElement();

			while(textureIndex)
			{
				// Get the texture
				std::string tpath = ParseXmlAttribute(*textureIndex, "path", "ERROR");
				std::string fullPath = texturePath + tpath;
				std::string index = ParseXmlAttribute(*textureIndex, "bind", "-1");
				// Get the sampler
				std::string samplerType = ParseXmlAttribute(*textureIndex, "sampler", "ERROR");


				int bindIndex = stoi(index);

				// making sure we dont bind at an index that doesn't exist
				if((uint) bindIndex > m_textures.size())
				{
					m_textures.push_back(Renderer::GetInstance()->CreateOrGetTexture(fullPath));
					m_samplers.push_back(CreateSamplerFromString(samplerType));

				}
				else
				{
					m_textures.at(bindIndex) = Renderer::GetInstance()->CreateOrGetTexture(fullPath);
					m_samplers.at(bindIndex) = CreateSamplerFromString(samplerType);
				}

				
					

				textureIndex = textureIndex->NextSiblingElement();
			}

		}

		//////////////////////////////////////////////////////////////////////////
		if(current == "Properties")
		{
			tinyxml2::XMLElement* propertyIndex = indexElement->FirstChildElement();

			while(propertyIndex)
			{
				std::string type = ParseXmlAttribute(*propertyIndex, "type", "ERROR");
				std::string name = ParseXmlAttribute(*propertyIndex, "name", "ERROR");
				std::string value = ParseXmlAttribute(*propertyIndex, "value", "ERROR");

				// This will handle all the cases and add it (so this function doesn't get stupid long yw)
				CreateAndAddProperty(type, name, value, &m_properties);

				propertyIndex = propertyIndex->NextSiblingElement();
			}
		}

		//////////////////////////////////////////////////////////////////////////
		indexElement = indexElement->NextSiblingElement();
	}
}

void Material::SetShader(Shader * shader)
{
	m_shader = shader;
}

void Material::SetSampler(uint bind, Sampler * sampler)
{
	m_samplers[bind] = sampler;
}

void Material::SetTexture(uint idx, Texture * texture)
{
	// This is so im not defining a sized vector, but requires you to Set in order which makes sense..? 
	if(idx >= m_textures.size())
	{
		m_textures.push_back(texture);
	}
	else
	{
		m_textures.at(idx) = texture;
	}

	TODO("Create different Samplers");
	// WE need a sampler for each texture so for now just gonna add a default
	// when we add a texture
	Sampler* newSampler = new Sampler();
	newSampler->Create();
	m_samplers.push_back(newSampler);
}

void Material::SetProperty(std::string name, float value)
{
	m_properties.push_back(new MaterialPropertiesFloat(name, value));
}

void Material::SetProperty(std::string name, Vector4 & value)
{
	m_properties.push_back(new MaterialPropertiesVector4(name, value));
}

void Material::SetProperty(std::string name, Rgba & value)
{
	m_properties.push_back(new MaterialPropertiesRgba(name, value));
}

void Material::SetTint(Rgba & tint)
{
	SetProperty("TINT", tint);
}

void Material::SetSpecular(float amount, float power)
{
	m_properties.push_back(new MaterialPropertiesSpecular("Specular", power, amount));
}

STATIC Material* Material::CreateOrGetMaterial(std::string path)
{
	
	//////////////////////////////////////////////////////////////////////////
	// See if we already have the material, if so return that
	std::map<std::string, Material*>::iterator materialIterator;
	materialIterator = Material::s_materials.find(path);
	
	// If we found one, CLONE IT
	if(materialIterator != Material::s_materials.end())
	{ 
		return new Material(*materialIterator->second); 
	
	}

	//////////////////////////////////////////////////////////////////////////
	// If we didn't find it, lets make a new one
	std::string fullpath = "Data/Materials/" + path + ".material";
	Material* newMaterial = new Material(fullpath);

	s_materials.insert(std::pair<std::string,Material*>(path, newMaterial));

	//////////////////////////////////////////////////////////////////////////
	// Since we are doing copies, once we store the resource, we will clone and return that
	return new Material(*newMaterial); 
}

STATIC Sampler* Material::CreateSamplerFromString(std::string type)
{
	Sampler* typeToCreate = new Sampler();

	if(type == "default") { typeToCreate->Create(); }
	// ... add more types


	// error check
	if(type == "ERROR") { ERROR_RECOVERABLE("Could not find a Sampler type: " + type); }

	return typeToCreate;
}

void Material::CreateAndAddProperty(std::string type, std::string name, std::string value, std::vector<MaterialProperties*>* propertyContainer)
{
	//////////////////////////////////////////////////////////////////////////\
	// See what type of property to create based off value


	// Since shaders can make properties, we are gonna check and see if we already have these and replace if so
	bool alreadyExists = false;
	uint indx = 0;

	for(uint i = 0; i < propertyContainer->size(); i++)
	{
		MaterialProperties* current = propertyContainer->at(i);

		if(current->m_name == name)
		{
			alreadyExists = true;
			indx = i;
			continue;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	if(type == "float")
	{
		float v = (float)atof(value.c_str());

		if(alreadyExists)
		{
			propertyContainer->at(indx) = new MaterialPropertiesFloat(name, v);
		}
		else
		{
			propertyContainer->push_back(new MaterialPropertiesFloat(name, v));
		}


		return;
	}

	if(type == "vec4")
	{
		Vector4 theVector;
		theVector.SetFromText(value);

		if(alreadyExists)
		{
			propertyContainer->at(indx) = new MaterialPropertiesVector4( name, theVector);
		}
		else
		{
			propertyContainer->push_back(new MaterialPropertiesVector4( name, theVector));

		}


		return;
	}

	if(type == "color")
	{
		Rgba theColor;
		theColor.SetFromText(value.c_str());

		if(alreadyExists)
		{
			propertyContainer->at(indx) = new MaterialPropertiesRgba( name, theColor);
		}
		else
		{
			propertyContainer->push_back(new MaterialPropertiesRgba( name, theColor));
		}

		return;
	}

	// x == amount, y == power
	if(type == "specular")
	{
		Vector2 theSpec;
		theSpec.SetFromText(value.c_str());

		if(alreadyExists)
		{
			propertyContainer->at(indx) = new MaterialPropertiesSpecular( name, theSpec.y, theSpec.x);
		}
		else
		{
			propertyContainer->push_back(new MaterialPropertiesSpecular( name, theSpec.y, theSpec.x));

		}


		return;
	}

}

// void Material::SetPropertyBlock(uint idx, UniformBuffer * block)
// {
// }

Sampler* Material::GetSample(uint idx)
{
	return m_samplers[idx];
}


