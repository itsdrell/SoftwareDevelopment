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
}

void UIWidget::Update()
{
	CheckForMouseOverlap();
}

void UIWidget::CheckForMouseOverlap()
{
	Vector3 mousePos = Renderer::GetInstance()->m_defaultUICamera->ScreenToWorldCoordinate(GetMouseCurrentPosition(), 0.f);

	if(m_bounds.IsPointInside(mousePos.xy()))
	{
		m_isHoveredOver = true;
		
		//DebugRenderLog(0.f, "Hovering over: " + m_definition->m_text);
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
	CommandRunScript(m_consoleCommandOnClick.c_str());
}

//====================================================================================
UIWidgetDefinition::UIWidgetDefinition(tinyxml2::XMLElement& node)
{
	m_name = ParseXmlAttribute(node, "name", "IDK");
	m_text = ParseXmlAttribute(node, "displayText", "IDK");

	std::string filePath = ParseXmlAttribute(node, "scriptFile", "idk");

	if(filePath == "idk")
		ERROR_AND_DIE("Could not read the file path for: " + m_name);

	//m_commands = GetAllLinesFromFile(filePath.c_str());

	s_definitions.insert(std::pair<std::string,UIWidgetDefinition*>(m_name,this));

}

UIWidgetDefinition * UIWidgetDefinition::GetUIWidgetDefinition(std::string name)
{
	std::map<std::string,UIWidgetDefinition*>::iterator widgetIterator;
	widgetIterator = UIWidgetDefinition::s_definitions.find(name);
	if(widgetIterator != UIWidgetDefinition::s_definitions.end()){return widgetIterator->second;}

	return nullptr;
}
