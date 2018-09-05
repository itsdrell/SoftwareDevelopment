#include "Engine/Renderer/Images/Sprites/SpriteAnimationSet.hpp"
#include "Engine/Renderer/Images/Textures/Texture.hpp"
#include "Engine/Renderer/Images/Sprites/SpriteAnimation.hpp"
#include "Engine/Renderer/Images/Sprites/SpriteAnimSetDefinition.hpp"
#include "Engine/Renderer/Images/Sprites/SpriteAnimationDefinition.hpp"
#include "Engine/Core/Utils/XmlUtilities.hpp"
#include <string>
#include <map>

//===============================================================================================
std::map<String, SpriteAnimationSet*>	 SpriteAnimationSet::s_animationSets;

//===============================================================================================
// Sprite Animation Set
//-----------------------------------------------------------------------------------------------
SpriteAnimationSet::SpriteAnimationSet(tinyxml2::XMLElement & definition)
{
	m_name = ParseXmlAttribute(definition, "id", "ERROR");

	tinyxml2::XMLElement* indexElement = definition.FirstChildElement();
	while(indexElement)
	{
		std::string id = ParseXmlAttribute(*indexElement, "id", "ERROR");

		tinyxml2::XMLElement* child = indexElement->FirstChildElement();

		std::string name = ParseXmlAttribute(*child, "id", "ERROR");

		m_sets.insert(std::pair<std::string, std::string>(id,name));


		indexElement = indexElement->NextSiblingElement();
	}

	s_animationSets.insert(std::pair<std::string, SpriteAnimationSet*>(m_name,this));
}

//-----------------------------------------------------------------------------------------------
String SpriteAnimationSet::GetAnimationName(const String & nameOfAnimation)
{
	std::map<String, String>::iterator animIterator;
	animIterator = m_sets.find(nameOfAnimation);
	if(animIterator != m_sets.end()){return animIterator->second;}

	ERROR_RECOVERABLE("Could not find Animation: " + nameOfAnimation);

	return "ERROR";
}

//-----------------------------------------------------------------------------------------------
SpriteAnimationSet * SpriteAnimationSet::AcquireResource(const String & name)
{
	std::map<String,SpriteAnimationSet*>::iterator animSetIterator;
	animSetIterator = s_animationSets.find(name);
	if(animSetIterator != s_animationSets.end()){return animSetIterator->second;}

	ERROR_RECOVERABLE("Could not find Animation: " + name);

	return nullptr;
}


// SpriteAnimationSet::SpriteAnimationSet(SpriteAnimSetDefinition * animSetDef)
// {
// 	m_animSetDef = animSetDef;
// 
// 	// Go through the def map and create those animations
// 	std::map<std::string, SpriteAnimationDefinition*>::iterator mapIterator = animSetDef->m_namedAnimDefs.begin();
// 
// 	for(mapIterator; mapIterator != animSetDef->m_namedAnimDefs.end(); mapIterator++)
// 	{
// 		std::string keyName = mapIterator->first;
// 		
// 		SpriteAnimation* newSpriteAnim = new SpriteAnimation(mapIterator->second);
// 
// 		m_namedAnims.insert(std::pair<std::string,SpriteAnimation*>(keyName,newSpriteAnim));
// 
// 		// Set the default animation when we find/create it
// 		if(keyName == animSetDef->m_defaultAnimName){m_currentAnim = newSpriteAnim;}
// 
// 		newSpriteAnim = nullptr;
// 		delete[] newSpriteAnim;
// 
// 	}
// }
// 
// SpriteAnimationSet::~SpriteAnimationSet()
// {
// }
// 
// void SpriteAnimationSet::Update(float deltaSeconds)
// {
// 	m_currentAnim->Update(deltaSeconds);
// }
// 
// void SpriteAnimationSet::StartAnim(const std::string & animName)
// {
// 	SetCurrentAnim(animName);
// 	m_currentAnim->PlayFromStart();
// }
// 
// void SpriteAnimationSet::SetCurrentAnim(const std::string & animName)
// {
// 	std::map<std::string, SpriteAnimation*>::iterator animIterator = m_namedAnims.begin();
// 
// 	for(animIterator; animIterator != m_namedAnims.end(); animIterator++)
// 	{
// 		if(animIterator->first == animName)
// 		{
// 			m_currentAnim = animIterator->second;
// 		}
// 	}
// }
// 
// const Texture& SpriteAnimationSet::GetCurrentTexture() const
// {
// 	return m_currentAnim->GetTexture();
// }
// 
// AABB2 SpriteAnimationSet::GetCurrentUVs() const
// {
// 	return m_currentAnim->GetCurrentUVs();
// }


