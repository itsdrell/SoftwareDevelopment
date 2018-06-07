#include "GameMap.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "Engine\Renderer\RenderableComponents\Renderable.hpp"
#include "Engine\Renderer\Systems\MeshBuilder.hpp"
#include "Engine\Math\Vectors\Vector2.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\ThirdParty\SquirrelNoise\SmoothNoise.hpp"
#include "..\Main\Game.hpp"
#include "Game\GameStates\Playing.hpp"
#include "Engine\Renderer\Systems\DebugRenderSystem.hpp"
#include "..\Main\GameCommon.hpp"

GameMap::GameMap()
{
}

//void GameMap::LoadFromImage(Image const & image, AABB2 const & extents, float min_height, float max_height, IntVector2 chunk_counts)
//{
//	m_image = image;
//	m_extents = extents;
//	m_min_height = min_height;
//	m_max_height = max_height;
//	m_chunk_counts = chunk_counts;
//
//	CreateAllChunks();
//}

void GameMap::LoadMap(const AABB2& bounds, const FloatRange& height, const IntVector2& amountOfChunks, float stepSize)
{
	m_mapBounds = bounds;
	m_height = height;
	m_chunkCount = amountOfChunks;
	m_stepSize = stepSize;

	MeshBuilder mb;

	// amount of steps I can make in a direction
	uint yRange = (uint) (bounds.GetHeight() / stepSize);
	uint xRange = (uint) (bounds.GetWidth() / stepSize);

	m_chunkCount = IntVector2(yRange, xRange);

	Vector2 pos = bounds.mins;

	//uint idx = 0U;
	for(uint y = 0; y < yRange; y++)
	{
		for(uint x = 0; x < xRange; x++)
		{
			//float height = GetHeight(pos);
			
			Vector3 topLeft =		Vector3(pos.x, CreateHeight(Vector2(pos.x , pos.y + stepSize)), pos.y + stepSize);
			Vector3 topRight =		Vector3(pos.x + stepSize, CreateHeight(Vector2(pos.x  + stepSize, pos.y + stepSize)), pos.y + stepSize);
			Vector3 bottomLeft =	Vector3(pos.x, CreateHeight(Vector2(pos.x , pos.y )), pos.y);
			Vector3 bottomRight =	Vector3(pos.x + stepSize, CreateHeight(Vector2(pos.x  + stepSize, pos.y )), pos.y);

			m_points.push_back(bottomLeft);

			mb.AddPlaneFromFourPoints(bottomLeft, bottomRight, topRight, topLeft);
			//DebugRenderLineSegment(100.f, topRight, bottomLeft, DEBUG_RENDER_IGNORE_DEPTH);

			//idx += 4;
			
			//mb.AddPlane(Vector3(pos.x, height, pos.y), Vector3(stepSize));
			
			pos.x += stepSize;
		}

		pos.x = bounds.mins.x;
		pos.y += stepSize;
	}

	//mb.AddPlaneFromFourPoints(Vector3(-5.f, 1.f, -5.f), Vector3(5.f, 0.f, -5.f), Vector3(5.f, 1.f, 5.f), Vector3(-5.f, 0.f, 5.f));

	Mesh* plane = mb.CreateMesh<VertexLit>();
	//plane->DrawAsWireFrame();
	m_test = new Renderable();

	Material* theMaterial = Material::CreateOrGetMaterial("terrain");
	m_test->SetMaterial(theMaterial);
	m_test->SetMesh(plane);

	g_theGame->m_playingState->AddRenderable(m_test);

}

void GameMap::CreateAllChunks()
{
	
}

void GameMap::FreeAllChunks()
{
	//m_chunks.erase(m_chunks.begin(), m_chunks.end());
}

float GameMap::CreateHeight(Vector2 xz)
{
	float n = Compute2dFractalNoise(xz.x, xz.y);

	float height = RangeMapFloat(n, -1.f, 1.f, m_height.min, m_height.max);

	return height;
}

float GameMap::GetHeight(Vector2 xz)
{
	
	Vector2 uv = Vector2::RangeMap(xz, m_mapBounds.mins, m_mapBounds.maxs, Vector2::ZERO, Vector2::ONE);

	Vector2 cell_uv = Vector2( m_chunkCount.x * uv.x , m_chunkCount.y * uv.y);
	Vector2 cellFract = Vector2(GetFractionOf(cell_uv.x), GetFractionOf(cell_uv.y));
	IntVector2 bl_Index = IntVector2((int) floor(cell_uv.x), (int) floor(cell_uv.y));

	float bl = m_points.at( bl_Index.y * m_chunkCount.x + bl_Index.x).y;
	float tl = m_points.at( (bl_Index.y + 1) * m_chunkCount.x + bl_Index.x).y;
	float br = m_points.at(bl_Index.y * m_chunkCount.x + (bl_Index.x + 1)).y;
	float tr = m_points.at( (bl_Index.y + 1) * m_chunkCount.x + (bl_Index.x + 1)).y;

	float h1 = Interpolate(bl, br, cellFract.x);
	float h2 = Interpolate(tl, tr, cellFract.x);
	float h = Interpolate(h1 , h2, cellFract.y);

	return h;

}

AABB2 GameMap::GetChunkExtents(IntVector2 chunk_idx)
{
	return AABB2();
}

//=============================================================
// void GameMapChunk::Setup(GameMap* theMap, uint idx)
// {
// 	m_map = theMap;
// 
// //	int x = index % theMap->m_chunk_counts.x;
// 	//int y = index / theMap->m_chunk_counts.x;
// 
// 	//m_chunk_index = IntVector2(x,y);
// 	
// 	SetUpRenderable();
// 	UpdateMesh();
// }
// 
// void GameMapChunk::Cleanup()
// {
// 	delete m_renderable;
// }
// 
// void GameMapChunk::UpdateMesh()
// {
// 	uint index = m_chunk_index.x * m_chunk_index.y;
// 
// 	MeshBuilder mb;
// 
// 	for(uint height = 0; height < m_chunk_index.y; height++)
// 	{
// 		for(uint width = 0; width < m_chunk_index.x; width++)
// 		{
// 			
// 			
// 			Vector3 currentPos = GenerateTerrain(Vector2::ZERO);
// 			mb.AddQuad(currentPos, AABB2());
// 		}
// 	}
// }
// 
// void GameMapChunk::SetUpRenderable()
// {
// 	m_renderable = new Renderable();
// 	
// 	Material* theMaterial = Material::CreateOrGetMaterial("terrain");
// 	m_renderable->SetMaterial(theMaterial);
// 
// }
// 
// Vector3 GameMapChunk::GenerateTerrain(Vector2 pos)
// {
// 	
// 	return Vector3(pos.x, m_map->GetHeight(pos), pos.y);
// }
