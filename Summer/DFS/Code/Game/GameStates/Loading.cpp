#include "Loading.hpp"
#include "..\Main\GameCommon.hpp"
#include "Engine\Math\Geometry\AABB2.hpp"
#include "..\Main\Game.hpp"
#include "Game\GameStates\Playing.hpp"
#include "Engine\ThirdParty\tinyxml\tinyxml2.h"
#include "..\General\Tiles\TileDefinition.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteSheet.hpp"

Loading::Loading()
{
	m_readyToLoad = false;
}

void Loading::LoadAssets()
{
	// Create the global sprite sheet for all the textures to use
	Texture* TileTexture = g_theRenderer->CreateOrGetTexture("Data/Images/Terrain_8x8.png", false);
	g_tileSpriteSheet = SpriteSheet(TileTexture,8,8);
	
	LoadDefinitions();

	g_theGame->m_playingState->StartUp();

	// Make sure we see it before switching
	Sleep((DWORD) 1.f);
	g_theGame->m_currentState = ATTRACT;
}

void Loading::LoadDefinitions()
{
	LoadTileDefinitions();
}

void Loading::LoadTileDefinitions()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( "Data/Definitions/Tiles.xml" );

	tinyxml2::XMLElement* rootElement = doc.RootElement();
	GUARANTEE_OR_DIE(rootElement != nullptr, "Could not read: TileDefs");

	tinyxml2::XMLElement* indexElement = rootElement->FirstChildElement();
	while( indexElement )
	{
		TileDefinition* newDef = new TileDefinition(*indexElement);
		indexElement = indexElement->NextSiblingElement();

		// For warning
		newDef = nullptr;
		delete[] newDef;
	}
}

void Loading::Update()
{
	
	// Wait one frame before doing the load
	if(m_readyToLoad == false)
		m_readyToLoad = true;
	else
		LoadAssets();
	
}

void Loading::Render()
{
	//=============================================================
	// Set up camera
	g_theRenderer->SetCamera(g_theRenderer->m_defaultUICamera);

	// Prepare for draw
	g_theRenderer->ClearDepth(1.f);
	g_theRenderer->EnableDepth(COMPARE_ALWAYS, true);

	g_theRenderer->DrawAABB2(AABB2(-200.f, -200.f, 200.f, 200.f), Rgba::GREEN);

	//=============================================================
	float cellHeight = 8.f;
	float offsetX = -30.f;

	// Play

	g_theRenderer->DrawText2D(Vector2(offsetX, 0.f), "Loading", cellHeight, Rgba::WHITE);
}