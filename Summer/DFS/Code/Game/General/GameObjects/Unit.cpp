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
#include "Engine\Renderer\Images\Sprites\SpriteAnimator.hpp"
#include "..\CombatLookUpTable.hpp"
#include "Game\General\Maps\Map.hpp"
#include "Game\General\Maps\BattleMap.hpp"
#include "Game\GameStates\MapEditor.hpp"

#pragma warning( disable : 4239) // Strings parsing

//====================================================================================

std::map<std::string, UnitDefinition*> UnitDefinition::s_definitions;

//====================================================================================
Unit::Unit(TeamName team)
	: GameObject2D("Test")
{
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

//-----------------------------------------------------------------------------------------------
Unit::Unit(TeamName team, UnitDefinition & def, Map& theMapToGoOn)
	: GameObject2D("Unit")
{
	m_team = team;
	m_definition = &def;
	m_name = m_definition->m_name;


	m_animator = new SpriteAnimator(Unit::GetAnimatorName(m_name,m_team));

	//--------------------------------------------------------------------------
	Material* newMaterial = Material::CreateOrGetMaterial("sprite");
	SpriteSheet* spriteSheet = SpriteSheet::CreateOrGet("Units");
	Texture* testSprite = spriteSheet->m_spriteSheetTexture;
	newMaterial->SetTexture(0, testSprite);

	m_renderable->SetMaterial(newMaterial);
	m_renderable->SetSprite(m_animator->GetCurrentSprite());
	m_renderable->SetLayer(UNITS);

	theMapToGoOn.AddRenderable(m_renderable);
}

//-----------------------------------------------------------------------------------------------
Unit::Unit(const String& name, const TeamName team, int hp)
{
	m_definition = UnitDefinition::GetUnitDefinition(name);
	m_team = team;
	m_health = hp;
}

//-----------------------------------------------------------------------------------------------
Unit::~Unit()
{
	// definition gets cleaned up in loading
	m_tileIAmOn = nullptr; // this is not allocated so just set it to null
}

//-----------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------
STATIC String Unit::GetAnimatorName(const String& unitName, TeamName team)
{
	String result = unitName;
	
	switch (team)
	{
	case TEAM_BLUE:
		result += ("_B");
		break;
	case TEAM_RED:
		result += ("_R");
		break;
	case TEAM_GREEN:
		result += ("_G");
		break;
	case TEAM_YELLOW:
		result += ("_Y");
		break;
	default:
		result = "ERROR";
		break;
	}

	return result;
}

//-----------------------------------------------------------------------------------------------
STATIC void Unit::Attack( Unit& attacker, Unit& defender )
{
	// Attack formula in advance wars http://awbw.wikia.com/wiki/Damage_Formula
	
	// attacker - damages.y, defender - damages.x
	Vector2 damages = CombatLookUpTable::GetDamageDoneToBoth(attacker.m_definition->m_name, defender.m_definition->m_name);

	// Do damage to the defender 
	defender.m_health -= ((int)damages.x);
	
	// see if we killed the defender and exit if we did (no damage to attacker!)
	if(defender.m_health <= 0)
	{
		defender.Die();
		return;
	}
	
	// do damage to the attacker
	attacker.m_health -= ((int)damages.y);

	// check if we killed the attacker :( 
	if(attacker.m_health <= 0)
		attacker.Die();


}

//-----------------------------------------------------------------------------------------------
float Unit::GetCostForTileType(const String& tileType)
{
	return m_definition->GetMovementCost(tileType);
}

//-----------------------------------------------------------------------------------------------
void Unit::Update()
{
	if(m_health <= 0)
		m_isDead = true;

	if(m_beenMoved == false)
		m_renderable->GetMaterial()->SetTint(Rgba::WHITE);

	// till we get a cool shader so we know
	if(m_beenMoved)
	{
		Vector2 pos = m_transform.GetWorldPosition().xy();
		
		//if(m_usedAction == false)
		//	DebugRender2DQuad(0.0f, AABB2(pos.x - HALF_TILE_SIZE, pos.y - HALF_TILE_SIZE, pos.x + HALF_TILE_SIZE, pos.y + HALF_TILE_SIZE), Rgba(255,255,0,200), Rgba(255,255,0,200));
		//else
		//	DebugRender2DQuad(0.0f, AABB2(pos.x - HALF_TILE_SIZE, pos.y - HALF_TILE_SIZE, pos.x + HALF_TILE_SIZE, pos.y + HALF_TILE_SIZE), Rgba(100, 100, 100, 200), Rgba(100, 100, 100, 200));

		if(m_usedAction == false)
			m_renderable->GetMaterial()->SetTint(Rgba(150,150,150,220));
	}

	m_animator->Update();
	m_renderable->SetSprite(m_animator->GetCurrentSprite());
	
}

//-----------------------------------------------------------------------------------------------
void Unit::AddHealth(int amount)
{
	m_health += amount;

	// make sure we don't over heal
	m_health = ClampInt(m_health, 0, MAX_UNIT_HEALTH);
}

//-----------------------------------------------------------------------------------------------
void Unit::Die()
{
	m_isDead = true;

	PlayOneShot("default");

	g_theGame->m_playingState->RemoveRenderable(m_renderable);

	Vector2 pos = m_transform.GetLocalPosition() * (1 / TILE_SIZE);
	g_theBattleMap->CreateEffect("explosion", pos.GetVector2AsInt());
}

//-----------------------------------------------------------------------------------------------
float CalculateWinChance(const Unit& attacking, const Unit& defending)
{
	// Attack formula in advance wars http://awbw.wikia.com/wiki/Damage_Formula

	
	Vector2 damage = CombatLookUpTable::GetDamageDoneToBoth(attacking.m_definition->m_name, defending.m_definition->m_name);

	return damage.x;
}

//====================================================================================
UnitDefinition::UnitDefinition(tinyxml2::XMLElement & node)
{
	m_name = ParseXmlAttribute(node,"name","Error");
	m_displayName = ParseXmlAttribute(node, "displayName", "ERROR");
	m_cost = (uint) ParseXmlAttribute(node, "cost", 1000);
	m_factoryTag = ParseXmlAttribute(node, "storeType", "ERROR");
	m_spriteCoords = ParseXmlAttribute(node, "spriteCoords", IntVector2(0,0));
	
	m_canCapture = ParseXmlAttribute(node, "capture", false);
	m_movement = ParseXmlAttribute(node, "movement", 1);

	Strings a;
	m_movementTags = ParseXmlAttribute(node, "movementTags", a);
	m_attackTags = ParseXmlAttribute(node, "attackTags", a);

	m_attackRange = ParseXmlAttribute(node, "attackRange", IntRange(0,0));

	// All sprite sheets share UVs so it doesn't matter which one (yet)
	m_uvCoords = g_redUnitSpriteSheet.GetTexCoordsForSpriteCoords(m_spriteCoords);

	// Get movement stops
	tinyxml2::XMLElement* movementNode = node.FirstChildElement("MovementCosts")->FirstChildElement();
	while(movementNode)
	{
		std::string tileName = ParseXmlAttribute(*movementNode, "tile", "ERROR");
		float cost = ParseXmlAttribute(*movementNode, "cost", 1.f);

		m_movementCosts[tileName] = cost;

		movementNode = movementNode->NextSiblingElement();
	}

	// Add definition to map
	s_definitions.insert(std::pair<std::string,UnitDefinition*>(m_name,this));
}

//-----------------------------------------------------------------------------------------------
UnitDefinition* UnitDefinition::GetUnitDefinition(std::string name)
{
	std::map<std::string,UnitDefinition*>::iterator unitIterator;
	unitIterator = UnitDefinition::s_definitions.find(name);
	if(unitIterator != UnitDefinition::s_definitions.end()){ return unitIterator->second;}

	//ERROR_AND_DIE("Could not find definition");
	DevConsole::GetInstance()->AddErrorMessage("Could not find unit: " + name + " so have a grunt");
	return GetUnitDefinition("grunt");
}

//-----------------------------------------------------------------------------------------------
void UnitDefinition::GetAllUnitDefinitionsWithStoreTag(String tag, std::vector<UnitDefinition*>* list)
{
	std::map<std::string,UnitDefinition*>::iterator unitIterator;
	
	for(unitIterator = s_definitions.begin(); unitIterator != s_definitions.end(); unitIterator++)
	{
		UnitDefinition* current = unitIterator->second;

		if(current->m_factoryTag == tag)
			list->push_back(current);
	}
}

//-----------------------------------------------------------------------------------------------
void UnitDefinition::GetAllUnitDefinitions(std::vector<UnitDefinition*>* list)
{
	std::map<std::string,UnitDefinition*>::iterator unitIterator;

	for(unitIterator = s_definitions.begin(); unitIterator != s_definitions.end(); unitIterator++)
	{
		UnitDefinition* current = unitIterator->second;
		list->push_back(current);
	}
}

//-----------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------
String UnitDefinition::GetRandomUnitName()
{
	Strings names = GetAllUnitNames();

	int index = GetRandomIntRange(0, (int)(names.size() - 1U));

	return names.at(index);
}

//--------------------------------------------------------------------------
void UnitDefinition::DeleteAllDefinitions()
{
	std::map<std::string,UnitDefinition*>::iterator unitIterator;
	unitIterator = UnitDefinition::s_definitions.begin();

	while(unitIterator != UnitDefinition::s_definitions.end())
	{
		UnitDefinition* current = unitIterator->second;

		delete current;
		current = nullptr;

		unitIterator++;
	}

	s_definitions.clear();
}

//--------------------------------------------------------------------------
float UnitDefinition::GetMovementCost(const String& tileName)
{
	std::map<String,float>::iterator tileCostIterator;
	tileCostIterator = m_movementCosts.find(tileName);
	if(tileCostIterator != m_movementCosts.end()){ return tileCostIterator->second;}

	return 1.f;
}

