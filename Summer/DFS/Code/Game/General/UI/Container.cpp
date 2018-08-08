#include "Container.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Math\Geometry\AABB2.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "Game\General\UI\UIWidget.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Core\Tools\Clock.hpp"
#include "Engine\Renderer\Systems\DebugRenderSystem.hpp"
#include "Game\General\Map.hpp"


Container::Container(const String& name, uint amoutOfWidgets, const Vector2& center, const AABB2& menuSize)
{
	m_amountOfWidgets = amoutOfWidgets;
	m_name = name;
	m_centerPos = center;
	m_menuSize = menuSize;
	m_menuSize.Translate(center);

	m_headerBox = AABB2(Vector2(m_menuSize.mins.x, m_menuSize.maxs.y), Vector2(m_menuSize.maxs.x, m_menuSize.maxs.y + 6.f));

	CreateWidgetSlots();

	m_close = new UIWidget(*UIWidgetDefinition::GetUIWidgetDefinition("close"));
	m_close->m_bounds =  AABB2(Vector2(m_menuSize.mins.x, m_menuSize.mins.y - 6.f), Vector2(m_menuSize.maxs.x, m_menuSize.mins.y));

	m_backgroundColor = Rgba::CYAN;
	m_fontColor = Rgba::BLACK;
}

Container::~Container()
{
	ClearWidgets();
	
	delete m_close;
	m_close = nullptr;
}

void Container::Update()
{
	for(uint i = 0; i < m_widgets.size(); i++)
	{
		UIWidget*& current = m_widgets.at(i);

		current->Update();
	}

	//m_close->Update();
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

	//r->DrawAABB2(m_menuSize, m_backgroundColor);
	//r->DrawTexturedAABB2(Renderer::GetInstance()->CreateOrGetTexture("Data/Images/Sprites/menuBackground.png"),m_menuSize);

	//r->DrawAABB2(m_headerBox, Rgba::GREEN);
	//r->DrawFittedTextInBox(m_headerBox, m_name, 2.f, 1.f, m_fontColor);

	for(UIWidget* currentWidget : m_widgets)
	{

		currentWidget->Render();
	}

	//m_close->Render();
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
			CloseMenu();
			return;
		}
	}

	//if(m_close->m_isHoveredOver)
	//{
	//	m_close->OnClick();
	//	CloseMenu();
	//}
}

void Container::CloseMenu()
{
	ClearWidgets();
	if(g_theCurrentMap->m_currentContainer != nullptr) g_theCurrentMap->m_currentContainer = nullptr;
}

void Container::AddPauseMenu()
{

	if(!CanWeAddWidgets())
		return;

	UIWidget* end = new UIWidget(*UIWidgetDefinition::GetUIWidgetDefinition("endTurn"));

	AddWidget(*end);

	AddCloseWidget();
}

void Container::AddWidget(UIWidget& newWidget)
{
	if(CanWeAddWidgets())
	{
		uint slot = (uint) m_widgets.size();

		newWidget.GenerateBounds(m_widgetSlots.at(slot));
		m_widgets.push_back(&newWidget);
	}
}

void Container::ClearWidgets()
{
	for(uint i = 0; i < m_widgets.size(); i++)
	{
		UIWidget* current = m_widgets.at(i);

		delete current;
		current = nullptr;
	}

	m_widgets.clear();
}

void Container::AddCloseWidget()
{
	uint slot = (uint) m_widgets.size();
	UIWidget* newWidget = new UIWidget(*UIWidgetDefinition::GetUIWidgetDefinition("close"));
	newWidget->GenerateBounds(m_widgetSlots.at(slot));
	m_widgets.push_back(newWidget);
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

		widgetBounds.mins.x = m_menuSize.mins.x + 4.f;
		widgetBounds.maxs.x = m_menuSize.maxs.x - 4.f;

		m_widgetSlots.push_back(widgetBounds);
	}
}

