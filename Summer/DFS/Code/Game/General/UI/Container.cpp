#include "Container.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Math\Geometry\AABB2.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "Game\General\UI\UIWidget.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Core\Tools\Clock.hpp"
#include "Engine\Renderer\Systems\DebugRenderSystem.hpp"


Container::Container(uint amoutOfWidgets, const Vector2& center, const AABB2& menuSize)
{
	m_amountOfWidgets = amoutOfWidgets;
	m_centerPos = center;
	m_menuSize = menuSize;
	m_menuSize.Translate(center);

	CreateWidgetSlots();

}

void Container::Update()
{
	for(uint i = 0; i < m_widgets.size(); i++)
	{
		UIWidget*& current = m_widgets.at(i);

		current->Update();
	}
}

void Container::Render() const
{
	if(m_widgets.size() == 0)
		return;
	
	Renderer* r = Renderer::GetInstance();

	r->SetCamera(r->m_defaultUICamera);
	r->BindMaterial(Material::CreateOrGetMaterial("sprite"));

	// Prepare for draw
	r->ClearDepth(1.f);
	r->EnableDepth(COMPARE_ALWAYS, true);

	for(UIWidget* currentWidget : m_widgets)
	{
		std::string name = currentWidget->GetText();
		AABB2 box = currentWidget->m_bounds;

		if(currentWidget->m_isHoveredOver)
			r->DrawAABB2(box, Rgba::YELLOW);
		else
			r->DrawAABB2(box, Rgba::BLUE);
		
		r->DrawAABB2(box, Rgba::WHITE, false);
		r->DrawFittedTextInBox(box, name, 2.f);

	}

}

bool Container::CanWeAddWidgets()
{
	// make sure we don't add on top of an already made menu
	//if(m_widgets.size() == 0)
	//	return true;

	// only spawn the max amount of widgets
	if(m_amountOfWidgets <= m_widgets.size())
		return false;

	return true;
}

void Container::OnClick()
{
	// see if a button is being hovered over

	for(uint buttonIndex = 0; buttonIndex < m_widgets.size(); buttonIndex++)
	{
		UIWidget*& current = m_widgets.at(buttonIndex);

		if(current->m_isHoveredOver)
		{
			current->OnClick();
			ClearWidgets();
			return;
		}
	}
}

void Container::AddPauseMenu()
{

	if(!CanWeAddWidgets())
		return;

	UIWidget* end = new UIWidget(*UIWidgetDefinition::GetUIWidgetDefinition("endTurn"));

	AddWidget(*end);
}

void Container::AddWidget(UIWidget& newWidget)
{
	if(CanWeAddWidgets())
	{
		uint slot = (uint) m_widgets.size();

		newWidget.m_bounds = m_widgetSlots.at(slot);
		m_widgets.push_back(&newWidget);
	}
}

void Container::CreateWidgetSlots()
{
	Vector2 dim = m_menuSize.GetDimensions();

//	float xStep = dim.x * .5f;
	float yStep = dim.y / (float)m_amountOfWidgets;

	for(uint i = 0; i < m_amountOfWidgets; i++)
	{
		AABB2 widgetBounds = m_menuSize;
		
		widgetBounds.maxs.y = m_menuSize.maxs.y - (i * yStep);
		widgetBounds.mins.y = m_menuSize.maxs.y - ((i + 1) * yStep);

		m_widgetSlots.push_back(widgetBounds);
	}
}

