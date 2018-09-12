#include "Engine/Renderer/Images/Sprites/SpriteAnimation.hpp"
#include "Engine/Math/Geometry/AABB2.hpp"
#include "Engine/Renderer/Images/Textures/Texture.hpp"
#include "Engine/Renderer/Images/Sprites/SpriteSheet.hpp"
#include "Engine/Renderer/Images/Sprites/SpriteAnimationDefinition.hpp"
#include "Engine/Core/Utils/XmlUtilities.hpp"
#include "Engine/Renderer/Images/Textures/Texture.hpp"
#include "Engine/Renderer/Images/Sprites/Sprite.hpp"
#include "Engine/Renderer/Images/Sprites/SpriteSheet.hpp"
#include "../../Systems/DebugRenderSystem.hpp"
#include "Engine/Math/MathUtils.hpp"

//===============================================================================================

std::map<String, SpriteAnimation*>	SpriteAnimation::s_animations;

//===============================================================================================
//	Sprite Animation
//-----------------------------------------------------------------------------------------------
SpriteAnimation::SpriteAnimation(tinyxml2::XMLElement & definition)
{
	m_totalLength = 0.f;

	m_name = ParseXmlAttribute(definition, "id", "ERROR");
	
	std::string playMode = ParseXmlAttribute(definition, "loop", "error");
	m_playMode = GetPlayModeFromString(playMode);


	tinyxml2::XMLElement* indexElement = definition.FirstChildElement();
	while(indexElement)
	{
		String currentName = indexElement->Name();

		if(currentName == "spriteSheet")
		{
			String spriteSheetName = ParseXmlAttribute(*indexElement, "name", "ERROR");
			
			m_spriteSheet = SpriteSheet::CreateOrGet(spriteSheetName);
			m_individualSpriteSize = m_spriteSheet->GetIndividualSpriteSize();
			m_pixelsPerUnit = m_spriteSheet->GetPPU();
		}

		if(currentName == "frame")
		{
			int currentFrame = ParseXmlAttribute(*indexElement, "idx", -1);
			
			if(currentFrame == -1)
			{
				IntVector2 coords = ParseXmlAttribute(*indexElement, "coords", IntVector2());
				currentFrame = m_spriteSheet->GetIndexFromSpriteCoords(coords);
			}
			
			m_frames.push_back(currentFrame);

			float length = ParseXmlAttribute(*indexElement, "time", -1.f);
			m_totalLength += length;
			m_frameLengths.push_back(length);
		}


		indexElement = indexElement->NextSiblingElement();
	}

	// this uses the frames and spritesheet that we have already to make
	// the current animation
	MakeSpritesFromFrames(); 

	s_animations.insert(std::pair<String, SpriteAnimation*>(m_name,this));
}

//-----------------------------------------------------------------------------------------------
void SpriteAnimation::MakeSpritesFromFrames()
{
	for(uint i = 0; i < m_frames.size(); i++)
	{
		AABB2 uvs = m_spriteSheet->GetTexCoordsForSpriteIndex(m_frames.at(i));

		Sprite* newSprite = new Sprite(*m_spriteSheet->m_spriteSheetTexture, m_individualSpriteSize.GetAsVector2(), m_pixelsPerUnit, Vector2(.5f,.5f), uvs);

		m_spriteFrames.push_back(newSprite);
	}
}

//-----------------------------------------------------------------------------------------------
Sprite* SpriteAnimation::Evaluate(float timeIntoAnimation)
{
	uint current = GetCurrentSpriteIndex(timeIntoAnimation);
	return m_spriteFrames.at(current);
}

//-----------------------------------------------------------------------------------------------
uint SpriteAnimation::GetCurrentSpriteIndex(float timeIntoAnimation)
{
	float range = RangeMapFloat(timeIntoAnimation, 0, m_totalLength, 0, (float) m_frameLengths.size());
	float clampedfloat = ClampFloat(range, 0, (float) m_frameLengths.size() - 1.f);
	return (uint) floorf(clampedfloat);
}

//-----------------------------------------------------------------------------------------------
SpriteAnimation * SpriteAnimation::AcquireResource(const String & name)
{
	std::map<std::string,SpriteAnimation*>::iterator animIterator;
	animIterator = SpriteAnimation::s_animations.find(name);
	if(animIterator != SpriteAnimation::s_animations.end()){return animIterator->second;}

	ERROR_RECOVERABLE("Could not find IsoSpriteAnimation Resorce: " + name);

	return nullptr;
}





