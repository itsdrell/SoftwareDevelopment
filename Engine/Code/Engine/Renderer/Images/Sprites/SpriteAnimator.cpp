#include "SpriteAnimator.hpp"
#include "Engine\Core\Tools\Clock.hpp"
#include "..\..\Renderer.hpp"
#include "Engine\Renderer\Images\Sprites\Sprite.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteAnimationSet.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteAnimation.hpp"
#include "..\..\Systems\DebugRenderSystem.hpp"

//===============================================================================================
SpritePlayMode GetPlayModeFromString(String mode)
{
	if(mode == "loop") { return SPRITE_PLAY_LOOP;}
	if(mode == "clamp") { return SPRITE_PLAY_ONCE;}
	if(mode == "pingpong") { return SPRITE_PLAY_PING_PONG; }

	return SPRITE_PLAY_LOOP;
}

//===============================================================================================
//	 Sprite Animator
//-----------------------------------------------------------------------------------------------
SpriteAnimator::SpriteAnimator(const String & idNameOfAnimationSet)
{
	m_animationSet = SpriteAnimationSet::AcquireResource(idNameOfAnimationSet);

	Play("idle", SPRITE_PLAY_LOOP);
}

//-----------------------------------------------------------------------------------------------
void SpriteAnimator::Update()
{
	if(m_isPaused)
		return;

	//DebugRenderLog(1.f, std::to_string(m_timeIntoAnimation));

	if(IsDone())
	{
		if(m_currentAnimation->m_playMode == SPRITE_PLAY_LOOP)
			m_timeIntoAnimation = 0.f;
		else
		{
			// could also just stop playing. This is assuming there is an idle :(
			Play("idle", SPRITE_PLAY_LOOP);
		}
	}
	else
	{
		m_timeIntoAnimation += g_theGameClock->deltaTime;
	}
}

//-----------------------------------------------------------------------------------------------
void SpriteAnimator::Play(const String & name, SpritePlayMode loop_override)
{
	String theName = name;
	
	if( m_animationSet != nullptr )
	{
		String newName = m_animationSet->GetAnimationName( theName );
		if(newName.length() > 0)
		{
			theName = newName;
		}
	}

	m_currentAnimation = SpriteAnimation::AcquireResource( theName );
	m_timeIntoAnimation = 0.0f;

	m_currentPlayMode = loop_override;
}

//-----------------------------------------------------------------------------------------------
void SpriteAnimator::Pause()
{
	m_isPaused = true;
}

//-----------------------------------------------------------------------------------------------
void SpriteAnimator::SetTime(float time)
{
	m_timeIntoAnimation = time;
}

//-----------------------------------------------------------------------------------------------
bool SpriteAnimator::IsDone()
{
	if( m_timeIntoAnimation > m_currentAnimation->m_totalLength)
		return true;

	return false;
}

//-----------------------------------------------------------------------------------------------
Sprite* SpriteAnimator::GetCurrentSprite()
{
	if( m_currentAnimation != nullptr)
	{
		return m_currentAnimation->Evaluate( m_timeIntoAnimation );
	}
	else
	{
		// Return a default sprite (may be a leak but its a bug so shouldn't happen)
		return new Sprite(*Renderer::GetInstance()->m_defaultNormalTexture, Vector2(1.f,1.f));
	}
}
