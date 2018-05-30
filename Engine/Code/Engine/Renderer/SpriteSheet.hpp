#pragma once
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/AABB2.hpp"

class SpriteSheet
{
public:
	SpriteSheet();
	SpriteSheet( Texture* texture, int tilesWide, int tilesHigh ); // I changed to pointer cause it was mad at me
	AABB2 GetTexCoordsForSpriteCoords( const IntVector2& spriteCoords ) const; // for sprites
	AABB2 GetTexCoordsForSpriteIndex( int spriteIndex ) const; // for sprite animations
	int GetNumSprites() const;

public:
	Texture*	m_spriteSheetTexture; 	// Texture w/grid-based layout of sprites

private:
	
	IntVector2		m_spriteLayout;		// # of sprites across, and down, on the sheet
	IntVector2 m_dimension;
};
