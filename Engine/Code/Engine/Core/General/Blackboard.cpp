#include "Engine/Core/General/Blackboard.hpp"
#include "ThirdParty/tinyxml/tinyxml2.h"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/General/Rgba.hpp"

Blackboard::Blackboard()
{
}

void Blackboard::PopulateFromXmlElementAttributes( const tinyxml2::XMLElement& element )
{
	const tinyxml2::XMLAttribute* attribute = element.FirstAttribute();
	while( attribute )
	{
		std::string name = attribute->Name();
		std::string value = attribute->Value();
		m_keyValuePairs[ name ] = value;
		attribute = attribute->Next();
	}
}

void Blackboard::SetValue(const std::string & keyName, const std::string& newValue)
{
	// The issue with insert is that it's like vector pushback but it will only
	// add whenever the map doesnt already have a key for it
	m_keyValuePairs.insert(std::pair<std::string,std::string>(keyName,newValue));
}

bool Blackboard::GetValue(const std::string & keyName, bool defaultValue) const
{
	std::string value = m_keyValuePairs.find(keyName)->second;
	
	if(value == "true")
	{
		return true;
	}
	else if (value == "false")
	{
		return false;
	}
	else
	{
		return defaultValue;
	}

}

int Blackboard::GetValue(const std::string & keyName, int defaultValue) const
{
	std::map< std::string, std::string >::const_iterator found = m_keyValuePairs.find( keyName );
	if( found == m_keyValuePairs.end() )
	{
		return defaultValue;
	}
	else
	{
		return (int)atoi( found->second.c_str() );
	}
}

float Blackboard::GetValue( const std::string& keyName, float defaultValue ) const
{
	std::map< std::string, std::string >::const_iterator found = m_keyValuePairs.find( keyName );
	if( found == m_keyValuePairs.end() )
	{
		return defaultValue;
	}
	else
	{
		return (float) atof( found->second.c_str() );
	}
}

std::string Blackboard::GetValue(const std::string & keyName, std::string defaultValue) const
{
	std::map< std::string, std::string >::const_iterator found = m_keyValuePairs.find( keyName );
	if( found == m_keyValuePairs.end() )
	{
		return defaultValue;
	}
	else
	{
		return found->second;
	}
}

std::string Blackboard::GetValue( const std::string & keyName, const char* defaultValue ) const
{
	std::map< std::string, std::string >::const_iterator found = m_keyValuePairs.find( keyName );
	if( found == m_keyValuePairs.end() )
	{
		return std::string(defaultValue); // WE JUST CHANGE THIS
	}
	else
	{
		return found->second;
	}
}

Rgba Blackboard::GetValue(const std::string & keyName, const Rgba & defaultValue) const
{
	std::map< std::string, std::string >::const_iterator found = m_keyValuePairs.find( keyName );
	if( found == m_keyValuePairs.end() )
	{
		return defaultValue;
	}
	else
	{
		Rgba value;
		value.SetFromText(found->second.c_str());
		return value;
	}
}

Vector2 Blackboard::GetValue(const std::string & keyName, const Vector2 & defaultValue) const
{
	std::map< std::string, std::string >::const_iterator found = m_keyValuePairs.find( keyName );
	if( found == m_keyValuePairs.end() )
	{
		return defaultValue;
	}
	else
	{
		Vector2 value;
		value.SetFromText(found->second.c_str());
		return value;
	}
}

IntVector2 Blackboard::GetValue(const std::string & keyName, const IntVector2 & defaultValue) const
{
	std::map< std::string, std::string >::const_iterator found = m_keyValuePairs.find( keyName );
	if( found == m_keyValuePairs.end() )
	{
		return defaultValue;
	}
	else
	{
		IntVector2 value;
		value.SetFromText(found->second.c_str());
		return value;
	}
}

FloatRange Blackboard::GetValue(const std::string & keyName, const FloatRange & defaultValue) const
{
	std::map< std::string, std::string >::const_iterator found = m_keyValuePairs.find( keyName );
	if( found == m_keyValuePairs.end() )
	{
		return defaultValue;
	}
	else
	{
		FloatRange value;
		value.SetFromText(found->second.c_str());
		return value;
	}
}

IntRange Blackboard::GetValue(const std::string & keyName, const IntRange & defaultValue) const
{
	std::map< std::string, std::string >::const_iterator found = m_keyValuePairs.find( keyName );
	if( found == m_keyValuePairs.end() )
	{
		return defaultValue;
	}
	else
	{
		IntRange value;
		value.SetFromText(found->second.c_str());
		return value;
	}
}
