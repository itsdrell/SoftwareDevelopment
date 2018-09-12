#include "Engine/Renderer/Images/Sprites/SpriteSheet.hpp"
#include "Engine/Math/Vectors/IntVector2.hpp"
#include "Engine/Core/General/EngineCommon.hpp"
#include "Engine/Core/Utils/XmlUtilities.hpp"

//===============================================================================================
std::map<std::string, SpriteSheet*> SpriteSheet::s_spriteSheets;
//===============================================================================================
SpriteSheet::SpriteSheet()
{
}

SpriteSheet::SpriteSheet(Texture* texture, int tilesWide, int tilesHigh)
{
	m_spriteSheetTexture = texture;
	m_spriteLayout = IntVector2(tilesWide,tilesHigh);
	m_dimension = m_spriteSheetTexture->GetDimensions();
}

//-----------------------------------------------------------------------------------------------
SpriteSheet::SpriteSheet(tinyxml2::XMLElement& node)
{

	m_name = ParseXmlAttribute(node, "name", "ERROR");
	m_path = ParseXmlAttribute(node, "src", "ERROR");
	m_spriteLayout = ParseXmlAttribute(node, "layout", m_spriteLayout);
	m_pixelsPerUnit = ParseXmlAttribute(node, "ppu", 16.f);

	std::string fullpath = "Data/Images/" + m_path + ".png";

	// Create or get texture
	m_spriteSheetTexture = Renderer::GetInstance()->CreateOrGetTexture(fullpath);
	m_dimension = m_spriteSheetTexture->GetDimensions();
	
	s_spriteSheets[m_name] = this;
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

	// What this does is fix the weird texture flip!
	return AABB2(Vector2(minx, 1.f - maxy),Vector2(maxx,1.f - miny));
}

AABB2 SpriteSheet::GetTexCoordsForSpriteIndex(int spriteIndex) const
{
	int indexX = spriteIndex % m_spriteLayout.x;
	int indexY = (int)(spriteIndex / m_spriteLayout.x);

	return GetTexCoordsForSpriteCoords(IntVector2(indexX,indexY));
}

//-----------------------------------------------------------------------------------------------
int SpriteSheet::GetNumSprites() const
{

	int amount = (m_spriteLayout.x * m_spriteLayout.y);
	return amount;
}

//-----------------------------------------------------------------------------------------------
IntVector2 SpriteSheet::GetIndividualSpriteSize() const
{
	// This is so you can tell the sprite how big it is going to be

	float x =((float)m_dimension.x / (float)m_spriteLayout.x);
	float y =((float)m_dimension.y / (float)m_spriteLayout.y);

	return Vector2(x,y).GetVector2AsInt();
}

//-----------------------------------------------------------------------------------------------
SpriteSheet* SpriteSheet::CreateOrGet(const std::string& filePath, IntVector2 dimensions)
{
	std::map<std::string, SpriteSheet*>::iterator it;

	// see if the bitmap already exists in our map
	it = s_spriteSheets.find(filePath);

	// if it does, return the value
	if(it != s_spriteSheets.end())
	{
		return s_spriteSheets.at(filePath);
	}
	else // make a new one
	{
		std::string fullpath = "Data/Images/" + filePath + ".png";

		// Create or get texture
		Texture* fontTexture = Renderer::GetInstance()->CreateOrGetTexture(fullpath, false);

		// Create a spritesheet from that texture
		SpriteSheet* newSheet = new SpriteSheet( fontTexture, dimensions.x , dimensions.y );

		s_spriteSheets[filePath] = newSheet;

		return newSheet;
	}
}

int SpriteSheet::GetIndexFromSpriteCoords(const IntVector2 & spriteCoords)
{
	// y * width + x
	int result = (spriteCoords.y * m_spriteLayout.x) + spriteCoords.x;
	return result;
}

IntVector2 SpriteSheet::GetCoordsFromSpriteIndex(int index)
{
	int indexX = index % m_spriteLayout.x;
	int indexY = (int)(index / m_spriteLayout.x);

	return IntVector2(indexX,indexY);
}