//===============================================================================================
// This is from Adventure
//===============================================================================================
// SpriteAnimation::SpriteAnimation(const SpriteSheet& spriteSheet, float durationSeconds, SpriteAnimMode playbackMode, int startSpriteIndex, int endSpriteIndex)
// {
// 	m_spriteSheet = &spriteSheet;
// 	m_durationSeconds = durationSeconds;
// 	m_animationMode = playbackMode;
// 	m_startSpriteIndex = startSpriteIndex;
// 	m_endSpriteIndex = endSpriteIndex;
// 
// 	m_isPlaying = true;
// 	m_isFinished = false;
// 	m_elapsedSeconds = 0.f;
// 
// }
// 
// SpriteAnimation::SpriteAnimation(SpriteAnimationDefinition* animationDefinition)
// {
// 	m_definition = animationDefinition;
// 
// 	// These are both false since we might not instantly play it...?
// 	m_isPlaying = false;
// 	m_isFinished = false;
// 	m_elapsedSeconds = 0.0f;
// 	m_durationSeconds = 0.f;
// 	m_animationMode = SPRITE_ANIM_MODE_LOOPING; //default for now
// }
// 
// void SpriteAnimation::Update(float deltaSeconds)
// {
// 	switch (m_animationMode)
// 	{
// 	case SPRITE_ANIM_MODE_PLAY_TO_END:
// 		PlayToEndUpdate(deltaSeconds);
// 		break;
// 	case SPRITE_ANIM_MODE_LOOPING:
// 		LoopUpdate(deltaSeconds);
// 		break;
// 	case SPRITE_ANIM_MODE_PINGPONG:
// 		PingPongUpdate(deltaSeconds);
// 		break;
// 	case NUM_SPRITE_ANIM_MODES:
// 		break;
// 	default:
// 		break;
// 	}
// 
// }
// 
// AABB2 SpriteAnimation::GetCurrentTexCoords() const
// {
// 	float howFarIn = GetFractionElapsed();
// 
// 	int currentFrame = (int)((m_endSpriteIndex - m_startSpriteIndex) * howFarIn);
// //	int currentIndex = (int)((int)m_definition->m_frames.size() * howFarIn);
// 	/*return m_definition->m_spriteSheet->GetTexCoordsForSpriteIndex(m_definition->m_frames.at(currentIndex));*/
// 
// 	return AABB2();
// 
// }

// Texture* SpriteAnimation::GetTexture() const
// {
// 	/*return m_definition->m_spriteSheet->m_spriteSheetTexture;*/
// 	Texture* temp = nullptr;
// 	return temp;
// }
// 
// void SpriteAnimation::Pause()
// {
// 	m_isPlaying = false;
// }
// 
// void SpriteAnimation::Resume()
// {
// 	m_isPlaying = true;
// }
// 
// void SpriteAnimation::Reset()
// {
// 	m_elapsedSeconds = 0.f;
// }
// 
// float SpriteAnimation::GetSecondsRemaining() const
// {
// 	return (m_durationSeconds - m_elapsedSeconds);
// }
// 
// float SpriteAnimation::GetFractionElapsed() const
// {
// 	return (m_elapsedSeconds / (m_durationSeconds -1));
// }
// 
// float SpriteAnimation::GetFractionRemaining() const
// {
// 	float timeLeft = GetSecondsRemaining();
// 	return (timeLeft / m_durationSeconds);
// }
// 
// void SpriteAnimation::SetSecondsElapsed(float secondsElapsed)
// {
// 	m_elapsedSeconds = secondsElapsed;
// }
// 
// void SpriteAnimation::SetFractionElapsed(float fractionElapsed)
// {
// 	m_elapsedSeconds = m_durationSeconds * fractionElapsed;
// }
// 
// void SpriteAnimation::PlayToEndUpdate(float deltaSeconds)
// {
// 	// only increase time when playing
// 	if(m_isPlaying == true)
// 	{
// 		m_elapsedSeconds += deltaSeconds;
// 		m_isFinished = false;
// 	}
// 
// 	// Check to see if we are done playing
// 	if(m_elapsedSeconds >= m_durationSeconds)
// 	{
// 		m_isPlaying = false;
// 		m_isFinished = true;
// 		m_elapsedSeconds = m_durationSeconds - deltaSeconds; // KEEP US ON THE LAST FRAME
// 	}
// }
// 
// void SpriteAnimation::LoopUpdate(float deltaSeconds)
// {
// 	m_elapsedSeconds += deltaSeconds;
// 
// 
// 	if(m_elapsedSeconds >= m_durationSeconds)
// 	{
// 		m_elapsedSeconds = 0.f;
// 	}
// }
// 
// void SpriteAnimation::PingPongUpdate(float deltaSeconds)
// {
// 	deltaSeconds++;
// }
// 
// SpriteAnimation::SpriteAnimation(const SpriteAnimationDefinition* animDef)
// {
// 	m_animDef = animDef;
// }
// 
// SpriteAnimation::~SpriteAnimation()
// {
// }
// 
// void SpriteAnimation::Update(float deltaSeconds)
// {
// 	m_elapsedSeconds += deltaSeconds;
// }
// 
// void SpriteAnimation::PlayFromStart()
// {
// 	m_elapsedSeconds = 0.0f;
// }
// 
// float SpriteAnimation::GetElapsedFraction() const
// {
// 	float duration = m_animDef->GetDuration();
// 	
// 	return (m_elapsedSeconds / duration);
// }
// 
// float SpriteAnimation::GetRemainingSeconds() const
// {
// 	float duration = m_animDef->GetDuration();
// 	
// 	return (duration - m_elapsedSeconds);
// }
// 
// float SpriteAnimation::GetRemainingFraction() const
// {
// 	float duration = m_animDef->GetDuration();
// 
// 	float remain = duration - m_elapsedSeconds;
// 
// 	return (remain / duration);
// }
// 
// const Texture & SpriteAnimation::GetTexture() const
// {
// 	return *m_animDef->m_spriteSheet->m_spriteSheetTexture;
// }
// 
// AABB2 SpriteAnimation::GetCurrentUVs() const
// {
// 	//float howFarIn = GetElapsedFraction();
// 
// 	int currentFrame = m_animDef->GetSpriteIndexAtTime(m_elapsedSeconds);
// 	AABB2 spriteUVs = m_animDef->m_spriteSheet->GetTexCoordsForSpriteIndex(currentFrame);
// 
// 	return spriteUVs;
// }
// 
// std::string SpriteAnimation::GetName() const
// {
// 	return m_animDef->m_name;
// }

