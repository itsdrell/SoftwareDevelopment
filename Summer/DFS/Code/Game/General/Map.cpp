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
#include "..\..\TankGame\Code\Game\Main\GameCommon.hpp"
#include "Engine\Core\Tools\Command.hpp"
#include "Engine\Core\Utils\StringUtils.hpp"
#include "Game\General\Tiles\Tile.hpp"
#include "Engine\Core\General\HeatMap.hpp"

//====================================================================================
void DebugGrid(Command& theCommand)
{
	std::string input;
	
	if(theCommand.m_commandArguements.size() == 1)
		input = "true";
	else
		input = theCommand.m_commandArguements.at(1);
	
	bool check = ParseString(input, false);


	if(check)
	{
		// Make sure there are only one of them
		g_theGame->m_playingState->RemoveRenderable(g_theGame->m_playingState->m_currentMap->m_debugRenderable);
		g_theGame->m_playingState->AddRenderable(g_theGame->m_playingState->m_currentMap->m_debugRenderable);
		g_theGame->m_playingState->m_showHeatmap = true;
	}
	else
	{
		g_theGame->m_playingState->RemoveRenderable(g_theGame->m_playingState->m_currentMap->m_debugRenderable);
		g_theGame->m_playingState->m_showHeatmap = false;

	}
}


//====================================================================================

Map::Map(std::string name, const IntVector2 & dimensions)
{
	m_name = name;
	m_dimensions = dimensions;

	m_heatmap = new HeatMap(dimensions);

	CreateMapRenderable();
	CreateMapRenderable(true);

	CommandRegister("debugMap","Type: debugMap <bool>","Turns on debug map mode", DebugGrid);
}

Map::Map(std::string name, Image& mapImage)
{
	m_name = name;
	m_dimensions = mapImage.m_dimensions;
	m_mapImage = mapImage;

	m_heatmap = new HeatMap(m_dimensions);

	CreateMapRenderableFromImage();
	CreateMapRenderable(true);
	
	CommandRegister("debugMap","Type: debugMap <bool>","Turns on debug map mode", DebugGrid);
}

void Map::CreateMapRenderable(bool makeDebug)
{
	if(makeDebug == false)
		m_mapRenderable = new Renderable2D();
	else
		m_debugRenderable = new Renderable2D();
	
	// For now, hacky
	TileDefinition* grass = GetTileDefinition("default");
	Sprite theSprite;

	float tileSize = TILE_SIZE;

	if(makeDebug == false)
		theSprite = Sprite(*g_tileSpriteSheet.m_spriteSheetTexture, Vector2::ONE, tileSize, Vector2(.5f,.5f), grass->m_uvCoords);
	else
		theSprite = Sprite(*g_theRenderer->CreateOrGetTexture("Data/Images/Sprites/highlightCell.png"), Vector2::ONE, tileSize, Vector2(.5f,.5f));


	Vector2 currentPos = Vector2::ZERO;
	float stepSize = tileSize;

	MeshBuilder mb;
	int theTileSize = TILE_SIZE_INT;
	for(int y = 0; y < m_dimensions.y; y++)
	{
		for(int x = 0; x < m_dimensions.x; x++)
		{
			Tile newTile = Tile(currentPos.GetVector2AsInt(), *grass, theTileSize);
			m_tiles.push_back(newTile);
			
			mb.AddFromSprite(currentPos, theSprite);


			currentPos.x += stepSize;
		}

		currentPos.x = 0.f;
		currentPos.y += stepSize;
	}


	//---------------------------------------------------------
	// Create the mesh
	Mesh* theMesh = mb.CreateMesh<Vertex3D_PCU>();

	if(makeDebug == false)
	{
		m_mapRenderable->SetMesh(theMesh);

		Material* mapMat = Material::CreateOrGetMaterial("default");
		mapMat->SetTexture(0, g_tileSpriteSheet.m_spriteSheetTexture);
		m_mapRenderable->SetMaterial(mapMat);

		g_theGame->m_playingState->AddRenderable(m_mapRenderable);
	}
	else
	{
		m_debugRenderable->SetMesh(theMesh);

		Material* mapMat = Material::CreateOrGetMaterial("sprite");
		mapMat->SetTexture(0,g_theRenderer->CreateOrGetTexture("Data/Images/Sprites/highlightCell.png"));
		m_debugRenderable->SetMaterial(mapMat);
		m_debugRenderable->SetLayer(-10);

		//g_theGame->m_playingState->AddRenderable(m_debugRenderable);

	}
	


}

