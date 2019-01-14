#include "Map.hpp"
#include "Game\General\Tiles\TileDefinition.hpp"
#include "Engine\Renderer\Images\Sprites\Sprite.hpp"
#include "Engine\Core\General\EngineCommon.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteSheet.hpp"
#include "Engine\Renderer\Systems\MeshBuilder.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "Game\Main\Game.hpp"
#include "Game\GameStates\Playing.hpp"
#include "Game\General\GameObjects\Unit.hpp"
#include "Engine\Core\General\GameObject2D.hpp"
#include "Game\General\GameObjects\Building.hpp"
#include "..\..\TankGame\Code\Game\Main\GameCommon.hpp"
#include "Engine\Core\Tools\Command.hpp"
#include "Engine\Core\Utils\StringUtils.hpp"
#include "Game\General\Tiles\Tile.hpp"
#include "Engine\Core\General\HeatMap.hpp"
#include "Game\Main\GameCommon.hpp"
#include "Engine\Core\Tools\DevConsole.hpp"
#include "Engine\Renderer\Systems\DebugRenderSystem.hpp"
#include "Game\General\Player\CommandingOfficer.hpp"
#include "Game\General\UI\Container.hpp"
#include "Game\General\UI\HUD.hpp"
#include "Game\General\UI\UIWidget.hpp"
#include "Game\General\UI\UnitWidget.hpp"
#include "Game\General\Maps\GameHeatMap.hpp"
#include "Game\General\GameObjects\Effect.hpp"
#include "Game\General\BattleScene\BattleCutscene.hpp"
#include "Game\GameStates\MapEditor.hpp"
#include "Game\General\Maps\Map.hpp"
#include "Engine\Core\Platform\File.hpp"


//====================================================================================
// Externs


//===============================================================================================
// Map
//===============================================================================================
Map::Map(std::string name, const IntVector2 & dimensions)
{
	m_name = name;
	m_dimensions = dimensions;
	m_scene = new Scene2D("Test");

	CreateMapRenderable();
	CreateMapRenderable(true);
}

Map::Map(std::string name, const Image& mapImage)
{
	m_name = name;
	m_dimensions = mapImage.m_dimensions;
	m_mapImage = mapImage;
	m_scene = new Scene2D("Test");

	CreateMapRenderableFromImage();
	CreateMapRenderable(true);
}

Map::~Map()
{
	DeleteBuildings();
	DeleteUnits();
	
	// this is deleted in the scene 
	m_mapRenderable = nullptr;

	delete m_debugRenderable;
	m_debugRenderable = nullptr;

	delete m_scene;
	m_scene = nullptr;
}

void Map::DeleteUnits()
{
	for(uint i = 0; i < m_units.size(); i++)
	{
		Unit* current = m_units.at(i);

		delete current;
		current = nullptr;
	}

	m_units.clear();
}

void Map::DeleteBuildings()
{
	for(uint i = 0; i < m_buildings.size(); i++)
	{
		Building* current = m_buildings.at(i);

		delete current;
		current = nullptr;
	}

	m_buildings.clear();
}



