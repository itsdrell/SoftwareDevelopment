#pragma once
#include "Engine\Core\General\GameObject2D.hpp"
#include "Engine\ThirdParty\tinyxml\tinyxml2.h"
#include "Engine\Core\General\EngineCommon.hpp"
#include <map>

//====================================================================================
// Forward Declare
//====================================================================================
class SpriteAnimator;
class SpriteAnimation;
class Map;

//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class EffectDefinition
{
public:
	EffectDefinition(tinyxml2::XMLElement& node);

	static EffectDefinition* GetEffectDefinition(String name);
	static Strings GetAllEffectDefinitionNames();
	static void DeleteAllDefinitions();

public:
	// All we need to do is keep the name because when we parse the XML
	// the SpriteAnimation will register the animation 
	String						m_name;

	static std::map<String, EffectDefinition*>		s_definitions;
};


//-----------------------------------------------------------------------------------------------
class Effect : public GameObject2D
{
public:
	Effect() {}
	Effect(const String& nameOfEffect, Map& mapItIsOn);

	void Update();

public:
	EffectDefinition*		m_definition;
	SpriteAnimator*			m_animator;

};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [9/22/2018]
//====================================================================================