#pragma once
#include <vector>
#include "Engine\Core\General\EngineCommon.hpp"
#include "Engine\Math\Geometry\AABB2.hpp"

//====================================================================================
// Forward Declare
//====================================================================================
class UIWidget;

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
class Container 
{
public:
	Container() {}
	Container(const String& name, uint amoutOfWidgets, const Vector2& center, const AABB2& menuSize);

	void CreateWidgetSlots();

	void Update();
	
	void Render() const;

	bool CanWeAddWidgets();
	void AddWidget(UIWidget& newWidget);
	void ClearWidgets() { m_widgets.clear(); }

	void OnClick();
	void CloseMenu();

	void SetBackgroundColor(const Rgba& color) { m_backgroundColor = color; }

	//--------------------------------------------------------------------------
	// Factory functions for Container
	void AddPauseMenu();

public:
	String						m_name;
	Vector2						m_centerPos;
	AABB2						m_menuSize;
	AABB2						m_headerBox;
	uint						m_amountOfWidgets;

	Rgba						m_backgroundColor;
	Rgba						m_fontColor;

	std::vector<AABB2>			m_widgetSlots;
	std::vector<UIWidget*>		m_widgets;
	UIWidget*					m_close;
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