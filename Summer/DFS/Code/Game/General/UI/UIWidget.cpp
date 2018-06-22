#include "UIWidget.hpp"
#include "Engine\Input\Mouse.hpp"
#include "Game\Main\Game.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Renderer\Systems\DebugRenderSystem.hpp"
#include "Engine\Core\General\Camera.hpp"
#include "Game\Main\Game.hpp"
#include "Game\GameStates\Playing.hpp"
#include "Game\General\Map.hpp"

UIWidget::UIWidget(const Vector2& centerPos, const AABB2& size, std::string text, bool active)
{
	m_text = text;
	m_active = active;
	m_centerPos = centerPos;
	m_bounds = size;
	m_bounds.Translate(centerPos);
	m_isHoveredOver = false;

	// Create a renderable from this info and add it to the scene?
}

UIWidget::UIWidget(std::string text, bool active /*= false*/)
{
	m_text = text;
	m_active = active;
	m_centerPos = Vector2::ONE;
	m_bounds = AABB2();
	m_isHoveredOver = false;
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
		
		DebugRenderLog(0.f, "Hovering over: " + m_text);
	}
	else
	{
		m_isHoveredOver = false;
	}

}

void UIWidget::OnClick()
{
	DebugRenderLog(3.f, m_text + ": Was Pressed!");
	g_theGame->m_playingState->m_currentMap->GoToNextTurn();
}
