#pragma once
#include "Engine/Math/Vectors/Vector2.hpp"
#include "Engine/Math/Geometry/AABB2.hpp"
#include "ThirdParty/tinyxml/tinyxml2.h"
#include <string>
#include <map>


//////////////////////////////////////////////////////////////////////////
class Texture;

//////////////////////////////////////////////////////////////////////////
class Sprite
{
public:
	Sprite(Texture& theTextureToUse, Vector2 pivot = Vector2(.5f,.5f), AABB2 theUVs = AABB2(0.f,0.f,1.f,1.f));
	Sprite(std::string pathOfTexture, Vector2 pivot = Vector2(.5f,.5f), AABB2 theUVs = AABB2(0.f,0.f,1.f,1.f));

	Sprite(tinyxml2::XMLElement& definition);

	static Sprite* GetSprite(std::string name);


public:
	Texture*		m_image;
	std::string		m_name;
	AABB2			m_uv;
	Vector2			m_dimensions;
	Vector2			m_pivot;
	float			m_pixelsPerUnit;


	// map of all sprite
	static std::map<std::string, Sprite*>		s_sprites;
};