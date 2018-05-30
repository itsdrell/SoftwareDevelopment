#include "Engine/Renderer/SpriteAnimSetDefinition.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Renderer/SpriteAnimationDefinition.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include <string>
#include <map>


SpriteAnimSetDefinition::SpriteAnimSetDefinition(tinyxml2::XMLElement& animSetElement, Renderer& renderer)
{
	
	// Get the spritesheet path FROM THE PARENT just once
	std::string path = ParseXmlAttribute(animSetElement, "spriteSheet", "Could not find");
	IntVector2 spriteDim = ParseXmlAttribute(animSetElement, "spriteLayout", IntVector2(1,1));

	// create index
	tinyxml2::XMLElement* indexElement = animSetElement.FirstChildElement(); 

	// We are looping to create Sprite Animation Definitions and storing them in our map
	while(indexElement)
	{
		
		// Get the name / key
		std::string keyName = ParseXmlAttribute(*indexElement, "name","Error");
		
		

		// Get the dimensions
		//IntVector2 spriteDimensions = ParseXmlAttribute(*indexElement, "spriteLayout", IntVector2(1,12));

		// Get the fps
		float fps = ParseXmlAttribute(*indexElement, "fps", 1.0f);

		// Make/Get the spritesheet
		Texture* spriteSheetTexture = renderer.CreateOrGetTexture(path);
		SpriteSheet* theSpriteSheet = new SpriteSheet(spriteSheetTexture,spriteDim.x, spriteDim.y);

		// Create the animation
		SpriteAnimationDefinition* newSpriteAnim = new SpriteAnimationDefinition(*indexElement, *theSpriteSheet,fps);

		// Add to map
		m_namedAnimDefs.insert(std::pair<std::string,SpriteAnimationDefinition*>(keyName,newSpriteAnim));
		
		// Delete pointers, could do this after the loop but im being safe
		newSpriteAnim = nullptr;
		delete[] newSpriteAnim;

		spriteSheetTexture = nullptr;
		delete[] spriteSheetTexture;

		theSpriteSheet = nullptr;
		delete[] theSpriteSheet;

		// Next index
		indexElement = indexElement->NextSiblingElement();
	}
}

SpriteAnimSetDefinition::~SpriteAnimSetDefinition()
{

}

