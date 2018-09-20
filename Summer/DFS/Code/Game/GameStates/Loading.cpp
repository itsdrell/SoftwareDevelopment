#include "Loading.hpp"
#include "..\Main\GameCommon.hpp"
#include "Engine\Math\Geometry\AABB2.hpp"
#include "..\Main\Game.hpp"
#include "Game\GameStates\Playing.hpp"
#include "Engine\ThirdParty\tinyxml\tinyxml2.h"
#include "..\General\Tiles\TileDefinition.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteSheet.hpp"
#include "Engine\Core\Tools\Profiling\ScopedProfile.hpp"
#include "..\General\UI\UIWidget.hpp"
#include "..\General\GameObjects\Unit.hpp"
#include "..\General\GameObjects\Building.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteAnimation.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteAnimationSet.hpp"
#include "..\General\CombatLookUpTable.hpp"
#include "..\General\Player\CommandingOfficer.hpp"

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

	g_theGame->m_playingState->StartUp();

	// Make sure we see it before switching
	Sleep((DWORD) 1.f);
	g_theGame->m_currentState = ATTRACT;
}

void Loading::LoadDefinitions()
{
	// MAKE SURE TO ADD THEM TO THE DELETE DEFINITION FUNCTION
	// AS WELL
	
	LoadTileDefinitions();
	LoadAnimationDefinitions();
	LoadWidgetDefinitions();
	LoadUnitDefinitions();
	LoadAllCODefinitions();
	LoadBuildingDefinitions();
	LoadCombatRelationships();

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

void Loading::LoadWidgetDefinitions()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( "Data/Definitions/UIWidgets.xml" );

	tinyxml2::XMLElement* rootElement = doc.RootElement();
	GUARANTEE_OR_DIE(rootElement != nullptr, "Could not read: UIWidgets");

	tinyxml2::XMLElement* indexElement = rootElement->FirstChildElement();
	while( indexElement )
	{
		UIWidgetDefinition* newDef = new UIWidgetDefinition(*indexElement);
		indexElement = indexElement->NextSiblingElement();

		// For warning
		newDef = nullptr;
		delete[] newDef;
	}
}

void Loading::LoadUnitDefinitions()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( "Data/Definitions/Units.xml" );

	tinyxml2::XMLElement* rootElement = doc.RootElement();
	GUARANTEE_OR_DIE(rootElement != nullptr, "Could not read: Unit Definitions");

	tinyxml2::XMLElement* indexElement = rootElement->FirstChildElement();
	while( indexElement )
	{
		UnitDefinition* newDef = new UnitDefinition(*indexElement);
		indexElement = indexElement->NextSiblingElement();

		// For warning
		newDef = nullptr;
		delete[] newDef;
	}
}

void Loading::LoadBuildingDefinitions()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( "Data/Definitions/Buildings.xml" );

	tinyxml2::XMLElement* rootElement = doc.RootElement();
	GUARANTEE_OR_DIE(rootElement != nullptr, "Could not read: UIWidgets");

	tinyxml2::XMLElement* indexElement = rootElement->FirstChildElement();
	while( indexElement )
	{
		BuildingDefinition* newDef = new BuildingDefinition(*indexElement);
		indexElement = indexElement->NextSiblingElement();

		// For warning
		newDef = nullptr;
		delete[] newDef;
	}
}

//-----------------------------------------------------------------------------------------------
void Loading::LoadCombatRelationships()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( "Data/Definitions/DamageRelationships.xml" );

	tinyxml2::XMLElement* rootElement = doc.RootElement();
	GUARANTEE_OR_DIE(rootElement != nullptr, "Could not read: Combat Relationships");

	tinyxml2::XMLElement* indexElement = rootElement->FirstChildElement();
	while( indexElement )
	{
		CombatLookUpTable::AddRelationshipToTable(*indexElement);
		indexElement = indexElement->NextSiblingElement();
	}
}

//-----------------------------------------------------------------------------------------------
void Loading::LoadAnimationDefinitions()
{
	// Animations first then sets
	LoadRedTeamAnimations();
	LoadRedTeamAnimationSets();
	
	LoadBlueTeamAnimations();
	LoadBlueTeamAnimationSets();
}

