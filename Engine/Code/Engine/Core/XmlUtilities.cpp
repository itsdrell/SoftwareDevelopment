#include "XmlUtilities.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "ThirdParty/tinyxml/tinyxml2.h"
#include "Engine/Math/AABB2.hpp"


int ParseXmlAttribute(const tinyxml2::XMLElement & element, const char* attributeName, int defaultValue)
{
	const char* intText = element.Attribute( attributeName );
	if( intText == nullptr )
	{
		return defaultValue;
	}

	return atoi( intText );
}

char ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, char defaultValue)
{
	const char* charText = element.Attribute( attributeName );
	if( charText == nullptr )
	{
		return defaultValue;
	}

	return charText[0]; // it's already a char...?
}

bool ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, bool defaultValue)
{
	const char* boolText = element.Attribute( attributeName );

	if(boolText == nullptr){return defaultValue;}

	std::string boolString( boolText );
	if( boolString == "true" )
		return true;

	return false;
}

float ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, float defaultValue)
{
	const char* floatText = element.Attribute( attributeName );
	if( floatText == nullptr )
	{
		return defaultValue;
	}

	return (float) atof( floatText );
}

Rgba ParseXmlAttribute(const tinyxml2::XMLElement& element, const char * attributeName, const Rgba& defaultValue)
{
	const char* valueText = element.Attribute( attributeName );
	if( valueText == nullptr )
	{
		return defaultValue;
	}

	Rgba value;
	value.SetFromText( valueText );
	return value;
}

Vector2 ParseXmlAttribute(const tinyxml2::XMLElement& element, const char * attributeName, const Vector2& defaultValue)
{
	const char* valueText = element.Attribute( attributeName );
	if( valueText == nullptr )
	{
		return defaultValue;
	}

	Vector2 value;
	value.SetFromText( valueText );
	return value;
}

IntRange ParseXmlAttribute(const tinyxml2::XMLElement& element, const char * attributeName, const IntRange& defaultValue)
{
	const char* inText = element.Attribute( attributeName );
	if( inText == nullptr )
	{
		return defaultValue;
	}

	IntRange value;
	value.SetFromText(inText);
	return value;
}

FloatRange ParseXmlAttribute(const tinyxml2::XMLElement& element, const char * attributeName, const FloatRange& defaultValue)
{
	const char* inText = element.Attribute( attributeName );
	if( inText == nullptr )
	{
		return defaultValue;
	}

	FloatRange value;
	value.SetFromText(inText);
	return value;
}

IntVector2 ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, const IntVector2& defaultValue)
{
	const char* inText = element.Attribute( attributeName );
	if( inText == nullptr )
	{
		return defaultValue;
	}

	IntVector2 value;
	value.SetFromText(inText);
	return value;
}

std::string ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, const std::string& defaultValue)
{
	const char* attributeValue = element.Attribute( attributeName );

	if(attributeValue == nullptr){return defaultValue;}

	return std::string( attributeValue );
}

std::string ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, const char* defaultValue)
{
	const char* attributeValue = element.Attribute( attributeName );
	if( attributeValue == nullptr )
	{
		if( defaultValue == nullptr )
		{
			return std::string( "" );
		}
		else
		{
			return std::string( defaultValue );
		}
	}

	return std::string( attributeValue );
}

eCullMode ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, eCullMode defaultValue)
{
	const char* inText = element.Attribute( attributeName );

	if( inText == nullptr )
	{
		return defaultValue;
	}
	
	std::string value = std::string(inText);
	// figure out which one
	if(value == "back"){ return CULLMODE_BACK; }
	if(value == "front") { return CULLMODE_FRONT; }
	if(value == "none") { return CULLMODE_NONE; }

	return defaultValue;
}

eFillMode ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, eFillMode defaultValue)
{
	const char* inText = element.Attribute( attributeName );

	if( inText == nullptr )
	{
		return defaultValue;
	}
	
	std::string value = std::string(inText);

	if(value == "solid") { return FILLMODE_SOLID; }
	if(value == "wire") { return FILLMODE_WIRE; }

	return defaultValue;
}

eWindOrder ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, eWindOrder defaultValue)
{
	const char* inText = element.Attribute( attributeName );

	if( inText == nullptr )
	{
		return defaultValue;
	}

	std::string value = std::string(inText);
	
	if(value == "cw") { return WIND_CLOCKWISE; }
	if(value == "ccw") { return WIND_COUNTER_CLOCKWISE; }

	return defaultValue;
}

