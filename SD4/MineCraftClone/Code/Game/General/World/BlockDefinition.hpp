#pragma once
#include "Engine/Core/General/EngineCommon.hpp"
#include "Engine/Math/Geometry/AABB2.hpp"
#include "Engine/ThirdParty/tinyxml/tinyxml2.h"
#include <map>

//====================================================================================
// Forward Declare
//====================================================================================


//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================
enum BlockTypes
{
	BLOCK_TYPE_AIR = 0,
	BLOCK_TYPE_GRASS,
	BLOCK_TYPE_STONE,
	BLOCK_TYPE_SNOW,
	BLOCK_TYPE_SAND, 
	BLOCK_TYPE_DIRT,
	BLOCK_TYPE_COBBLESTONE,
	BLOCK_TYPE_BEDROCK,
	BLOCK_TYPE_TEST = 69
};

//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class BlockDefinition
{
public:
	BlockDefinition(const tinyxml2::XMLElement& definitionNode);
	BlockDefinition(const String& name, BlockTypes theType, bool isOpqaue, const IntVector2& spriteCoordsForTop, const IntVector2& spriteCoordsForSides, const IntVector2& spriteCoordsForBottom);
	~BlockDefinition() {}

	static BlockDefinition* GetDefinitionByName(const String& nameOfDefinition);
	static BlockDefinition* GetDefinitionByType( BlockTypes theType );
	static void DeleteDefinitions();

private:
	static std::map<String, BlockDefinition*> s_definitions;

public:
	unsigned char	m_type;
	String			m_name;
	AABB2			m_topUVs;
	AABB2			m_sideUVs;
	AABB2			m_bottomUVs;

	bool			m_isFullyOpaque; // cant see through
};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [2/6/2019]
//====================================================================================