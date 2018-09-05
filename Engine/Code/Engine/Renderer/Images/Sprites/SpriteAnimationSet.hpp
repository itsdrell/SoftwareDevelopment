#pragma once
#include "Engine/Core/General/EngineCommon.hpp"
#include "Engine/Core/General/Tags.hpp"
#include "Engine/ThirdParty/tinyxml/tinyxml2.h"
#include <map>

//====================================================================================
// Forward Declare
//====================================================================================


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
class SpriteAnimationSet
{
public:
	SpriteAnimationSet(tinyxml2::XMLElement& definition);

	String GetAnimationName(const String& nameOfAnimation);

	static SpriteAnimationSet* AcquireResource(const String& name);

public:
	String											m_name; // name of the actor
	std::map<String, String>						m_sets;

	static std::map<String, SpriteAnimationSet*>	s_animationSets;
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
// This is what we used in Adventure
//===============================================================================================
// class SpriteAnimSetDefinition;
// class SpriteAnimation;
// class Texture;
// 
// class SpriteAnimationSet
// {
// public:
// 	SpriteAnimationSet( SpriteAnimSetDefinition* animSetDef );
// 	~SpriteAnimationSet();
// 
// 	void 			Update( float deltaSeconds );
// 	void 			StartAnim( const std::string& animName );
// 	void 			SetCurrentAnim( const std::string& animName );
// 	const Texture& 	GetCurrentTexture() const;
// 	AABB2 			GetCurrentUVs() const;
// 
// protected:
// 	SpriteAnimSetDefinition*					m_animSetDef = nullptr;
// 	std::map< std::string, SpriteAnimation* >	m_namedAnims;
// 	SpriteAnimation*							m_currentAnim = nullptr;
// };
