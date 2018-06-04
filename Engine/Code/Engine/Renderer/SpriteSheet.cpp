#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "../Core/General/EngineCommon.hpp"


SpriteSheet::SpriteSheet()
{
}

SpriteSheet::SpriteSheet(Texture* texture, int tilesWide, int tilesHigh)
{
	m_spriteSheetTexture = texture;
	m_spriteLayout = IntVector2(tilesWide,tilesHigh);
	m_dimension = m_spriteSheetTexture->GetDimensions();
}

AABB2 SpriteSheet::GetTexCoordsForSpriteCoords(const IntVector2& spriteCoords) const
{
	
	// Create min 
	float minIndexX = (float)(spriteCoords.x);
	float minIndexY = (float)(spriteCoords.y);
	
	// Get fraction over
	float fractionX = minIndexX / (float)m_spriteLayout.x;
	float fractionY = minIndexY / (float)m_spriteLayout.y;
	
	// Get position
	float minx = fractionX;
	float miny = fractionY;

	// Create Max
	float maxIndexX = (float)(spriteCoords.x + 1);
	float maxIndexY = (float)(spriteCoords.y + 1);
	
	fractionX = (maxIndexX / m_spriteLayout.x);
	fractionY = (maxIndexY / m_spriteLayout.y);

	float maxx = fractionX;
	float maxy = fractionY;

	return AABB2(Vector2(minx,miny),Vector2(maxx,maxy));
}

AABB2 SpriteSheet::GetTexCoordsForSpriteIndex(int spriteIndex) const
{
	TODO("Fix this so it works with UVs being bottom left so we dont have to use the moved font :l");
	int indexX = spriteIndex % m_spriteLayout.x;
	int indexY = (int)(spriteIndex / m_spriteLayout.x);

	return GetTexCoordsForSpriteCoords(IntVector2(indexX,indexY));
}

int SpriteSheet::GetNumSprites() const
{

	int amount = (m_spriteLayout.x * m_spriteLayout.y);
	return amount;
}


