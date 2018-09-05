#pragma once
#include "Engine/Renderer/Images/Sprites/SpriteAnimator.hpp"
#include "Engine/Math/Vectors/IntVector2.hpp"
#include "Engine/ThirdParty/tinyxml/tinyxml2.h"
#include <string>
#include <map>

//====================================================================================
// Forward Declare
//====================================================================================
class SpriteSheet;

//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class SpriteAnimation
{
public:
	SpriteAnimation(tinyxml2::XMLElement& definition);
	
	// This is taking the frames from XML and making sprites from the sprite sheet
	// that we also got from XML so that our SpriteRenderer can just copy them!
	void MakeSpritesFromFrames();

	Sprite* Evaluate(float timeIntoAnimation);

	static SpriteAnimation* AcquireResource(const String& name);

public:

	SpriteSheet*			m_spriteSheet;
	SpritePlayMode			m_playMode;
	String					m_name;

	Ints					m_frames; // keeping these even tho we don't have to
	std::vector<float>		m_frameLengths;
	float					m_totalLength;

	// These are more sprite specific data, but since we are making the sprites here,
	// We need to know this data
	IntVector2				m_dimensions;
	float					m_pixelsPerUnit = 16.f;

	// we don't need actual sprites made if we know the sprite sheet and index
	std::vector<Sprite*>	m_spriteFrames;

	static std::map<String, SpriteAnimation*>	s_animations;
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



//===============================================================================================
// This is the version from Adventure
//===============================================================================================
// class SpriteSheet;
// class AABB2;
// class Texture;
// class SpriteAnimationDefinition;
// 
// enum SpriteAnimMode
// {
// 	SPRITE_ANIM_MODE_PLAY_TO_END,	// Play from time=0 to durationSeconds, then finish
// 	SPRITE_ANIM_MODE_LOOPING,		// Play from time=0 to end then repeat (never finish)
//     SPRITE_ANIM_MODE_PINGPONG, 		// optional, play forwards, backwards, forwards...
// 	NUM_SPRITE_ANIM_MODES
// };
// 
// class SpriteAnimation
// {
// public:
// 	SpriteAnimation( const SpriteAnimationDefinition* animDef );
// 	~SpriteAnimation();
// 
// 	void Update( float deltaSeconds );
// 	void PlayFromStart();
// 	bool IsFinished() const { return m_isFinished; }
// 	float GetElapsedSeconds() const { return m_elapsedSeconds; }
// 	float GetElapsedFraction() const; // Hint: Asks its SpriteAnimDefinition for total duration
// 	float GetRemainingSeconds() const;
// 	float GetRemainingFraction() const;
// 	const Texture& GetTexture() const;
// 	AABB2 GetCurrentUVs() const;
// 	std::string GetName() const;
// 
// protected:
// 	const SpriteAnimationDefinition*		m_animDef = nullptr;
// 	bool									m_isPlaying = true;
// 	bool									m_isFinished = false;
// 	float									m_elapsedSeconds = 0.f;
// };




// 
// class SpriteAnimation
// {
// public:
// // 	SpriteAnimation( const SpriteSheet& spriteSheet, float durationSeconds,
// // 		SpriteAnimMode playbackMode, int startSpriteIndex, int endSpriteIndex );
// 
// 	SpriteAnimation(SpriteAnimationDefinition* animationDefinition);
// 
// 	void Update( float deltaSeconds );
// 	AABB2 GetCurrentTexCoords() const;								// Based on the current elapsed time
// 	Texture* GetTexture() const;
// 	void Pause();													// Starts unpaused (playing) by default
// 	void Resume();													// Resume after pausing
// 	void Reset();													// Rewinds to time 0 and starts (re)playing
// 	bool IsFinished() const			{ return m_isFinished; }
// 	bool IsPlaying() const			{ return m_isPlaying; }
// 	float GetDurationSeconds() const	{ return m_durationSeconds; }
// 	float GetSecondsElapsed() const		{ return m_elapsedSeconds; }
// 	float GetSecondsRemaining() const;
// 	float GetFractionElapsed() const;
// 	float GetFractionRemaining() const;
// 	void SetSecondsElapsed( float secondsElapsed );					 // Jump to specific time
// 	void SetFractionElapsed( float fractionElapsed );				// e.g. 0.33f for one-third in
// 	void PlayToEndUpdate(float deltaSeconds);
// 	void LoopUpdate(float deltaSeconds);
// 	void PingPongUpdate(float deltaSeconds);
// 
// public:
// 	SpriteAnimationDefinition*			m_definition;
// 	
// 	
// 	bool m_isFinished;
// 	bool m_isPlaying;
// 	float m_durationSeconds;
// 	float m_elapsedSeconds;
// 
// /*	const SpriteSheet* m_spriteSheet;*/
// 	SpriteAnimMode m_animationMode;
//  	//int m_startSpriteIndex;
//  	//int m_endSpriteIndex;
// 	
// };

