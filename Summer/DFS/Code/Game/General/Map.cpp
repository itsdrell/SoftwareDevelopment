#include "Map.hpp"
#include "Tiles\TileDefinition.hpp"
#include "Engine\Renderer\Images\Sprites\Sprite.hpp"
#include "Engine\Core\General\EngineCommon.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteSheet.hpp"
#include "Engine\Renderer\Systems\MeshBuilder.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "..\Main\Game.hpp"
#include "Game\GameStates\Playing.hpp"

Map::Map(std::string name, const IntVector2 & dimensions)
{
	m_name = name;
	m_dimensions = dimensions;

	CreateMapRenderable();
}

void Map::CreateMapRenderable()
{
	m_mapRenderable = new Renderable2D();
	
	// For now, hacky
	TileDefinition* grass = GetTileDefinition("default");
	Sprite grassSprite = Sprite(*g_tileSpriteSheet.m_spriteSheetTexture, Vector2::ONE, 16.f, Vector2(.5f,.5f), grass->m_uvCoords);

	Vector2 currentPos = Vector2::ZERO;
	float stepSize = 16.f;

	MeshBuilder mb;

	for(int y = 0; y < m_dimensions.y; y++)
	{
		for(int x = 0; x < m_dimensions.x; x++)
		{
			mb.AddFromSprite(currentPos, grassSprite);

			currentPos.x += stepSize;
		}

		currentPos.x = 0.f;
		currentPos.y += stepSize;
	}

	m_mapRenderable->SetMesh(mb.CreateMesh<Vertex3D_PCU>());

	Material* mapMat = Material::CreateOrGetMaterial("default");
	mapMat->SetTexture(0, g_tileSpriteSheet.m_spriteSheetTexture);
	m_mapRenderable->SetMaterial(mapMat);
	

	g_theGame->m_playingState->AddRenderable(m_mapRenderable);

}
