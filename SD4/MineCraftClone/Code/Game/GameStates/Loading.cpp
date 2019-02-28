#include "Loading.hpp"
#include "..\Main\GameCommon.hpp"
#include "Engine\Math\Geometry\AABB2.hpp"
#include "..\Main\Game.hpp"
#include "Game\GameStates\Playing.hpp"
#include "Engine\ThirdParty\tinyxml\tinyxml2.h"
#include "Engine\Renderer\Images\Sprites\SpriteSheet.hpp"
#include "Engine\Core\Tools\Profiling\ScopedProfile.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteAnimation.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteAnimationSet.hpp"
#include "Game\General\World\BlockDefinition.hpp"


Loading::Loading()
{
	m_readyToLoad = false;
}

Loading::~Loading()
{
	// Since I do not have an asset data base, Loading will be in charge of 
	// deleting globals and definitions and other assets creating at the beginning 

	DeleteAllDefinitions();

}

void Loading::LoadAssets()
{
	ScopedProfile loadTime = ScopedProfile("Load time");
	g_theAudioSystem->StartUp();
	
	
	LoadSpriteSheets();
	LoadDefinitions();

	// Make sure we see it before switching
	Sleep((DWORD) 1.f);
	g_theGame->m_currentState = ATTRACT;
}

void Loading::LoadDefinitions()
{
	// MAKE SURE TO ADD THEM TO THE DELETE DEFINITION FUNCTION
	// AS WELL
	LoadBlockDefinitions();
}


//-----------------------------------------------------------------------------------------------
void Loading::LoadBlockDefinitions()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( "Data/Definitions/Blocks.xml" );

	tinyxml2::XMLElement* rootElement = doc.RootElement();
	GUARANTEE_OR_DIE(rootElement != nullptr, "Could not read: BlockDefs");

	tinyxml2::XMLElement* indexElement = rootElement->FirstChildElement();
	while( indexElement )
	{
		BlockDefinition* newDef = new BlockDefinition(*indexElement);
		indexElement = indexElement->NextSiblingElement();

		// For warning
		newDef = nullptr;
		delete[] newDef;
	}
}

//-----------------------------------------------------------------------------------------------
void Loading::LoadSpriteSheets()
{
	
	tinyxml2::XMLDocument doc;
	doc.LoadFile( "Data/Definitions/SpriteSheets.xml" );

	tinyxml2::XMLElement* rootElement = doc.RootElement();
	GUARANTEE_OR_DIE(rootElement != nullptr, "Could not read: Sprite sheet xml");

	tinyxml2::XMLElement* indexElement = rootElement->FirstChildElement();
	while( indexElement )
	{
		SpriteSheet* newDef = new SpriteSheet(*indexElement);
		indexElement = indexElement->NextSiblingElement();

		// For warning
		newDef = nullptr;
		delete[] newDef;
	}
	
	
	// Legacy stuff
	// Create the global sprite sheet for all the textures to use
	Texture* TileTexture = g_theRenderer->CreateOrGetTexture("Data/Images/Sprites/tileSpriteSheet.png");
	g_tileSpriteSheet = SpriteSheet(TileTexture,27,3);

}

void Loading::DeleteAllDefinitions()
{
	// call a static deconstructor on all definition classes we have made
	BlockDefinition::DeleteDefinitions();
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
