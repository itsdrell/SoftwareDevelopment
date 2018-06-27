#include "Building.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "Engine\Renderer\Images\Sprites\Sprite.hpp"
#include "..\..\TankGame\Code\Game\Main\GameCommon.hpp"
#include "Game\GameStates\Playing.hpp"
#include "Game\Main\Game.hpp"
#include "Engine\Renderer\Systems\DebugRenderSystem.hpp"

Building::Building(TeamName theTeam)
	: GameObject2D("testBuilding")
{
	m_team = theTeam;


	Material* newMaterial = Material::CreateOrGetMaterial("sprite");
	Texture* testSprite = g_theRenderer->CreateOrGetTexture("Data/Images/Sprites/testBuilding.png");
	newMaterial->SetTexture(0, testSprite);

	Sprite* newSprite = new Sprite(*testSprite, Vector2::ONE, 16.f);
	newSprite->m_pixelsPerUnit = 16.f;


	m_renderable->SetMaterial(newMaterial);
	m_renderable->SetSprite(newSprite);
	m_renderable->SetLayer(BUILDINGS);

	g_theGame->m_playingState->AddRenderable(m_renderable);
}

void Building::Update()
{
	Vector2 pos = m_transform.GetWorldPosition().xy();

	switch (m_team)
	{
	case TEAM_NONE:
		DebugRender2DQuad(0.0f, AABB2(pos.x - HALF_TILE_SIZE, pos.y - HALF_TILE_SIZE, pos.x + HALF_TILE_SIZE, pos.y + HALF_TILE_SIZE), Rgba(128,128,128,200), Rgba(128,128,128,200));
		break;
	case TEAM_BLUE:
		DebugRender2DQuad(0.0f, AABB2(pos.x - HALF_TILE_SIZE, pos.y - HALF_TILE_SIZE, pos.x + HALF_TILE_SIZE, pos.y + HALF_TILE_SIZE), Rgba(0,0,255,200), Rgba(0,0,255,200));
		break;
	case TEAM_GREEN:
		DebugRender2DQuad(0.0f, AABB2(pos.x - HALF_TILE_SIZE, pos.y - HALF_TILE_SIZE, pos.x + HALF_TILE_SIZE, pos.y + HALF_TILE_SIZE), Rgba(0,255,0,200), Rgba(0,255,0,200));
		break;
	case TEAM_RED:
		DebugRender2DQuad(0.0f, AABB2(pos.x - HALF_TILE_SIZE, pos.y - HALF_TILE_SIZE, pos.x + HALF_TILE_SIZE, pos.y + HALF_TILE_SIZE), Rgba(255,0,0,200), Rgba(255,0,0,200));
		break;
	case TEAM_YELLOW:
		DebugRender2DQuad(0.0f, AABB2(pos.x - HALF_TILE_SIZE, pos.y - HALF_TILE_SIZE, pos.x + HALF_TILE_SIZE, pos.y + HALF_TILE_SIZE), Rgba(255, 255, 0, 200),  Rgba(255, 255, 0, 200));
		break;
	case NUM_OF_TEAMS:
		break;
	default:
		break;
	}

	
}
