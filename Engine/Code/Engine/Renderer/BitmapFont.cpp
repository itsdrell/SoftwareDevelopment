#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include <string>

AABB2 BitmapFont::GetUVsForGlyph(int glyphUnicode) const
{
	return m_spriteSheet->GetTexCoordsForSpriteIndex(glyphUnicode);
}

float BitmapFont::GetStringWidth(const std::string & asciiText, float cellHeight, float aspectScale)
{
	return cellHeight * aspectScale * m_baseAspect * asciiText.length();
}

float BitmapFont::GetWidthOfAGlyph(float cellHeight, float aspectScale)
{
	return cellHeight * aspectScale * m_baseAspect;
}

BitmapFont::BitmapFont(const std::string& fontName, const SpriteSheet& glyphSheet, float baseAspect)
{
	m_spriteSheet = &glyphSheet;
	m_baseAspect = baseAspect;
	m_name = fontName;
}


