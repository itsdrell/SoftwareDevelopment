#include "Tile.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "Game\Main\Game.hpp"
#include "Engine\Renderer\Images\Sprites\Sprite.hpp"
#include "..\..\TankGame\Code\Game\Main\GameCommon.hpp"
#include "Game\GameStates\Playing.hpp"
#include "..\Map.hpp"
//====================================================================================
Tile::Tile()
{
	m_position = IntVector2::ZERO;
	m_definition = nullptr;
	m_tileSize = 16;
}

Tile::Tile(const IntVector2 & pos, TileDefinition & def, int tileCellSize )
{
	m_position = pos;
	m_definition = &def;
	m_tileSize = tileCellSize;
}

bool Tile::IsPointInsideTile(const IntVector2& pos)
{
	int offset = (m_tileSize / 2);
	IntVector2 mins = pos - IntVector2(offset);
	IntVector2 maxs = pos + IntVector2(offset);
	
	if(m_position.x > mins.x && m_position.x < maxs.x && m_position.y > mins.y && m_position.y < maxs.y)
		return true;

	return false;
}

AABB2 Tile::GetTileBounds()
{
	int offset = (m_tileSize / 2);
	IntVector2 mins = IntVector2(-offset);
	IntVector2 maxs = IntVector2(offset);

	return AABB2(mins.GetAsVector2(), maxs.GetAsVector2());
}

//====================================================================================
HoverTile::HoverTile(IntVector2& position, HoverTileTypes theType /*= MOVEMENT_TILE_TYPE*/)
	: GameObject2D("Cursor")
{
	m_tileCoords = position;
	m_type = theType;

	//float TileSize = TILE_SIZE;
	
	switch (theType)
	{
	case MOVEMENT_TILE_TYPE:
		CreateMovementTile();
		break;
	case ATTACK_RANGE_TILE_TYPE:
		CreateAttackTile();
		break;
	case NUM_OF_HOVER_TILE_TYPES:
		break;
	default:
		break;
	}
	
}

HoverTile::~HoverTile()
{
	g_theGame->m_playingState->RemoveRenderable(m_renderable);
}

void HoverTile::CreateMovementTile()
{
	Material* newMaterial = Material::CreateOrGetMaterial("sprite");
	Texture* testSprite = g_theRenderer->CreateOrGetTexture("Data/Images/Sprites/movementHover.png");
	newMaterial->SetTexture(0, testSprite);

	Sprite* newSprite = new Sprite(*testSprite, Vector2::ONE, TILE_SIZE);
	newSprite->m_pixelsPerUnit = TILE_SIZE;

	m_transform.SetLocalPosition(m_tileCoords.GetAsVector2() * TILE_SIZE);

	m_renderable->SetMaterial(newMaterial);
	m_renderable->SetSprite(newSprite);
	m_renderable->SetLayer(HOVER_TILES);

	g_theGame->m_playingState->AddRenderable(m_renderable);
}

void HoverTile::CreateAttackTile()
{
	Material* newMaterial = Material::CreateOrGetMaterial("sprite");
	Texture* testSprite = g_theRenderer->CreateOrGetTexture("Data/Images/Sprites/target.png");
	newMaterial->SetTexture(0, testSprite);

	Sprite* newSprite = new Sprite(*testSprite, Vector2::ONE, TILE_SIZE);
	newSprite->m_pixelsPerUnit = TILE_SIZE;

	m_transform.SetLocalPosition(m_tileCoords.GetAsVector2() * TILE_SIZE);

	m_renderable->SetMaterial(newMaterial);
	m_renderable->SetSprite(newSprite);
	m_renderable->SetLayer(HOVER_TILES);

	g_theGame->m_playingState->AddRenderable(m_renderable);
}