void Map::CreateMapRenderableFromImage()
{
	m_mapRenderable = new Renderable2D();

	float tileSize = TILE_SIZE;

	Vector2 currentPos = Vector2::ZERO;
	float stepSize = tileSize;

	MeshBuilder mb;
	int theTileSize = TILE_SIZE_INT;
	for(int y = 0; y < m_dimensions.y; y++)
	{
		for(int x = 0; x < m_dimensions.x; x++)
		{
			Rgba color = m_mapImage.GetColorAt(x,y);
			TileDefinition* currentDef = GetTileDefinition(color);

			Tile newTile = Tile(currentPos.GetVector2AsInt(), *currentDef, theTileSize);
			m_tiles.push_back(newTile);

			Sprite theSprite = Sprite(*g_tileSpriteSheet.m_spriteSheetTexture, Vector2::ONE, tileSize, Vector2(.5f,.5f), currentDef->m_uvCoords);
			mb.AddFromSprite(currentPos, theSprite);


			currentPos.x += stepSize;
		}

		currentPos.x = 0.f;
		currentPos.y += stepSize;
	}


	//---------------------------------------------------------
	// Create the mesh
	Mesh* theMesh = mb.CreateMesh<Vertex3D_PCU>();

	m_mapRenderable->SetMesh(theMesh);

	Material* mapMat = Material::CreateOrGetMaterial("default");
	mapMat->SetTexture(0, g_tileSpriteSheet.m_spriteSheetTexture);
	m_mapRenderable->SetMaterial(mapMat);

	g_theGame->m_playingState->AddRenderable(m_mapRenderable);
}

Tile* Map::GetTile(Vector2& worldPos)
{
	int Tilesize = TILE_SIZE_INT;
	
	for(uint i = 0; i < m_tiles.size(); i++)
	{
		Tile* current = &m_tiles.at(i);

		if( current->IsPointInsideTile(worldPos.GetVector2AsInt()) )
		{
			return current;
		}
	}

	return nullptr; 
}

Tile* Map::GetTile(IntVector2& tilePos)
{
	return &m_tiles.at(tilePos.y * m_dimensions.x + tilePos.x);
}

void Map::CreateMovementTiles(const Unit& theUnitToUse)
{
	m_heatmap->ResetHeatMap();
	
	// Gotta translate from world coords to tileCoords
	IntVector2 worldCoords = GetTileCoords(theUnitToUse.m_transform.GetLocalPosition());
	IntVector2 tileCoords = IntVector2(worldCoords.x / TILE_SIZE_INT, worldCoords.y / TILE_SIZE_INT);

	m_heatmap->AddHeat(tileCoords);

	std::vector<IntVector2> tilePos = m_heatmap->GetAllTileCoordsWithHeatLessOrEqual(3);
	
	for(uint i = 0; i < tilePos.size(); i++)
	{
		if(CanUnitEnterThatTile(theUnitToUse, tilePos.at(i)))
		{
			HoverTile* newTile = new HoverTile(tilePos.at(i));

			m_hoverTiles.push_back(newTile);
		}

		//HoverTile* newTile = new HoverTile(tilePos.at(i));
		//
		//m_hoverTiles.push_back(newTile);
	}
	
	
}

bool Map::CanPlayerMoveThere(IntVector2& posToCheck)
{
	int tileSize = TILE_SIZE_INT;

	// We just need to see if the tile pos we have is in our cached off list
	for(uint i = 0; i < m_hoverTiles.size(); i++)
	{
		IntVector2 current = m_hoverTiles.at(i)->m_tileCoords * tileSize;

		if(posToCheck == current)
			return true;
	}

	return false;
}

bool Map::CanUnitEnterThatTile(const Unit& theUnitToUse, IntVector2& tileToCheck)
{
	Tags unitTags = theUnitToUse.m_tags;

	Tile* currentTile = GetTile(tileToCheck);

	if(currentTile == nullptr)
		return false;

	Tags tileTags = currentTile->m_definition->m_movementTags;
	
	return DoTagsShareATag(unitTags, tileTags);
}

void Map::ClearHoverTiles()
{
	for(uint i = 0; i < m_hoverTiles.size(); i++)
	{
		g_theGame->m_playingState->RemoveRenderable(m_hoverTiles.at(i)->m_renderable);
	}

	m_hoverTiles.clear();
}

void Map::RemoveDeadGameObjects()
{
	std::vector<uint> deadindices;
	
	for(uint i = 0; i < m_gameObjects.size(); i++)
	{
		GameObject2D* current = m_gameObjects.at(i);

		if(current->m_isDead)
			deadindices.push_back(i);
	}

	for(uint j = 0; j < deadindices.size(); j++)
	{
		g_theGame->m_playingState->RemoveRenderable(m_gameObjects.at(j)->m_renderable);
		m_gameObjects.erase(m_gameObjects.begin() + deadindices.at(j));
	}
}

