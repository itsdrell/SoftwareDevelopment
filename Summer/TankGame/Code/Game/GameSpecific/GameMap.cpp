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
#include "Enemy.hpp"
#include "Engine\Core\General\GameObject.hpp"
#include "Engine\Math\Geometry\AABB3.hpp"
#include "Engine\Renderer\RenderableComponents\Renderable.hpp"
#include "EnemySpawner.hpp"

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
			
			Vector3 topLeft =		Vector3(pos.x, CreateHeight(Vector2(pos.x , pos.y + stepSize)), pos.y + stepSize);
			Vector3 topRight =		Vector3(pos.x + stepSize, CreateHeight(Vector2(pos.x  + stepSize, pos.y + stepSize)), pos.y + stepSize);
			Vector3 bottomLeft =	Vector3(pos.x, CreateHeight(Vector2(pos.x , pos.y )), pos.y);
			Vector3 bottomRight =	Vector3(pos.x + stepSize, CreateHeight(Vector2(pos.x  + stepSize, pos.y )), pos.y);

			m_points.push_back(bottomLeft);

			mb.AddPlaneFromFourPoints(bottomLeft, bottomRight, topRight, topLeft);
			
			pos.x += stepSize;
		}

		pos.x = bounds.mins.x;
		pos.y += stepSize;
	}

	Mesh* plane = mb.CreateMesh<VertexLit>();
	//plane->DrawAsWireFrame();
	m_test = new Renderable();

	Material* theMaterial = Material::CreateOrGetMaterial("terrain");
	m_test->SetMaterial(theMaterial);
	m_test->SetMesh(plane);

	AABB3 areaBounds = plane->m_bounds;
	//DebugRenderWireAABB3(100.f, areaBounds);

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
	float cellSizeX = floor(m_mapBounds.GetWidth() / m_stepSize);
	float cellSizeY = floor(m_mapBounds.GetHeight() / m_stepSize);

	float maxBoundsX = (m_mapBounds.mins.x) + ((cellSizeX -1) * m_stepSize);
	float maxBoundsY = (m_mapBounds.mins.y) + ((cellSizeY -1) * m_stepSize);

	// 92 is the max
	Vector2 actualMaxBounds = Vector2( maxBoundsX, maxBoundsY);
	Vector2 uv = Vector2::RangeMap(xz, m_mapBounds.mins, actualMaxBounds, Vector2::ZERO, Vector2::ONE);

	Vector2 cell_uv = Vector2( (m_chunkCount.x -1) * uv.x , (m_chunkCount.y -1) * uv.y);
	Vector2 cellFract = Vector2(GetFractionOf(cell_uv.x), GetFractionOf(cell_uv.y));
	IntVector2 bl_Index = IntVector2((int) floor(cell_uv.x), (int) floor(cell_uv.y));

	// lazy
	try 
	{
		Vector3 theBl = m_points.at( bl_Index.y * m_chunkCount.x + bl_Index.x);
		Vector3 theTl = m_points.at( (bl_Index.y + 1) * m_chunkCount.x + bl_Index.x);
		Vector3 theBr = m_points.at(bl_Index.y * m_chunkCount.x + (bl_Index.x + 1));
		Vector3 theTr = m_points.at( (bl_Index.y + 1) * m_chunkCount.x + (bl_Index.x + 1));      // vector::at throws an out-of-range

																								 // heights
		float bl = theBl.y;
		float tl = theTl.y;
		float br = theBr.y;
		float tr = theTr.y;

		//=============================================================
		// Debug
		//DebugRenderWireSphere(0.f, theBl, 1.f, DEBUG_RENDER_IGNORE_DEPTH, GetRandomColor());
		//DebugRenderWireSphere(0.f, theTl, 1.f, DEBUG_RENDER_IGNORE_DEPTH, GetRandomColor());
		//DebugRenderWireSphere(0.f, theBr, 1.f, DEBUG_RENDER_IGNORE_DEPTH, GetRandomColor());
		//DebugRenderWireSphere(0.f, theTr, 1.f, DEBUG_RENDER_IGNORE_DEPTH, GetRandomColor());
		//=============================================================

		float h1 = Interpolate(bl, br, cellFract.x);
		float h2 = Interpolate(tl, tr, cellFract.x);
		float h = Interpolate(h1 , h2, cellFract.y);

		return h + 1.f;
	}
	catch (const std::out_of_range& oor) {
		return 1.f;
	}
	
	
	

}

Vector3 GameMap::GetWorldPosition(Vector2 xz)
{
	Vector3 result = Vector3(xz.x, GetHeight(xz), xz.y);

	return result;
}