void Map::Update()
{
	for(uint i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects.at(i)->Update();

		// delete dead objects
		if(m_gameObjects.at(i)->m_isDead)
		{
			DeleteGameObjectFromMap(m_gameObjects.at(i));
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Map::Save(const String& levelName)
{
	Strings data;
	String fullPath = SAVED_MAP_FILE_PATH + levelName + ".script";
	String xmlReadPath = "LevelScripts/Custom/" + levelName;

	SaveTiles(data);
	SaveBuildings(data);
	SaveUnits(data);

	LogStringsToFile(fullPath.c_str(), data, true);

	String mapDefinitionEntry = "\n<Map name=\"" + levelName + "\" script=\"" + xmlReadPath + "\" />";
	LogStringToFile("Data/Definitions/Maps.xml", mapDefinitionEntry.c_str());
}

//-----------------------------------------------------------------------------------------------
void Map::SaveTiles(Strings & currentStrings)
{
	// changeTile TileDefName Pos
	String consoleCommandName = "changeTile ";

	for(uint i = 0; i < m_tiles.size(); i++)
	{
		Tile current = m_tiles.at(i);

		String currentInfo =	consoleCommandName + 
								current.m_definition->m_name + " " + 
								current.m_position.ToString();
		
		currentStrings.push_back(currentInfo);
	}
}

//-----------------------------------------------------------------------------------------------
void Map::SaveUnits(Strings & currentStrings)
{
	if(m_units.size() == 0)
		return;
	
	// addUnit UnitDefName TeamName Pos
	String consoleCommandName = "addUnit ";

	for(uint i = 0; i < m_units.size(); i++)
	{
		Unit* current = m_units.at(i);

		IntVector2 pos = (current->m_transform.GetLocalPosition() / TILE_SIZE).GetVector2AsInt();

		String currentInfo =	
			consoleCommandName + 
			current->m_definition->m_name + " " + 
			TeamNameToString(current->m_team) + " " +
			pos.ToString();

		currentStrings.push_back(currentInfo);
	}
}

//-----------------------------------------------------------------------------------------------
void Map::SaveBuildings(Strings & currentStrings)
{
	if(m_buildings.size() == 0)
		return;
	
	// addBuilding BuildingDef TeamName Pos
	String consoleCommandName = "addBuilding ";

	for(uint i = 0; i < m_units.size(); i++)
	{
		Building* current = m_buildings.at(i);

		IntVector2 pos = (current->m_transform.GetLocalPosition() / TILE_SIZE).GetVector2AsInt();

		String currentInfo =	
			consoleCommandName + 
			current->m_definition->m_name + " " + 
			TeamNameToString(current->m_team) + " " +
			pos.ToString();

		currentStrings.push_back(currentInfo);
	}
}

void Map::CreateMapRenderable(bool makeDebug)
{
	if(makeDebug == false)
		m_mapRenderable = new Renderable2D();
	else
		m_debugRenderable = new Renderable2D();
	
	// For now, hacky
	TileDefinition* grass = GetTileDefinition("water");
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
			
			if(!makeDebug)
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

		AddRenderable(m_mapRenderable);
		//if(g_theGame->m_currentState == PLAY)
		//	g_theGame->m_playingState->AddRenderable(m_mapRenderable);
		//else
		//	g_theGame->m_mapEditorState->AddRenderable(m_mapRenderable);
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

//-----------------------------------------------------------------------------------------------
void Map::RecreateMapRenderable()
{
	Vector2 currentPos = Vector2::ZERO;
	float stepSize = TILE_SIZE;

	MeshBuilder mb;
	int theTileSize = TILE_SIZE_INT;
	for(int y = 0; y < m_dimensions.y; y++)
	{
		for(int x = 0; x < m_dimensions.x; x++)
		{
			Tile* current = GetTile(currentPos);
			Sprite theSprite = Sprite(*g_tileSpriteSheet.m_spriteSheetTexture, Vector2::ONE, 
				TILE_SIZE, Vector2(.5f,.5f), current->m_definition->m_uvCoords);

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

	//g_theGame->m_mapEditorState->AddRenderable(m_mapRenderable);

}

Tile* Map::GetTile(const Vector2& worldPos)
{
	//int Tilesize = TILE_SIZE_INT;
	
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

Tile* Map::GetTile(const IntVector2& tilePos)
{
	return &m_tiles.at(tilePos.y * m_dimensions.x + tilePos.x);
}

bool Map::SelectUnit(Vector2 pos)
{
	Tile* selectedTile = GetTile(pos);

	if(selectedTile->m_unit == nullptr)
		return false;

	m_selectedUnit = selectedTile->m_unit;
	m_selectedUnit->m_tileIAmOn = selectedTile; // keep this so we know where to go back to
	selectedTile->m_unit = nullptr;

	return true;
}

void Map::PlaceUnit(Vector2 pos)
{
	Tile* selectedTile = GetTile(pos);

	selectedTile->m_unit = m_selectedUnit;
	m_selectedUnit->m_tileIAmOn->m_unit = nullptr;
	m_selectedUnit->m_tileIAmOn = selectedTile;
	m_selectedUnit->m_beenMoved = true;
	selectedTile->m_unit = m_selectedUnit;
}

void Map::PutSelectedUnitBack()
{
	Tile* tileToGoBackTo = m_selectedUnit->m_tileIAmOn;

	tileToGoBackTo->m_unit = m_selectedUnit;
	m_selectedUnit->m_transform.SetLocalPosition(tileToGoBackTo->GetCenterOfTile());

}

//-----------------------------------------------------------------------------------------------
void Map::ChangeTile(String tileDefName, IntVector2 tilePos)
{
	TileDefinition* newDefintion = GetTileDefinition(tileDefName);
	Tile* tileToChange = GetTile(tilePos);

	tileToChange->m_definition = newDefintion;

	RecreateMapRenderable();
}

//-----------------------------------------------------------------------------------------------
void Map::DeleteGameObjectFromMap(GameObject2D* objectToRemove)
{
	int ID = objectToRemove->m_ID;

	RemoveRenderable(objectToRemove->m_renderable);
	RemoveGameObject(ID);

	// this could probably be optimized but they aren't gonna be that big so its ok for now
	RemoveUnit(ID);
	RemoveBuilding(ID);

	// doing the delete here
	delete objectToRemove;
	objectToRemove = nullptr;
}

//-----------------------------------------------------------------------------------------------
void Map::RemoveGameObject(int ID)
{
	for(uint i = 0; i < m_gameObjects.size(); i++)
	{
		GameObject2D* current = m_gameObjects.at(i);

		if(current->m_ID == ID)
			RemoveFast(i, m_gameObjects);
	}
}

//-----------------------------------------------------------------------------------------------
void Map::RemoveUnit(int ID)
{
	for(uint i = 0; i < m_units.size(); i++)
	{
		Unit* current = m_units.at(i);

		if(current->m_ID == ID)
			RemoveFast(i, m_units);
	}
}

//-----------------------------------------------------------------------------------------------
void Map::RemoveBuilding(int ID)
{
	for(uint i = 0; i < m_buildings.size(); i++)
	{
		Building* current = m_buildings.at(i);

		if(current->m_ID == ID)
			RemoveFast(i, m_buildings);
	}
}

//-----------------------------------------------------------------------------------------------
Unit* Map::CreateUnit(std::string name, TeamName team, IntVector2 pos, int hp)
{
	Unit* newUnit = new Unit(team, *UnitDefinition::GetUnitDefinition(name), *this);
	Vector2 position = pos.GetAsVector2() * TILE_SIZE;
	newUnit->SetLocalPosition(position);

	newUnit->m_health = hp;

	// Put the unit on the tile
	Tile* tilePlacedOn = GetTile(position);
	tilePlacedOn->m_unit = newUnit;
	newUnit->m_tileIAmOn = tilePlacedOn;

	// make sure we know about their team
	//m_turnOrder.CheckIfTeamIsRegisteredAndAdd(team);

	AddGameObject(*newUnit);
	AddUnit(*newUnit);

	return newUnit;
}

void Map::CreateBuilding(const std::string& name, const TeamName& team, const IntVector2& pos)
{
	Building* newBuilding = new Building(team, *BuildingDefinition::GetDefinition(name), *this);
	Vector2 position = pos.GetAsVector2() * TILE_SIZE;
	newBuilding->SetLocalPosition(position);

	Tile* tilePlacedOn = GetTile(position);
	tilePlacedOn->m_building = newBuilding;

	newBuilding->m_tileReference = tilePlacedOn;

	if(tilePlacedOn->m_definition != newBuilding->m_definition->m_tileToSpawnBeneath)
	{
		tilePlacedOn->m_definition = newBuilding->m_definition->m_tileToSpawnBeneath;
		RecreateMapRenderable();
	}

	//if(team != TEAM_NONE)
	//	m_turnOrder.CheckIfTeamIsRegisteredAndAdd(team);

	AddGameObject(*newBuilding);
	AddBuilding(*newBuilding);
}

//-----------------------------------------------------------------------------------------------
void Map::CreateEffect(const String& name, const IntVector2& pos)
{
	Effect* newEffect = new Effect(name, *this);
	Vector2 position = (pos.GetAsVector2() * TILE_SIZE);
	position.y += (TILE_SIZE * .5f);
	newEffect->SetLocalPosition(position);

	AddGameObject(*newEffect);
}

