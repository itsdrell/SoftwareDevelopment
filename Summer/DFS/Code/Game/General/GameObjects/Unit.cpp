#include "Unit.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "..\..\TankGame\Code\Game\Main\GameCommon.hpp"
#include "Engine\Renderer\Images\Sprites\Sprite.hpp"
#include "Game\GameStates\Playing.hpp"
#include "Game\Main\Game.hpp"
#include "Engine\Renderer\Systems\DebugRenderSystem.hpp"

Unit::Unit(TeamName team)
	: GameObject2D("Test")
{
	m_health = 10;
	m_team = team;
	m_attackRange = IntRange(1,1);

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

	// till we get a cool shader so we know
	if(m_beenMoved)
	{
		Vector2 pos = m_transform.GetWorldPosition().xy();
		
		if(m_usedAction == false)
			DebugRender2DQuad(0.0f, AABB2(pos.x - HALF_TILE_SIZE, pos.y - HALF_TILE_SIZE, pos.x + HALF_TILE_SIZE, pos.y + HALF_TILE_SIZE), Rgba(255,255,0,200), Rgba(255,255,0,200));
		else
			DebugRender2DQuad(0.0f, AABB2(pos.x - HALF_TILE_SIZE, pos.y - HALF_TILE_SIZE, pos.x + HALF_TILE_SIZE, pos.y + HALF_TILE_SIZE), Rgba(100, 100, 100, 200), Rgba(100, 100, 100, 200));

	}
}
