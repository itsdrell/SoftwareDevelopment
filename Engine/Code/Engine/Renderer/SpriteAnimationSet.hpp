#pragma once
#include <string>
#include <map>
#include "Engine/Math/Geometry/AABB2.hpp"



class SpriteAnimSetDefinition;
class SpriteAnimation;
class Texture;

class SpriteAnimationSet
{
public:
	SpriteAnimationSet( SpriteAnimSetDefinition* animSetDef );
	~SpriteAnimationSet();

	void 			Update( float deltaSeconds );
	void 			StartAnim( const std::string& animName );
	void 			SetCurrentAnim( const std::string& animName );
	const Texture& 	GetCurrentTexture() const;
	AABB2 			GetCurrentUVs() const;

protected:
	SpriteAnimSetDefinition*					m_animSetDef = nullptr;
	std::map< std::string, SpriteAnimation* >	m_namedAnims;
	SpriteAnimation*							m_currentAnim = nullptr;
};
