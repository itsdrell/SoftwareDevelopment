#include "Building.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "Engine\Renderer\Images\Sprites\Sprite.hpp"
#include "..\..\TankGame\Code\Game\Main\GameCommon.hpp"
#include "Game\GameStates\Playing.hpp"
#include "Game\Main\Game.hpp"
#include "Engine\Renderer\Systems\DebugRenderSystem.hpp"
#include "Engine\Core\Utils\XmlUtilities.hpp"
#include "..\Tiles\TileDefinition.hpp"
#include "Game\General\Maps\Map.hpp"

//====================================================================================
std::map<std::string, BuildingDefinition*>	BuildingDefinition::s_definitions;
std::vector<BuildingDefinition*> BuildingDefinition::s_definitionsInOrderOfLoad;

//====================================================================================
BuildingDefinition::BuildingDefinition(tinyxml2::XMLElement& node)
{
	m_name = ParseXmlAttribute(node,"name","Error");
	m_displayName = ParseXmlAttribute(node, "displayName", "ERROR");
	m_spriteCoords = ParseXmlAttribute(node, "spriteCoords", IntVector2(0,0));

	m_isHQ = ParseXmlAttribute(node, "isFactory", false);
	m_isFactory = ParseXmlAttribute(node, "isHQ", false);
	m_typeOfUnitToSpawn = ParseXmlAttribute(node, "unitType", "NONE");

	m_tileToSpawnBeneath = GetTileDefinition(ParseXmlAttribute(node, "underTile", "default"));

	// All sprite sheets share UVs so it doesn't matter which one (yet)
	m_uvCoords = g_buildingSpriteSheet.GetTexCoordsForSpriteCoords(m_spriteCoords);

	// Add definition to map
	s_definitions.insert(std::pair<std::string,BuildingDefinition*>(m_name,this));
	s_definitionsInOrderOfLoad.push_back(this);
}

BuildingDefinition * BuildingDefinition::GetDefinition(std::string name)
{
	std::map<std::string,BuildingDefinition*>::iterator buildingIterator;
	buildingIterator = BuildingDefinition::s_definitions.find(name);
	if(buildingIterator != BuildingDefinition::s_definitions.end()){ return buildingIterator->second;}

	//ERROR_AND_DIE("Could not find definition");
	
	return nullptr;
}


Strings BuildingDefinition::GetAllBuildingNames()
{
	Strings result;

	std::map<std::string,BuildingDefinition*>::iterator unitIterator;

	for(unitIterator = s_definitions.begin(); unitIterator != s_definitions.end(); unitIterator++)
	{
		std::string current = unitIterator->second->m_name;
		result.push_back(current);
	}

	return result;
}

//--------------------------------------------------------------------------
void BuildingDefinition::DeleteAllDefinitions()
{
	std::map<std::string,BuildingDefinition*>::iterator buildingIterator;
	buildingIterator = BuildingDefinition::s_definitions.begin();

	while(buildingIterator != BuildingDefinition::s_definitions.end())
	{
		BuildingDefinition* current = buildingIterator->second;

		delete current;
		current = nullptr;

		buildingIterator++;
	}

	s_definitions.clear();
	s_definitionsInOrderOfLoad.clear();
}

//====================================================================================
Building::Building(TeamName theTeam)
	: GameObject2D("testBuilding")
{
	m_team = theTeam;


	Material* newMaterial = Material::CreateOrGetMaterial("sprite");
	Texture* testSprite = g_theRenderer->CreateOrGetTexture("Data/Images/Sprites/testBuilding.png");
	newMaterial->SetTexture(0, testSprite);

	Sprite* newSprite = new Sprite(*testSprite, Vector2::ONE, 16.f);
	newSprite->m_pixelsPerUnit = 16.f;


	m_renderable->SetMaterial(newMaterial);
	m_renderable->SetSprite(newSprite);
	m_renderable->SetLayer(BUILDINGS);

	g_theGame->m_playingState->AddRenderable(m_renderable);
}

Building::Building(const TeamName& theTeam, BuildingDefinition& def, Map& mapToPutOn)
	: GameObject2D("Building")
{
	m_team = theTeam;
	m_definition = &def;
	m_name = m_definition->m_name;

	//--------------------------------------------------------------------------
	Material* newMaterial = Material::CreateOrGetMaterial("sprite");
	Texture* buildingSprite = g_buildingSpriteSheet.m_spriteSheetTexture;
	newMaterial->SetTexture(0, buildingSprite);

	//Sprite* newSprite = new Sprite(*buildingSprite, Vector2::ONE, 16.f, Vector2(.5f, 0.5f), m_definition->m_uvCoords);
	//newSprite->m_pixelsPerUnit = 16.f;

	SetSpriteFromTeam(theTeam);

	m_renderable->SetMaterial(newMaterial);
	m_renderable->SetLayer(BUILDINGS);

	mapToPutOn.AddRenderable(m_renderable);
}

void Building::SetSpriteFromTeam(const TeamName& theTeam)
{
	AABB2 newSpriteCoords = g_buildingSpriteSheet.GetTexCoordsForSpriteCoords(IntVector2(m_definition->m_spriteCoords.x, theTeam));
	Sprite* newSprite = new Sprite(*g_buildingSpriteSheet.m_spriteSheetTexture, Vector2::ONE, 16.f, Vector2(.5f, 0.5f), newSpriteCoords);
	newSprite->m_pixelsPerUnit = 16.f;

	m_renderable->SetSprite(newSprite);
}

Building::~Building()
{
	// def gets deleted in loading
	m_tileReference = nullptr; // not allocated so just setting to null
}

void Building::Update()
{
	Vector2 pos = m_transform.GetWorldPosition().xy();
	
}

void Building::Capture(const Unit& unitTryingToCapture)
{
	m_health -= unitTryingToCapture.m_health;
	
	// we captured!
	if(m_health <= 0)
	{
		m_team = unitTryingToCapture.m_team;
		m_health = MAX_BUILDING_HEALTH;

		SetSpriteFromTeam(unitTryingToCapture.m_team);
	}
	
	
}