Matrix44 GameMap::GetAdjustedModelMatrix(const Vector2& pos, const Vector3& forward, const Vector3& right)
{
	Vector3 currentPos = GetWorldPosition(pos);

	// this is how far to step in that directions
	Vector3 forwardStep = Normalize(forward) * .2f;
	Vector3 rightStep = Normalize(right) * .2f;

	// We add the step to the vec2 pos to get the world pos to check
	Vector3 pointBasedOffForward = GetWorldPosition(Vector2(forwardStep.x, forwardStep.z) + pos);
	Vector3 pointBasedOffRight = GetWorldPosition(Vector2(rightStep.x, rightStep.z) + pos);

	// Create a direction based off the steps in direction - my pos
	Vector3 newForward = Normalize(pointBasedOffForward - currentPos);
	Vector3 newRight = Normalize(pointBasedOffRight - currentPos);


	Vector3 normal = Cross(newForward, newRight);

	Matrix44 result = Matrix44(newRight, normal, newForward, currentPos);

	return result;
}

AABB2 GameMap::GetChunkExtents(IntVector2 chunk_idx)
{
	return AABB2();
}

bool GameMap::Raycast(Contact3* contact, Ray3 theRay)
{
	//Vector3 abovePoint = ...;
	//Vector3 belowPoint = ...; 
	//Vector3 point = Average( abovePoint, belowPoint ); 
	//
	//uint step_count = 0; 
	//while ((Abs(DistanceFromTerrain(point)) > SOME_SMALL_VALUE) && (step_count < MAX_RAY_STEPS)) 
	//{
	//	if (IsBelow(point)) 
	//	{ 
	//		belowPoint = point; 
	//	} 
	//	else 
	//	{ 
	//		abovePoint = point; 
	//	}
	//
	//	point = Average( abovePoint, belowPoint ); 
	//
	//	// honestly would debug without this safety net - if you hit an infinite loop you've probably done it wrong; 
	//	// but it is usually a good idea to have a guaranteed exit case in loops like this in production code.
	//	step_count++;  
	//}
	//
	//contact->position = point;

	Vector3 stepSize = theRay.direction * .1f;
	Vector3 startPos = theRay.position + theRay.direction + stepSize;

	Vector3 currentPos = startPos;
	for(uint i = 0; i < (uint) MAX_RAY_STEPS; i++)
	{
		Vector3 mapPos = GetWorldPosition(currentPos.xz());

		
		if(DoesPointHitTower(currentPos))
		{
			contact->position = currentPos - stepSize;
			DebugRenderLog(0.f, "HITTING TOWER", Rgba::WHITE);
			return true;
		}
		else if(DoesPointHitAnEnemy(currentPos))
		{
			contact->position = currentPos - stepSize;
			DebugRenderLog(0.f, "HITTING ENEMY", Rgba::WHITE);
			return true;
		}
		else if(IsBelow(currentPos))
		{
			contact->position = currentPos - stepSize;
			DebugRenderLog(0.f, "HITTING TERRAIN", Rgba::WHITE);
			return true;
		}

		currentPos += stepSize;
			
	}

	return false;
}

bool GameMap::IsBelow(Vector3 point)
{
	Vector3 terrainPos = GetWorldPosition(point.xz());

	if(point.y < terrainPos.y)
		return true;

	return false;
}

bool GameMap::DoesPointHitAnEnemy(const Vector3 & point)
{
	Vector3 thePoint = point;
	
	std::vector<Enemy*>& enemies = g_theGame->m_playingState->m_enemies;

	for(uint i = 0; i < enemies.size(); i++)
	{
		Enemy& current = *enemies.at(i);

		// get the distance from point to enemy
		float distance = GetDistance(thePoint, current.m_transform.GetWorldPosition());
		
		// Get the radius
		float enemyRadius = current.m_radius;
		
		// if the distance is less than the radius its a hit
		if(distance <= enemyRadius)
			return true;
	}
	
	
	return false;
}

bool GameMap::DoesPointHitTower(const Vector3 & point)
{
	
	Vector3 thePoint = point;

	std::vector<EnemySpawner*>& spawners = g_theGame->m_playingState->m_enemySpawner;

	for(uint i = 0; i < spawners.size(); i++)
	{
		EnemySpawner& current = *spawners.at(i);

		AABB3 towerBounds = current.m_renderable->GetMesh()->m_bounds;
		DebugRenderWireAABB3(0.f,towerBounds, DEBUG_RENDER_IGNORE_DEPTH, Rgba::WHITE);
		towerBounds.Translate(current.m_transform.GetWorldMatrix());
		DebugRenderWireAABB3(0.f,towerBounds);

		if(towerBounds.IsPointInside(thePoint))
			return true;
	}
	
	return false;
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
