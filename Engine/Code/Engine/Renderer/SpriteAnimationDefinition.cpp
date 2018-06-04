#include "Engine/Renderer/SpriteAnimationDefinition.hpp"
#include "Engine/Core/Utils/XmlUtilities.hpp"
#include "Engine/Math/Vectors/IntVector2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/MathUtils.hpp"


// SpriteAnimationDefinition::SpriteAnimationDefinition(std::string nameOfAnimationSet, tinyxml2::XMLElement& definition)
// {
// 	m_name = nameOfAnimationSet;
// 
// 	// Get Variables from the <SpriteAnimSet>
// 	m_filePath = ParseXmlAttribute(definition,"spriteSheet","Error");
// 	IntVector2 frames = ParseXmlAttribute(definition, "spriteLayout",IntVector2(0,0));
// 
// 
// 	// Create the map of animations based on the next nodes
// 	tinyxml2::XMLElement* indexElement = definition.FirstChildElement(); // said it wasnt a pointer?
// 
// 	while(indexElement)
// 	{
// 		// Get attribute values
// 		std::string keyName = ParseXmlAttribute(*indexElement, "name","Error");
// 		m_frames = ParseXmlAttribute(*indexElement,"spriteIndexes",m_frames);
// 		m_durationSeconds = ParseXmlAttribute(*indexElement, "duration",0.f);
// 
// 		// Create the animation
// 		SpriteAnimation* newSpriteAnim = new SpriteAnimation(this);
// 
// 		// Add to map
// 		m_animations.insert(std::pair<std::string,SpriteAnimation*>(keyName,newSpriteAnim));
// 		
// 		// Delete new Anim pointer
// 		newSpriteAnim = nullptr;
// 		delete[] newSpriteAnim;
// 
// 		// Next index
// 		indexElement = indexElement->NextSiblingElement();
// 	}
// 
// 	// Add definition to static map
// 	s_definitions.insert(std::pair<std::string,SpriteAnimationDefinition*>(m_name,this));
// 
// }

SpriteAnimationDefinition::SpriteAnimationDefinition(const tinyxml2::XMLElement& animElement, SpriteSheet& defaultSpriteSheet, float defaultFps)
{
	m_spriteSheet = &defaultSpriteSheet;
	m_framesPerSecond = ParseXmlAttribute(animElement, "fps", defaultFps);
	m_name = ParseXmlAttribute(animElement, "name", "ERROR NO NAME");
	m_spriteIndexes = ParseXmlAttribute(animElement,"spriteIndexes", Ints());
}

SpriteAnimationDefinition::~SpriteAnimationDefinition()
{
}

int SpriteAnimationDefinition::GetSpriteIndexAtTime(float elapsedSeconds) const
{
	float length = elapsedSeconds * m_framesPerSecond;

	int currentFrame = (int)length % (int)m_spriteIndexes.size();
	
	int frame = ClampInt(currentFrame,0,(int)m_spriteIndexes.size());

	return m_spriteIndexes.at(frame);
}
