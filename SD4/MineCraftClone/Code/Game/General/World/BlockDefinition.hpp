#pragma once
#include "Engine/Core/General/EngineCommon.hpp"
#include "Engine/Math/Geometry/AABB2.hpp"
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
	BLOCK_TYPE_TEST
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
	BlockDefinition(const String& name, BlockTypes theType, const IntVector2& spriteCoordsForTop, const IntVector2& spriteCoordsForSides, const IntVector2& spriteCoordsForBottom);
	~BlockDefinition() {}

	static BlockDefinition* GetDefinitionByName(const String& nameOfDefinition);
	static BlockDefinition* GetDefinitionByType( BlockTypes theType );

private:
	static std::map<String, BlockDefinition*> s_definitions;

public:
	BlockTypes		m_type;
	String			m_name;
	AABB2			m_topUVs;
	AABB2			m_sideUVs;
	AABB2			m_bottomUVs;
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