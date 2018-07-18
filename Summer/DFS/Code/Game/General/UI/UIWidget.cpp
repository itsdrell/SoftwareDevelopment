#include "UIWidget.hpp"
#include "Engine\Input\Mouse.hpp"
#include "Game\Main\Game.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Renderer\Systems\DebugRenderSystem.hpp"
#include "Engine\Core\General\Camera.hpp"
#include "Game\Main\Game.hpp"
#include "Game\GameStates\Playing.hpp"
#include "Game\General\Map.hpp"
#include "Engine\Core\Platform\File.hpp"
#include "Engine\Core\Utils\XmlUtilities.hpp"
#include "Engine\Core\Tools\DevConsole.hpp"

//====================================================================================
std::map<std::string, UIWidgetDefinition*> UIWidgetDefinition::s_definitions;

//====================================================================================

UIWidget::UIWidget(const Vector2& centerPos, const AABB2& size, std::string text, bool active)
{
	m_active = active;
	m_centerPos = centerPos;
	m_bounds = size;
	m_bounds.Translate(centerPos);
	m_isHoveredOver = false;

	// Create a renderable from this info and add it to the scene?
}

UIWidget::UIWidget(const std::string& text, const std::string& consoleCommandOnClick, bool active /*= false*/)
{
	m_name = text;
	m_active = active;
	m_centerPos = Vector2::ONE;
	m_bounds = AABB2();
	m_isHoveredOver = false;

	m_consoleCommandOnClick = consoleCommandOnClick;
}

UIWidget::UIWidget( UIWidgetDefinition& definition)
{
	m_active = true;
	m_isHoveredOver = false;
	m_definition = &definition;

	m_hoverColor = m_definition->m_defaultHoverColor;
	m_nonHoverColor = m_definition->m_defaultNonHoverColor;
	m_borderColor = m_definition->m_defaultBorderColor;
	m_fontColor = m_definition->m_defaultFontColor;
}

void UIWidget::Update()
{
	CheckForMouseOverlap();
}

void UIWidget::Render() const
{
	Renderer* r = Renderer::GetInstance();

	if(m_isHoveredOver)
		r->DrawTexturedAABB2(Renderer::GetInstance()->CreateOrGetTexture("Data/Images/Sprites/menuBackground.png"), m_bounds, m_hoverColor);
	else
		r->DrawTexturedAABB2(Renderer::GetInstance()->CreateOrGetTexture("Data/Images/Sprites/menuBackground.png"), m_bounds, m_nonHoverColor);

	r->DrawAABB2(m_bounds, m_borderColor, false);
	r->DrawFittedTextInBox(m_textBounds, m_definition->m_text, 2.f, 1.f, m_fontColor);
}

void UIWidget::CheckForMouseOverlap()
{
	Vector3 mousePos = Renderer::GetInstance()->m_defaultUICamera->ScreenToWorldCoordinate(GetMouseCurrentPosition(), 0.f);
	//DebugRenderLog(0.f, mousePos.ToString());
	//DebugRenderLog(0.f, m_bounds.ToString());

	if(m_bounds.IsPointInside(mousePos.xy()))
	{
		m_isHoveredOver = true;
		
		//DebugRenderLog(0.f, "Hovering over: button");
	}
	else
	{
		m_isHoveredOver = false;
	}

}

void UIWidget::OnClick()
{
	//DebugRenderLog(3.f, m_definition->m_text + ": Was Pressed!");
	//g_theGame->m_playingState->m_currentMap->GoToNextTurn();
	if(m_definition != nullptr)
	{
		Strings& theCommands = m_definition->m_commands;

		for(uint i = 0; i < theCommands.size(); i++)
		{
			CommandRunScript(theCommands.at(i).c_str());
		}
	}
	else
	{
		CommandRunScript(m_consoleCommandOnClick.c_str());
	}
	
}

void UIWidget::GenerateBounds(const AABB2& newBounds)
{
	m_bounds = newBounds;
	m_textBounds = GetBounds(m_bounds, Vector2(.1f, 0.f), Vector2(.9f, 1.f));
}

//====================================================================================
UIWidgetDefinition::UIWidgetDefinition(tinyxml2::XMLElement& node)
{
	m_name = ParseXmlAttribute(node, "name", "IDK");
	m_text = ParseXmlAttribute(node, "displayText", "IDK");

	m_defaultHoverColor = ParseXmlAttribute(node, "hoverColor", Rgba::WHITE);
	m_defaultNonHoverColor = ParseXmlAttribute(node, "defaultColor", Rgba::BLUE);
	m_defaultBorderColor = ParseXmlAttribute(node, "borderColor", Rgba::WHITE);
	m_defaultFontColor = ParseXmlAttribute(node, "fontColor", Rgba::BLACK);

	std::string filePath = ParseXmlAttribute(node, "scriptFile", "idk");

	if(filePath == "idk")
		ERROR_AND_DIE("Could not read the file path for: " + m_name);

	std::string fullPath = UI_WIDGET_RELATIVE_PATH + filePath + ".script";

	m_commands = GetAllLinesFromFile(fullPath.c_str());

	s_definitions.insert(std::pair<std::string,UIWidgetDefinition*>(m_name,this));

}

UIWidgetDefinition * UIWidgetDefinition::GetUIWidgetDefinition(std::string name)
{
	std::map<std::string,UIWidgetDefinition*>::iterator widgetIterator;
	widgetIterator = UIWidgetDefinition::s_definitions.find(name);
	if(widgetIterator != UIWidgetDefinition::s_definitions.end()){return widgetIterator->second;}

	return nullptr;
}
