#pragma once
#include "Engine/Core/General/EngineCommon.hpp"
#include "Engine/Renderer/RenderableComponents/Shader.hpp"
#include <map>
#include "../Pipeline/Sampler.hpp"

//////////////////////////////////////////////////////////////////////////
// Forward Declare
class Texture;
class Sampler;
//class UniformBlock;


//////////////////////////////////////////////////////////////////////////
// Property classes

// Parent
class MaterialProperties
{
public:
	MaterialProperties(std::string name)
		: m_name(name)
	{}

	virtual MaterialProperties* Clone() = 0;

	virtual void Bind(int programHandle) = 0;

public:
	std::string m_name;
};

// Children
class MaterialPropertiesFloat : public MaterialProperties
{
public:
	MaterialPropertiesFloat(std::string name, float value)
		: MaterialProperties(name)
		, m_data( value ) {}

	virtual void Bind(int programHandle) override;

	virtual MaterialProperties* Clone() override;

public:
	float m_data;
};

class MaterialPropertiesVector4 : public MaterialProperties
{
public:
	MaterialPropertiesVector4(std::string name, Vector4& value)
		: MaterialProperties(name)
		, m_data( value ) {}

	virtual void Bind(int programHandle) override;

	virtual MaterialProperties* Clone() override;

public:
	Vector4 m_data;
};

class MaterialPropertiesRgba : public MaterialProperties
{
public:
	MaterialPropertiesRgba(std::string name, Rgba& value)
		: MaterialProperties(name)
		, m_data( value ) {}

	virtual void Bind(int programHandle) override;

	virtual MaterialProperties* Clone() override;

public:
	Rgba m_data;
};

// This is gross but an edge case so w/e
class MaterialPropertiesSpecular : public MaterialProperties
{
public:
	MaterialPropertiesSpecular(std::string name, float power, float amount)
		: MaterialProperties(name)
		, m_specularAmount (amount)
		, m_specularPower (power) {}

	virtual void Bind(int programHandle) override;

	virtual MaterialProperties* Clone() override;

public:
	float m_specularPower;
	float m_specularAmount;
};


//////////////////////////////////////////////////////////////////////////
// MATERIALS
class Material
{
public:
	
	Material(){}
	Material(Shader* shader);
	Material(std::string path); // for xml

	// We are gonna store all materials and then clone from them to make instances
	Material(Material& copyFrom);

	inline bool IsValid() const { return m_shader != nullptr; }

	void SetShader( Shader* shader );
	void SetSampler( uint bind, Sampler* sampler);
	void SetTexture(uint idx, Texture* texture, SamplerTypes theType = NORMAL);
	void SetPropertyBlock(uint idx, UniformBuffer* block);

	Sampler* GetSample( uint idx );
	
	// Set Properties
	void SetProperty( std::string name, float value );
	void SetProperty( std::string name, Vector4& value );
	void SetProperty( std::string name, Rgba& value );


	// Cool helpers
	void SetTint( const Rgba tint);
	void SetSpecular( float amount, float power );

	int GetSortOrder() { return m_shader->m_sortOrder; }
	RenderQueue GetQueue() { return m_shader->m_renderQueue; }

	// XML

	// Only returns a copy (instance), no matter what
	static Material* CreateOrGetMaterial(std::string path);

	// These need to be static so the Shader can also use these functions
	// Which is nice so if they get changed you don't have to do it twice
	static Sampler* CreateSamplerFromString(std::string type);
	static void CreateAndAddProperty(std::string type, std::string name, std::string value, std::vector<MaterialProperties*>* propertyContainer);

public:

	std::string							m_name;
	
	Shader*								m_shader;
	
	std::vector<Texture*>				m_textures;
	std::vector<Sampler*>				m_samplers;
	std::vector<MaterialProperties*>	m_properties;
	//std::vector<UniformBlock*>			m_blocks;

	// if true, its what we clone from and is NOT an instance
	bool m_isResource;


	static std::map<std::string, Material*>		s_materials;
};