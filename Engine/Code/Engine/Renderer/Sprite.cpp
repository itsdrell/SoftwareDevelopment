#include "Sprite.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/Utils/XmlUtilities.hpp"
#include "Engine/Math/Geometry/AABB2.hpp"

//////////////////////////////////////////////////////////////////////////
std::map<std::string, Sprite*> Sprite::s_sprites;

//////////////////////////////////////////////////////////////////////////
Sprite::Sprite(Texture& theTextureToUse, Vector2 pivot, AABB2 theUVs)
{
	m_image = &theTextureToUse;
	m_dimensions = theTextureToUse.GetDimensions().GetAsVector2();
	m_pivot = pivot;
	m_uv = theUVs;

}

Sprite::Sprite(std::string pathOfTexture, Vector2 pivot, AABB2 theUVs /*= AABB2(0.f,0.f,1.f,1.f)*/)
{
	m_image = Renderer::GetInstance()->CreateOrGetTexture(pathOfTexture);

	m_dimensions = m_image->GetDimensions().GetAsVector2();
	m_pivot = pivot;
	m_uv = theUVs;
}

Sprite::Sprite(tinyxml2::XMLElement& definition)
{
	m_name = ParseXmlAttribute(definition, "id", "ERROR");

	tinyxml2::XMLElement* indexElement = definition.FirstChildElement();

	while(indexElement)
	{
		std::string nameOfCurrentElemenet = indexElement->Name();

		if(nameOfCurrentElemenet == "diffuse")
		{
			std::string path = ParseXmlAttribute(*indexElement,"src", "ERROR");
			
			m_image = Renderer::GetInstance()->CreateOrGetTexture(path,false);

		}

		if(nameOfCurrentElemenet == "ppu")
		{
			m_pixelsPerUnit = ParseXmlAttribute(*indexElement, "count", 1.f);
		}

		if(nameOfCurrentElemenet == "uv")
		{
			m_uv = ParseXmlAttribute(*indexElement, "uvs", AABB2(0.f,0.f,1.f,1.f));
		}

		if(nameOfCurrentElemenet == "pivot")
		{
			m_pivot = ParseXmlAttribute(*indexElement, "xy", Vector2(.5f,.5f));	
		}


		indexElement = indexElement->NextSiblingElement();
	}


	// once we are done gonna calculate the dimensions.

	// Gonna do the ppu conversion here instead of everytime we draw
	float xdim = (m_uv.maxs.x - m_uv.mins.x) / m_pixelsPerUnit;
	float ydim = (m_uv.maxs.y - m_uv.mins.y) / m_pixelsPerUnit;

	m_dimensions = Vector2(xdim,ydim);

	// Calculating the uvs based off image size
	float width =  (float) m_image->GetDimensions().x;
	float height = (float) m_image->GetDimensions().y;

	AABB2 newUvs = AABB2(m_uv.mins.x / width, m_uv.mins.y / height, m_uv.maxs.x / width, m_uv.maxs.y / height);



	m_uv = newUvs;

	// Add it to the list
	s_sprites.insert(std::pair<std::string,Sprite*>(m_name,this));
}

Sprite* Sprite::GetSprite(std::string name)
{
	std::map<std::string,Sprite*>::iterator spriteIterator;
	spriteIterator = Sprite::s_sprites.find(name);
	if(spriteIterator != Sprite::s_sprites.end()){return spriteIterator->second;}

	ERROR_RECOVERABLE("Could not find Sprite: " + name);

	return nullptr;
}

