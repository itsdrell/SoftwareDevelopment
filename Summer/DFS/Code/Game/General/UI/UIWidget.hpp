#pragma once
#include <string>
#include <map>
#include "Engine\Math\Geometry\AABB2.hpp"
#include "Engine\ThirdParty\tinyxml\tinyxml2.h"
#include "Engine\Core\General\EngineCommon.hpp"

//====================================================================================
// Forward Declare
//====================================================================================


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

class UIWidgetDefinition
{
public:
	UIWidgetDefinition(tinyxml2::XMLElement& node);

	static UIWidgetDefinition* GetUIWidgetDefinition(std::string name);

public:
	std::string		m_name;
	std::string		m_text;
	
	Strings			m_commands;

	static std::map<std::string, UIWidgetDefinition*>		s_definitions;
};


class UIWidget
{
public:
	UIWidget() {}
	UIWidget(const Vector2& centerPos, const AABB2& size, std::string text, bool active = true);
	UIWidget(const std::string& text, const std::string& consoleCommandOnClick, bool active = false);
	UIWidget(UIWidgetDefinition& definition);

	// check for clicks
	void Update();

	void CheckForMouseOverlap();

	void OnClick();

	std::string GetText() { return m_definition ? m_definition->m_text : m_consoleCommandOnClick;}


public:
	std::string					m_name;
	UIWidgetDefinition*			m_definition;

	bool						m_active;
	bool						m_isHoveredOver;

	std::string					m_consoleCommandOnClick;

	Vector2						m_centerPos;
	AABB2						m_bounds;
};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [6/20/2018]
//====================================================================================