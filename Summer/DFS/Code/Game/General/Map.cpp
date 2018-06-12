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
		g_theGame->m_playingState->RemoveRenderable(g_theGame->m_playingState->m_testMap->m_debugRenderable);
		g_theGame->m_playingState->AddRenderable(g_theGame->m_playingState->m_testMap->m_debugRenderable);
	}
	else
		g_theGame->m_playingState->RemoveRenderable(g_theGame->m_playingState->m_testMap->m_debugRenderable);

}


//====================================================================================

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

	if(makeDebug == false)
		theSprite = Sprite(*g_tileSpriteSheet.m_spriteSheetTexture, Vector2::ONE, 16.f, Vector2(.5f,.5f), grass->m_uvCoords);
	else
		theSprite = Sprite(*g_theRenderer->CreateOrGetTexture("Data/Images/Sprites/highlightCell.png"), Vector2::ONE, 16.f, Vector2(.5f,.5f));


	Vector2 currentPos = Vector2::ZERO;
	float stepSize = 16.f;

	MeshBuilder mb;

	for(int y = 0; y < m_dimensions.y; y++)
	{
		for(int x = 0; x < m_dimensions.x; x++)
		{
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

