#pragma once
#include "Engine/Math/Vectors/IntVector2.hpp"
#include "Engine/Renderer/Images/Textures/Texture.hpp"
#include "Engine/Math/Geometry/AABB2.hpp"
#include "Engine/ThirdParty/tinyxml/tinyxml2.h"

class SpriteSheet
{
public:
	SpriteSheet();
	SpriteSheet( Texture* texture, int tilesWide, int tilesHigh ); // I changed to pointer cause it was mad at me
	SpriteSheet(tinyxml2::XMLElement& node);
	
	AABB2 GetTexCoordsForSpriteCoords( const IntVector2& spriteCoords ) const; // for sprites
	AABB2 GetTexCoordsForSpriteIndex( int spriteIndex ) const; // for sprite animations
	int GetNumSprites() const;
	float GetPPU() const { return m_pixelsPerUnit; }
	IntVector2 GetSpriteLayout() const { return m_spriteLayout; }
	IntVector2 GetIndividualSpriteSize() const;

	static SpriteSheet* CreateOrGet(const std::string& filePath, IntVector2 dimensions = IntVector2());
	
	int GetIndexFromSpriteCoords( const IntVector2& spriteCoords );
	IntVector2 GetCoordsFromSpriteIndex( int index );

public:
	Texture*	m_spriteSheetTexture; 	// Texture w/grid-based layout of sprites

private:
	
	String			m_name;
	String			m_path;
	IntVector2		m_spriteLayout;		// # of sprites across, and down, on the sheet
	IntVector2		m_dimension;
	float			m_pixelsPerUnit = 16;

	static std::map<std::string, SpriteSheet*>		s_spriteSheets;
};
