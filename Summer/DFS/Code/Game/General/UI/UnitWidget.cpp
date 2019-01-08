#include "UnitWidget.hpp"
#include "Engine/Renderer/Images/Sprites/Sprite.hpp"
#include "Game/General/Maps/Map.hpp"
#include "Game/General/Player/CommandingOfficer.hpp"
#include "Engine/Core/Tools/DevConsole.hpp"
#include "../ConsoleCommands.hpp"
#include "Game\General\Maps\BattleMap.hpp"

//--------------------------------------------------------------------------
UnitWidget::UnitWidget(const TeamName & team, UnitDefinition & unitDef, UIWidgetDefinition & definition)
	: UIWidget(definition)
{
	m_unitToDisplay = &unitDef;
	m_team = team;

	CreateSprite();
}

void UnitWidget::CreateSprite()
{
	SpriteSheet spriteSheet = Unit::GetTeamTexture(m_team);
	Texture* theTexture = spriteSheet.m_spriteSheetTexture;
	
	m_sprite = new Sprite(*theTexture, Vector2::ONE, 16.f, Vector2(.5f, 0.5f), m_unitToDisplay->m_uvCoords);

}

UnitWidget::~UnitWidget()
{
	delete m_sprite;
	m_sprite = nullptr;
}

//--------------------------------------------------------------------------
void UnitWidget::Render() const
{
	Renderer* r = Renderer::GetInstance();

// 	if(m_isHoveredOver)
// 		r->DrawAABB2(m_bounds, m_hoverColor );
// 	else
// 		r->DrawAABB2(m_bounds, m_nonHoverColor);
// 	
// 	r->DrawAABB2(m_bounds, m_borderColor, false);

	// debug
	//r->DrawAABB2(m_spriteBounds, Rgba::RED);
	//r->DrawAABB2(m_unitNameBounds, Rgba::GREEN);
	//r->DrawAABB2(m_unitPriceBounds, Rgba::VIOLET);

	if(g_theBattleMap->m_currentOfficer->m_money < m_unitToDisplay->m_cost)
	{
		r->DrawTexturedAABB2(Renderer::GetInstance()->CreateOrGetTexture("Data/Images/Sprites/menuBackground.png"),m_bounds, Rgba(100,100,100,255));
	}
	else
	{
		if(m_isHoveredOver)
			r->DrawTexturedAABB2(Renderer::GetInstance()->CreateOrGetTexture("Data/Images/Sprites/menuBackground.png"),m_bounds, m_hoverColor);

		else
			r->DrawTexturedAABB2(Renderer::GetInstance()->CreateOrGetTexture("Data/Images/Sprites/menuBackground.png"),m_bounds, m_nonHoverColor);
	}
	

	r->DrawTexturedAABB2(m_spriteBounds, *m_sprite->m_image, m_sprite->m_uv.mins, m_sprite->m_uv.maxs, Rgba::WHITE);
	r->DrawFittedTextInBox(m_unitNameBounds, m_unitToDisplay->m_displayName, 1.f, 1.f, m_definition->m_defaultFontColor);
	r->DrawFittedTextInBox(m_unitPriceBounds, std::to_string(m_unitToDisplay->m_cost), 1.f, 1.f, m_definition->m_defaultFontColor);

}

void UnitWidget::GenerateBounds(const AABB2& newBounds)
{
	m_bounds = newBounds;

	m_spriteBounds =	GetBounds(newBounds, Vector2(.1f,0.f), Vector2(.2f,1.f));
	m_unitNameBounds =	GetBounds(newBounds, Vector2(.3f, 0.f), Vector2(.6f, 1.f));
	m_unitPriceBounds = GetBounds(newBounds, Vector2(.7f, 0.f), Vector2(1.f,1.f));
}

//--------------------------------------------------------------------------
void UnitWidget::OnClick()
{
	if(g_theBattleMap->m_currentOfficer->m_money < m_unitToDisplay->m_cost)
		return;
	
	if(m_definition != nullptr)
	{
		Strings& theCommands = m_definition->m_commands;

		// set the variables for the console functions
		AssignVariables();

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

//--------------------------------------------------------------------------
void UnitWidget::AssignVariables()
{
	g_unitToSpawn = m_unitToDisplay;
}