//-----------------------------------------------------------------------------------------------
void Loading::LoadRedTeamAnimations()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( "Data/Definitions/Animations/Red/RedSmallUnits.xml" );

	tinyxml2::XMLElement* rootElement = doc.RootElement();
	GUARANTEE_OR_DIE(rootElement != nullptr, "Could not read: Red Team Animation Definitions");

	tinyxml2::XMLElement* indexElement = rootElement->FirstChildElement();
	while( indexElement )
	{
		SpriteAnimation* newDef = new SpriteAnimation(*indexElement);
		indexElement = indexElement->NextSiblingElement();

		// For warning
		newDef = nullptr;
		delete[] newDef;
	}
}

//-----------------------------------------------------------------------------------------------
void Loading::LoadRedTeamAnimationSets()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( "Data/Definitions/Animations/Red/RedSmallUnitSets.xml" );

	tinyxml2::XMLElement* rootElement = doc.RootElement();
	GUARANTEE_OR_DIE(rootElement != nullptr, "Could not read: Red Team AnimationSet Definitions");

	tinyxml2::XMLElement* indexElement = rootElement->FirstChildElement();
	while( indexElement )
	{
		SpriteAnimationSet* newDef = new SpriteAnimationSet(*indexElement);
		indexElement = indexElement->NextSiblingElement();

		// For warning
		newDef = nullptr;
		delete[] newDef;
	}
}

void Loading::LoadBlueTeamAnimations()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( "Data/Definitions/Animations/Blue/BlueSmallUnits.xml" );

	tinyxml2::XMLElement* rootElement = doc.RootElement();
	GUARANTEE_OR_DIE(rootElement != nullptr, "Could not read: Blue Team Animation Definitions");

	tinyxml2::XMLElement* indexElement = rootElement->FirstChildElement();
	while( indexElement )
	{
		SpriteAnimation* newDef = new SpriteAnimation(*indexElement);
		indexElement = indexElement->NextSiblingElement();

		// For warning
		newDef = nullptr;
		delete[] newDef;
	}
}

void Loading::LoadBlueTeamAnimationSets()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( "Data/Definitions/Animations/Blue/BlueSmallUnitSets.xml" );

	tinyxml2::XMLElement* rootElement = doc.RootElement();
	GUARANTEE_OR_DIE(rootElement != nullptr, "Could not read: Blue Team AnimationSet Definitions");

	tinyxml2::XMLElement* indexElement = rootElement->FirstChildElement();
	while( indexElement )
	{
		SpriteAnimationSet* newDef = new SpriteAnimationSet(*indexElement);
		indexElement = indexElement->NextSiblingElement();

		// For warning
		newDef = nullptr;
		delete[] newDef;
	}
}

//-----------------------------------------------------------------------------------------------
void Loading::LoadAllCODefinitions()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( "Data/Definitions/CommandingOfficers.xml" );

	tinyxml2::XMLElement* rootElement = doc.RootElement();
	GUARANTEE_OR_DIE(rootElement != nullptr, "Could not read: Commanding Officers Definitions");

	tinyxml2::XMLElement* indexElement = rootElement->FirstChildElement();
	while( indexElement )
	{
		CommandingOfficerDefinition* newDef = new CommandingOfficerDefinition(*indexElement);
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

	Texture* blueTex = g_theRenderer->CreateOrGetTexture("Data/Images/Sprites/blueUnits.png");
	g_blueUnitSpriteSheet = SpriteSheet(blueTex, 5, 4);

	Texture* redTex = g_theRenderer->CreateOrGetTexture("Data/Images/Sprites/redUnits.png");
	g_redUnitSpriteSheet = SpriteSheet(redTex, 5, 4);

	Texture* buildingTexture = g_theRenderer->CreateOrGetTexture("Data/Images/Sprites/Buildings.png");
	g_buildingSpriteSheet = SpriteSheet(buildingTexture, 4,3);
}

void Loading::DeleteAllDefinitions()
{
	// call a static deconstructor on all definition classes we have made

	TileDefinition::DeleteAllDefinitions();
	UIWidgetDefinition::DeleteAllDefinitions();
	UnitDefinition::DeleteAllDefinitions();
	BuildingDefinition::DeleteAllDefinitions();
	CommandingOfficerDefinition::DeleteAllDefinitions();

	// TODO Delete Animation and SpriteSheets
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
