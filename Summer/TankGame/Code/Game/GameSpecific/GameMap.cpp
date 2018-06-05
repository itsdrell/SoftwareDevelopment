#include "GameMap.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "Engine\Renderer\RenderableComponents\Renderable.hpp"
#include "Engine\Renderer\Systems\MeshBuilder.hpp"
#include "Engine\Math\Vectors\Vector2.hpp"
#include "Engine\Math\MathUtils.hpp"

GameMap::GameMap()
{
}

void GameMap::LoadFromImage(Image const & image, AABB2 const & extents, float min_height, float max_height, IntVector2 chunk_counts)
{
	m_image = image;
	m_extents = extents;
	m_min_height = min_height;
	m_max_height = max_height;
	m_chunk_counts = chunk_counts;

	CreateAllChunks();
}

void GameMap::CreateAllChunks()
{
	uint amount = m_chunk_counts.x * m_chunk_counts.y;

	for(uint i = 0; i < amount; i++)
	{
		GameMapChunk* newChunk = new GameMapChunk();

		newChunk->Setup(this, i);

		m_chunks.push_back(newChunk);
	}
}

void GameMap::FreeAllChunks()
{
	m_chunks.erase(m_chunks.begin(), m_chunks.end());
}

float GameMap::GetHeight(Vector2 xz)
{
	float rChannel = (float) m_image.GetColorAt(xz.GetVector2AsInt()).r;

	float height = RangeMapFloat(rChannel, 0.f, 255.f,m_min_height, m_max_height);
	
	return height;
}

AABB2 GameMap::GetChunkExtents(IntVector2 chunk_idx)
{
	return AABB2();
}

//=============================================================
void GameMapChunk::Setup(GameMap* theMap, uint idx)
{
	m_map = theMap;

	int index = (int) idx;
	int x = index % theMap->m_chunk_counts.x;
	int y = index / theMap->m_chunk_counts.x;

	m_chunk_index = IntVector2(x,y);
	
	SetUpRenderable();
	UpdateMesh();
}

void GameMapChunk::Cleanup()
{
	delete m_renderable;
}

void GameMapChunk::UpdateMesh()
{
	uint index = m_chunk_index.x * m_chunk_index.y;

	MeshBuilder mb;

	for(uint height = 0; height < m_chunk_index.y; height++)
	{
		for(uint width = 0; width < m_chunk_index.x; width++)
		{
			
			
			Vector3 currentPos = GenerateTerrain(Vector2::ZERO);
			mb.AddQuad(currentPos, AABB2());
		}
	}
}

void GameMapChunk::SetUpRenderable()
{
	m_renderable = new Renderable();
	
	Material* theMaterial = Material::CreateOrGetMaterial("terrain");
	m_renderable->SetMaterial(theMaterial);

}

Vector3 GameMapChunk::GenerateTerrain(Vector2 pos)
{
	
	return Vector3(pos.x, m_map->GetHeight(pos), pos.y);
}
