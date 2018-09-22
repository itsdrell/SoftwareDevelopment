#pragma once
#include "Engine/Core/General/EngineCommon.hpp"

//====================================================================================
// Forward Declare
//====================================================================================
class Sprite;
class SpriteAnimationSet;
class SpriteAnimation;

//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================
enum SpritePlayMode
{
	SPRITE_PLAY_ONCE,
	SPRITE_PLAY_LOOP,
	SPRITE_PLAY_PING_PONG,
	SPRITE_NUM_OF_MODES
};
SpritePlayMode GetPlayModeFromString(String mode);

//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class SpriteAnimator
{
public:
	SpriteAnimator() {}
	SpriteAnimator(const String& idNameOfAnimationSet); // ex "Grunt" is an ID of an Animation Set

	void Update();

	void Play( const String& name, SpritePlayMode loop_override = SPRITE_PLAY_LOOP );
	void Pause();
	void SetTime(float time);
	bool IsDone();

	Sprite* GetCurrentSprite();

public:

	SpriteAnimationSet*				m_animationSet = nullptr;
	SpriteAnimation*				m_currentAnimation = nullptr;
	
	SpritePlayMode					m_currentPlayMode = SPRITE_PLAY_ONCE;
	bool							m_isPaused = false;
	float							m_timeIntoAnimation = 0.f;

};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [9/4/2018]
//====================================================================================