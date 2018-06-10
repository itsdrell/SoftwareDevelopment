#include "Map.hpp"
#include "Tiles\TileDefinition.hpp"
#include "Engine\Renderer\Images\Sprites\Sprite.hpp"
#include "Engine\Core\General\EngineCommon.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteSheet.hpp"
#include "Engine\Renderer\Systems\MeshBuilder.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "..\Main\Game.hpp"
#include "Game\GameStates\Playing.hpp"
#include "GameObjects\Unit.hpp"
#include "..\SystemsAndTools\GameObject2D.hpp"
#include "GameObjects\Building.hpp"

Map::Map(std::string name, const IntVector2 & dimensions)
{
	m_name = name;
	m_dimensions = dimensions;


	//---------------------------------------------------------
	// Creating a test scene
	GameObject2D* newUnit = new Unit();
	m_gameObjects.push_back(newUnit);

	GameObject2D* newBuilding = new Building();
	newBuilding->m_transform.TranslateLocal(Vector2(16.f,16.f));
	m_gameObjects.push_back(newBuilding);

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

	Mesh* theMesh = mb.CreateMesh<Vertex3D_PCU>();

	m_mapRenderable->SetMesh(theMesh);

	Material* mapMat = Material::CreateOrGetMaterial("default");
	mapMat->SetTexture(0, g_tileSpriteSheet.m_spriteSheetTexture);
	m_mapRenderable->SetMaterial(mapMat);
	

	g_theGame->m_playingState->AddRenderable(m_mapRenderable);

}

