#pragma once
#include "Engine\Core\General\EngineCommon.hpp"
#include "Engine\Renderer\Images\Image.hpp"
#include "Engine\Math\Geometry\AABB2.hpp"

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

	void LoadFromImage( Image const &image, 
		AABB2 const &extents, 
		float min_height, 
		float max_height,
		IntVector2 chunk_counts );

	void CreateAllChunks();
	
	void FreeAllChunks(); 

	float GetHeight( Vector2 xz );

	AABB2 GetChunkExtents( IntVector2 chunk_idx );


public:
	AABB2 m_extents; 
	float m_min_height; 
	float m_max_height; 

	Image m_image; 

	std::vector<GameMapChunk*>	m_chunks; 
	IntVector2					m_chunk_counts; 
}; 

//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
