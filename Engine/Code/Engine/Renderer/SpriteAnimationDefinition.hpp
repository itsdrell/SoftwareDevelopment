#pragma once
#include "ThirdParty/tinyxml/tinyxml2.h"
#include "Engine/Renderer/SpriteAnimation.hpp"
#include "Engine/Core/General/EngineCommon.hpp"
#include <string>
#include <map>
#include <vector>

class SpriteSheet;
class Renderer;

class SpriteAnimationDefinition
{
	friend class SpriteAnimation;
	friend class SpriteAnimSetDefinition;

public:
	SpriteAnimationDefinition( const tinyxml2::XMLElement& animElement,
		SpriteSheet& defaultSpriteSheet, 
		float defaultFps/*Renderer& renderer*/ );

	~SpriteAnimationDefinition();

	float GetDuration() const { return (float) m_spriteIndexes.size() / m_framesPerSecond; }
	int GetSpriteIndexAtTime( float elapsedSeconds ) const;

protected:
	std::string			m_name;
	SpriteSheet*		m_spriteSheet = nullptr;
	float				m_framesPerSecond = 10.f;
	Ints				m_spriteIndexes;
	bool				m_isLooping = true;
};




/*
class SpriteAnimationDefinition
{
public:
	// We need the name from the node above the one we are given so thats why it takes a string..?
	SpriteAnimationDefinition(std::string nameOfAnimationSet, tinyxml2::XMLElement& definition);

public:
	static std::map<std::string, SpriteAnimationDefinition*>		s_definitions;

	std::map<std::string, SpriteAnimation*>							m_animations;

	std::string					m_name;
	std::string					m_filePath;
	//const SpriteSheet*			m_spriteSheet;
	SpriteAnimMode				m_animationMode;
	float						m_durationSeconds;
	Ints						m_frames;
};*/