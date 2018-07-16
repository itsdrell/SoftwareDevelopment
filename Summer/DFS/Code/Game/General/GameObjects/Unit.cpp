#include "Unit.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "..\..\TankGame\Code\Game\Main\GameCommon.hpp"
#include "Engine\Renderer\Images\Sprites\Sprite.hpp"
#include "Game\GameStates\Playing.hpp"
#include "Game\Main\Game.hpp"
#include "Engine\Renderer\Systems\DebugRenderSystem.hpp"
#include "Engine\Core\Utils\XmlUtilities.hpp"
#include "Game\Main\GameCommon.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteSheet.hpp"
#include "Engine\Core\Tools\DevConsole.hpp"

//====================================================================================

std::map<std::string, UnitDefinition*> UnitDefinition::s_definitions;

//====================================================================================
Unit::Unit(TeamName team)
	: GameObject2D("Test")
{
	m_health = 10;
	m_team = team;

	//--------------------------------------------------------------------------
	Material* newMaterial = Material::CreateOrGetMaterial("sprite");
	Texture* testSprite = g_theRenderer->CreateOrGetTexture("Data/Images/Sprites/testSprite.png");
	newMaterial->SetTexture(0, testSprite);

	Sprite* newSprite = new Sprite(*testSprite, Vector2::ONE, 16.f);
	newSprite->m_pixelsPerUnit = 16.f;


	m_renderable->SetMaterial(newMaterial);
	m_renderable->SetSprite(newSprite);
	m_renderable->SetLayer(UNITS);

	g_theGame->m_playingState->AddRenderable(m_renderable);

}

Unit::Unit(TeamName team, UnitDefinition & def)
	: GameObject2D("Unit")
{
	m_health = 10;
	m_team = team;
	m_definition = &def;
	m_name = m_definition->m_name;

	//--------------------------------------------------------------------------
	Material* newMaterial = Material::CreateOrGetMaterial("sprite");
	SpriteSheet spriteSheet = GetTeamTexture(m_team);
	Texture* testSprite = spriteSheet.m_spriteSheetTexture;
	newMaterial->SetTexture(0, testSprite);

	Sprite* newSprite = new Sprite(*testSprite, Vector2::ONE, 16.f, Vector2(.5f, 0.5f), m_definition->m_uvCoords);
	newSprite->m_pixelsPerUnit = 16.f;

	m_renderable->SetMaterial(newMaterial);
	m_renderable->SetSprite(newSprite);
	m_renderable->SetLayer(UNITS);

	g_theGame->m_playingState->AddRenderable(m_renderable);
}

STATIC SpriteSheet Unit::GetTeamTexture(TeamName name)
{
	switch (name)
	{
	case TEAM_BLUE:
		return g_blueUnitSpriteSheet;
		break;
	case TEAM_GREEN:
		return g_redUnitSpriteSheet;
		break;
	case TEAM_RED:
		return g_redUnitSpriteSheet;
		break;
	case TEAM_YELLOW:
		return g_redUnitSpriteSheet;
		break;
	default:
		return g_redUnitSpriteSheet;
		break;
	}
}

void Unit::Update()
{
	if(m_health <= 0)
		m_isDead = true;

	// till we get a cool shader so we know
	if(m_beenMoved)
	{
		Vector2 pos = m_transform.GetWorldPosition().xy();
		
		if(m_usedAction == false)
			DebugRender2DQuad(0.0f, AABB2(pos.x - HALF_TILE_SIZE, pos.y - HALF_TILE_SIZE, pos.x + HALF_TILE_SIZE, pos.y + HALF_TILE_SIZE), Rgba(255,255,0,200), Rgba(255,255,0,200));
		else
			DebugRender2DQuad(0.0f, AABB2(pos.x - HALF_TILE_SIZE, pos.y - HALF_TILE_SIZE, pos.x + HALF_TILE_SIZE, pos.y + HALF_TILE_SIZE), Rgba(100, 100, 100, 200), Rgba(100, 100, 100, 200));

	}
}

float CalculateWinChance(const Unit& attacking, const Unit& defending)
{
	UNUSED(attacking);
	UNUSED(defending);

	return 100.f;
}

//====================================================================================
UnitDefinition::UnitDefinition(tinyxml2::XMLElement & node)
{
	m_name = ParseXmlAttribute(node,"name","Error");
	m_cost = (uint) ParseXmlAttribute(node, "cost", 1000);
	m_spriteCoords = ParseXmlAttribute(node, "spriteCoords", IntVector2(0,0));
	
	m_canCapture = ParseXmlAttribute(node, "capture", false);
	m_movement = ParseXmlAttribute(node, "movement", 1);

	Strings a;
	m_movementTags = ParseXmlAttribute(node, "movementTags", a);
	m_attackTags = ParseXmlAttribute(node, "attackTags", a);

	m_attackRange = ParseXmlAttribute(node, "attackRange", IntRange(0,0));

	// All sprite sheets share UVs so it doesn't matter which one (yet)
	m_uvCoords = g_redUnitSpriteSheet.GetTexCoordsForSpriteCoords(m_spriteCoords);

	// Add definition to map
	s_definitions.insert(std::pair<std::string,UnitDefinition*>(m_name,this));
}

UnitDefinition* UnitDefinition::GetUnitDefinition(std::string name)
{
	std::map<std::string,UnitDefinition*>::iterator unitIterator;
	unitIterator = UnitDefinition::s_definitions.find(name);
	if(unitIterator != UnitDefinition::s_definitions.end()){ return unitIterator->second;}

	//ERROR_AND_DIE("Could not find definition");
	DevConsole::GetInstance()->AddErrorMessage("Could not find unit: " + name + " so have a grunt");
	return GetUnitDefinition("grunt");
}

Strings UnitDefinition::GetAllUnitNames()
{
	Strings result;
	
	std::map<std::string,UnitDefinition*>::iterator unitIterator;

	for(unitIterator = s_definitions.begin(); unitIterator != s_definitions.end(); unitIterator++)
	{
		std::string current = unitIterator->second->m_name;
		result.push_back(current);
	}

	return result;
}
