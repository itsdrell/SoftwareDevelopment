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
constexpr const char* UI_WIDGET_RELATIVE_PATH = "Data/Scripts/UIScripts/";

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
	static void DeleteAllDefinitions();

public:
	std::string					m_name;
	std::string					m_text;
	
	Strings						m_commands;

	Rgba						m_defaultFontColor;
	Rgba						m_defaultHoverColor;
	Rgba						m_defaultNonHoverColor;
	Rgba						m_defaultBorderColor;

	static std::map<std::string, UIWidgetDefinition*>		s_definitions;
};

//--------------------------------------------------------------------------
class UIWidget
{
public:
	UIWidget() {}
	UIWidget(const Vector2& centerPos, const AABB2& size, std::string text, bool active = true);
	UIWidget(const std::string& text, const std::string& consoleCommandOnClick, bool active = false);
	UIWidget(UIWidgetDefinition& definition);

	// check for clicks
	virtual void Update();

	virtual void Render() const;

	virtual void CheckForMouseOverlap();

	virtual void OnClick();

	virtual void GenerateBounds(const AABB2& newBounds);

	std::string GetText() { return m_definition ? m_definition->m_text : m_consoleCommandOnClick;}


public:
	std::string					m_name;
	UIWidgetDefinition*			m_definition;

	bool						m_active;
	bool						m_isHoveredOver;

	std::string					m_consoleCommandOnClick;

	Vector2						m_centerPos;
	AABB2						m_bounds;
	AABB2						m_textBounds;

	Rgba						m_hoverColor;
	Rgba						m_nonHoverColor;
	Rgba						m_borderColor;
	Rgba						m_fontColor;

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