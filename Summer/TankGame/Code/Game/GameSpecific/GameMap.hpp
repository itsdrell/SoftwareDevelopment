#pragma once
#include "Engine\Core\General\EngineCommon.hpp"
#include "Engine\Renderer\Images\Image.hpp"
#include "Engine\Math\Geometry\AABB2.hpp"
#include "Engine\Math\Ranges\FloatRange.hpp"

//=============================================================
// Forward Declare
//=============================================================
class Renderable;
class GameMap;

//=============================================================
// ENUMS
//=============================================================


//=============================================================
// Structs
//=============================================================


//=============================================================
// Classes
//=============================================================
class GameMapChunk 
{
public:
	GameMapChunk()
		: m_chunk_index(IntVector2::ZERO)
		, m_renderable(nullptr)
	{}

	~GameMapChunk() { Cleanup(); }

	void Setup( GameMap* theMap, uint idx );
	void Cleanup();
	void UpdateMesh(); 
	void SetUpRenderable();

	Vector3 GenerateTerrain( Vector2 pos ); 


public:
	GameMap*			m_map; 
	IntVector2			m_chunk_index; 
	Renderable*			m_renderable; 
}; 

//------------------------------------------------------------------------
//------------------------------------------------------------------------
class GameMap 
{
public:
	GameMap();
	~GameMap() { FreeAllChunks(); }

	//void LoadFromImage( Image const &image, 
	//	AABB2 const &extents, 
	//	float min_height, 
	//	float max_height,
	//	IntVector2 chunk_counts );

	void LoadMap(const AABB2& bounds, const FloatRange& height, const IntVector2& amountOfChunks, float stepSize);

	void CreateAllChunks();
	
	void FreeAllChunks(); 

	float CreateHeight( Vector2 xz );
	float GetHeight( Vector2 xz );

	AABB2 GetChunkExtents( IntVector2 chunk_idx );


public:
	AABB2		m_mapBounds; 
	FloatRange	m_height;
	float		m_stepSize;

	//Image m_image; 

	std::vector<Vector3>		m_points;

	std::vector<GameMapChunk*>	m_chunks; 
	IntVector2					m_chunkCount; 
	Renderable*					m_test;
}; 

//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