eBlendOperation ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, eBlendOperation defaultValue)
{
	const char* inText = element.Attribute( attributeName );

	if( inText == nullptr )
	{
		return defaultValue;
	}

	std::string value = std::string(inText);
	
	if(value == "add") { return COMPARE_ADD; }
	if(value == "sub") { return COMPARE_SUBTRACT; }
	if(value == "rev_sub") {return COMPARE_REVERSE_SUBTRACT; }
	if(value == "min") { return COMPARE_MIN; }
	if(value == "max") { return COMPARE_MAX; }

	return defaultValue;

}

eBlendFactor ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, eBlendFactor defaultValue)
{
	const char* inText = element.Attribute( attributeName );

	if( inText == nullptr )
	{
		return defaultValue;
	}

	std::string value = std::string(inText);
	
	if(value == "one") { return BLEND_ONE; }
	if(value == "zero") { return BLEND_ZERO; }
	if(value == "src_color") { return BLEND_SRC_COLOR; }
	if(value == "inv_src_color") { return BLEND_ONE_MINUS_SRC_COLOR; }
	if(value == "src_alpha") { return BLEND_SRC_ALPHA; }
	if(value == "inv_src_alpha") { return BLEND_ONE_MINUS_SRC_ALPHA; }
	if(value == "dest_color") { return BLEND_DST_COLOR; }
	if(value == "inv_dest_color") { return BLEND_ONE_MINUS_DST_COLOR; }
	if(value == "dest_alpha") { return BLEND_DST_ALPHA; }
	if(value == "inv_dest_alpha") { return BLEND_ONE_MINUS_DST_ALPHA; }
	if(value == "constant") { return BLEND_CONSTANT_COLOR; }
	if(value == "inv_constant") { return BLEND_ONE_MINUS_CONSTANT_COLOR; }
	if(value == "const_alpha") { return BLEND_CONSTANT_ALPHA; }
	if(value == "inv_const_alpha") { return BLEND_ONE_MINUS_CONSTANT_ALPHA; }
	if(value == "alpha_saturate") { return BLEND_SRC_ALPHA_SATURATE; }

	return defaultValue;

}

DepthBufferComparisons ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, DepthBufferComparisons defaultValue)
{
	const char* inText = element.Attribute( attributeName );

	if( inText == nullptr )
	{
		return defaultValue;
	}

	std::string value = std::string(inText);
	
	if(value == "less") { return COMPARE_LESS; }
	if(value == "never") { return COMPARE_NEVER; }
	if(value == "equal") { return COMPARE_EQUAL; }
	if(value == "lequal") { return COMPARE_LEQUAL; }
	if(value == "greater") { return COMPARE_GREATER; }
	if(value == "gequal") { return COMPARE_GEQUAL; }
	if(value == "not") { return COMPARE_NOT_EQUAL; }
	if(value == "always") { return COMPARE_ALWAYS; }

	return defaultValue;

}

Ints ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, const Ints defaultValue)
{
	Ints amountOfNumbers;
	std::vector<std::string> intsAsStrings;
	std::string word;

	const char* value = element.Attribute(attributeName);

	if(value == nullptr){return defaultValue;}

	int length = (int)std::string(value).length();

	// Create a vector without the comma
	for(int i = 0; i <= length; i++)
	{
		char currentCharacter = value[i];

		if(currentCharacter == 44) // should be ,
		{
			intsAsStrings.push_back(word);
			word.clear();
		}
		else
		{
			word.append(1,currentCharacter);
		}
	}

	// push back the last word
	intsAsStrings.push_back(word);

	// Now we go through that vector and turn the chars to ints and add to our return vector
	for(int j = 0; j < (int)intsAsStrings.size(); j++)
	{
		const char* currentInt = intsAsStrings.at(j).c_str();
		int current = atoi(currentInt); // turn to int
		amountOfNumbers.push_back(current);
	}

	return amountOfNumbers;
}

AABB2 ParseXmlAttribute(const tinyxml2::XMLElement& element, const char* attributeName, const AABB2 defaultValue)
{
	const char* inText = element.Attribute(attributeName);

	if(inText == nullptr)
	{
		return defaultValue;
	}

	AABB2 value;
	value.SetFromText(inText);
	return value;
}
