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
	static BlockDefinition* GetDefinitionByType( Byte theType );
	static BlockDefinition* GetNextBlockDefinition(int direction, BlockDefinition* current);
	static void DeleteDefinitions();

private:
	static std::map<String, BlockDefinition*> s_definitions;

public:
	unsigned char	m_type = 0;
	int				m_lightLevel = 7;
	String			m_name;
	AABB2			m_topUVs;
	AABB2			m_sideUVs;
	AABB2			m_bottomUVs;

	bool			m_isFullyOpaque = true; // I block light, visibility, and hide my neighbors’ faces
	bool			m_isSolid = true;
	bool			m_isVisible = true; // I cannot be skipped during chunk mesh rebuilding
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