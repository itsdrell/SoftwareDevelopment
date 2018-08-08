#pragma once
#include "Engine/ThirdParty/tinyxml/tinyxml2.h"
#include <map>
#include "Engine/Math/Vectors/IntVector2.hpp"
#include "Engine/Math/Geometry/AABB2.hpp"
#include "Engine/Core/General/Rgba.hpp"
#include "Game/SystemsAndTools/Tags.hpp"

//=============================================================
// Forward Declare
//=============================================================


//=============================================================
// ENUMS
//=============================================================


//=============================================================
// Structs
//=============================================================


//=============================================================
// Classes
//=============================================================
class TileDefinition
{
public:
	TileDefinition(const tinyxml2::XMLElement& definitionNode);

	static void DeleteAllDefinitions();

public:
	std::string			m_name;
	std::string			m_displayName;
	IntVector2			m_spriteCoords; // index of sprite in sprite sheet
	AABB2				m_uvCoords;
	Rgba				m_colorRepresentation;

	Tags				m_movementTags;

	static std::map<std::string, TileDefinition*>	s_definitions;

};

//=============================================================
// Standalone C Functions
//=============================================================
TileDefinition* GetTileDefinition(std::string name);
TileDefinition* GetTileDefinition(Rgba& color);

//=============================================================
// Externs
//=============================================================
