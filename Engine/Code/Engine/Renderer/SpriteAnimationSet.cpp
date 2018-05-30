#include "SpriteAnimationSet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"
#include "Engine/Renderer/SpriteAnimSetDefinition.hpp"
#include "Engine/Renderer/SpriteAnimationDefinition.hpp"
#include <string>
#include <map>

SpriteAnimationSet::SpriteAnimationSet(SpriteAnimSetDefinition * animSetDef)
{
	m_animSetDef = animSetDef;

	// Go through the def map and create those animations
	std::map<std::string, SpriteAnimationDefinition*>::iterator mapIterator = animSetDef->m_namedAnimDefs.begin();

	for(mapIterator; mapIterator != animSetDef->m_namedAnimDefs.end(); mapIterator++)
	{
		std::string keyName = mapIterator->first;
		
		SpriteAnimation* newSpriteAnim = new SpriteAnimation(mapIterator->second);

		m_namedAnims.insert(std::pair<std::string,SpriteAnimation*>(keyName,newSpriteAnim));

		// Set the default animation when we find/create it
		if(keyName == animSetDef->m_defaultAnimName){m_currentAnim = newSpriteAnim;}

		newSpriteAnim = nullptr;
		delete[] newSpriteAnim;

	}
}

SpriteAnimationSet::~SpriteAnimationSet()
{
}

void SpriteAnimationSet::Update(float deltaSeconds)
{
	m_currentAnim->Update(deltaSeconds);
}

void SpriteAnimationSet::StartAnim(const std::string & animName)
{
	SetCurrentAnim(animName);
	m_currentAnim->PlayFromStart();
}

void SpriteAnimationSet::SetCurrentAnim(const std::string & animName)
{
	std::map<std::string, SpriteAnimation*>::iterator animIterator = m_namedAnims.begin();

	for(animIterator; animIterator != m_namedAnims.end(); animIterator++)
	{
		if(animIterator->first == animName)
		{
			m_currentAnim = animIterator->second;
		}
	}
}

const Texture& SpriteAnimationSet::GetCurrentTexture() const
{
	return m_currentAnim->GetTexture();
}

AABB2 SpriteAnimationSet::GetCurrentUVs() const
{
	return m_currentAnim->GetCurrentUVs();
}
