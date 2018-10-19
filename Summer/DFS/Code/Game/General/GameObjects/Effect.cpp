#include "Effect.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteAnimation.hpp"
#include "Engine\Core\Utils\XmlUtilities.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteSheet.hpp"
#include "Game\Main\Game.hpp"
#include "Game\Main\GameCommon.hpp"
#include "Game\GameStates\Playing.hpp"

//===============================================================================================
std::map<String, EffectDefinition*> EffectDefinition::s_definitions;

//===============================================================================================
EffectDefinition::EffectDefinition(tinyxml2::XMLElement & node)
{
	m_name = ParseXmlAttribute(node, "name", "ERROR");

	tinyxml2::XMLElement& animationNode = *node.FirstChildElement();

	SpriteAnimation* m_animation = new SpriteAnimation(animationNode);
	UNUSED(m_animation);

	s_definitions.insert(std::pair<String, EffectDefinition*>(m_name,this));
}

//-----------------------------------------------------------------------------------------------
EffectDefinition* EffectDefinition::GetEffectDefinition(String name)
{
	std::map<std::string,EffectDefinition*>::iterator effectIterator;
	effectIterator = EffectDefinition::s_definitions.find(name);
	if(effectIterator != EffectDefinition::s_definitions.end()){ return effectIterator->second;}

	ERROR_AND_DIE("Could not find Effect definition");
}

//-----------------------------------------------------------------------------------------------
Strings EffectDefinition::GetAllEffectDefinitionNames()
{
	Strings result;

	std::map<std::string,EffectDefinition*>::iterator theIterator;

	for(theIterator = s_definitions.begin(); theIterator != s_definitions.end(); theIterator++)
	{
		std::string current = theIterator->second->m_name;
		result.push_back(current);
	}

	return result;
}

//-----------------------------------------------------------------------------------------------
void EffectDefinition::DeleteAllDefinitions()
{
	std::map<std::string,EffectDefinition*>::iterator theIterator;
	theIterator = EffectDefinition::s_definitions.begin();

	while(theIterator != EffectDefinition::s_definitions.end())
	{
		EffectDefinition* current = theIterator->second;

		delete current;
		current = nullptr;

		theIterator++;
	}

	s_definitions.clear();
}

//===============================================================================================
Effect::Effect(const String& nameOfEffect)
	: GameObject2D("effect")
{
	m_definition = EffectDefinition::GetEffectDefinition(nameOfEffect);

	m_animator = new SpriteAnimator();
	SpriteAnimation* theAnimation = SpriteAnimation::AcquireResource(m_definition->m_name);
	m_animator->m_currentAnimation = theAnimation; 

	//--------------------------------------------------------------------------
	Material* newMaterial = Material::CreateOrGetMaterial("sprite");
	SpriteSheet* spriteSheet = theAnimation->m_spriteSheet;
	Texture* testSprite = spriteSheet->m_spriteSheetTexture;
	newMaterial->SetTexture(0, testSprite);

	m_renderable->SetMaterial(newMaterial);
	m_renderable->SetSprite(m_animator->GetCurrentSprite());
	m_renderable->SetLayer(EFFECTS);

	g_theGame->m_playingState->AddRenderable(m_renderable);
}

//-----------------------------------------------------------------------------------------------
void Effect::Update()
{
	m_animator->Update();
	m_renderable->SetSprite(m_animator->GetCurrentSprite());

	if(m_animator->IsDone())
		m_isDead = true;
}
