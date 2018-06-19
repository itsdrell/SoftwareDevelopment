#include "Unit.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "..\..\TankGame\Code\Game\Main\GameCommon.hpp"
#include "Engine\Renderer\Images\Sprites\Sprite.hpp"
#include "Game\GameStates\Playing.hpp"
#include "Game\Main\Game.hpp"

Unit::Unit(TeamName team)
	: GameObject2D("Test")
{
	m_health = 10;
	m_team = team;

	//--------------------------------------------------------------------------
	Material* newMaterial = Material::CreateOrGetMaterial("sprite");
	Texture* testSprite = g_theRenderer->CreateOrGetTexture("Data/Images/Sprites/testSprite.png");
	newMaterial->SetTexture(0, testSprite);

	Sprite* newSprite = new Sprite(*testSprite, Vector2::ONE, 16.f);
	newSprite->m_pixelsPerUnit = 16.f;


	m_renderable->SetMaterial(newMaterial);
	m_renderable->SetSprite(newSprite);

	g_theGame->m_playingState->AddRenderable(m_renderable);

	m_tags.AddTag("ground");
}

void Unit::Update()
{
	if(m_health <= 0)
		m_isDead = true;
}
