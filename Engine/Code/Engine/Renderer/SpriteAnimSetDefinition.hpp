#pragma once
#include <map>
#include <string>
#include "ThirdParty/tinyxml/tinyxml2.h"

class Renderer;
class SpriteAnimationDefinition;

class SpriteAnimSetDefinition
{
	friend class SpriteAnimationSet;

public:
	SpriteAnimSetDefinition(tinyxml2::XMLElement& animSetElement, Renderer& renderer );
	~SpriteAnimSetDefinition();

protected:
	std::map< std::string, SpriteAnimationDefinition* >			m_namedAnimDefs;
	std::string													m_defaultAnimName = "Idle";
};
