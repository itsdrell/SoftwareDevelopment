#include "Engine/Core/Utils/StringUtils.hpp"
#include "Engine/Core/Tools/ErrorWarningAssert.hpp"
#include <stdarg.h>
#include <vector>
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include <regex>
#include <string>


//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

std::vector<std::string> BreakSentenceIntoWords(std::string text)
{
	std::vector<std::string> vectorOfWords;
	
	std::string word;

	const char* textIndex = text.c_str();

	for(int i = 0; i < (int)text.length(); i++)
	{
		if(isspace(textIndex[i])) // stops every white space
		{
			vectorOfWords.push_back(word); // adds the built word to the vector
			word.clear(); // reset the word we are building
		}
		else
		{
			word.append(1,textIndex[i]); // build a word char by char
		}
	}

	// Add the last word!
	vectorOfWords.push_back(word);

	return vectorOfWords;
}

Strings SplitString(const std::string text, const char* seperator)
{
	int found = (int)text.find(seperator);
	int index = 0;
	Strings returnStrings;

	while(found!=(int)std::string::npos)
	{
		std::string currentWord = std::string(text,index,found - index);
		returnStrings.push_back(currentWord);

		index = found + 1;
		found = (int)text.find(seperator,index);
	}

	// now add the remaining part
	std::string lastWord = std::string(text,index,text.length() - index);
	returnStrings.push_back(lastWord);

	return returnStrings;
}


std::string RemoveCharacterFromString(std::string stringToChange, const char* thingToRemove)
{
	int found = (int)stringToChange.find(thingToRemove);

	while(found != (int)std::string::npos)
	{
		stringToChange.erase(stringToChange.begin() + found);

		found = (int)stringToChange.find(thingToRemove);
	}

	return stringToChange;
}

uint CountHowManyLinesAreInAString(std::string stringToCount)
{
	uint total = 0;

	size_t currentLocation = stringToCount.find("\n");

	while(currentLocation != std::string::npos)
	{
		total++;
		currentLocation = stringToCount.find("\n", currentLocation + 1);
	}

	return total;
}

int ParseString(std::string text, int defaultValue)
{
	int result;

	try
	{
		result = std::stoi(text);
	}
	catch (std::invalid_argument&)
	{
		result = defaultValue;
	}

	return result;
}
	

float ParseString(std::string text, float defaultValue)
{
	float result;

	try
	{
		result = std::stof(text);
	}
	catch (std::invalid_argument&)
	{
		result = defaultValue;
	}

	return result;
}

Vector2 ParseString(std::string text, Vector2 defaultValue)
{
	Vector2 result;

	//////////////////////////////////////////////////////////////////////////
	// String formatting stuff
	text = RemoveCharacterFromString(text, "(");
	text = RemoveCharacterFromString(text, ")");
	Strings s = SplitString(text, ",");

	//////////////////////////////////////////////////////////////////////////
	// Catch exceptions
	if(s.size() < 2)
		return defaultValue;

	//////////////////////////////////////////////////////////////////////////
	// Create result
	result.x = ParseString(s.at(0), defaultValue.x);
	result.y = ParseString(s.at(1), defaultValue.y);

	return result;
}

Rgba ParseString(std::string text, Rgba defaultValue)
{
	
	//////////////////////////////////////////////////////////////////////////
	// String formatting stuff
	text = RemoveCharacterFromString(text, "(");
	text = RemoveCharacterFromString(text, ")");
	Strings s = SplitString(text, ",");

	//////////////////////////////////////////////////////////////////////////
	// Catch exceptions
	if(s.size() < 3)
		return defaultValue;

	//////////////////////////////////////////////////////////////////////////
	// Create result
	float r = ParseString(s.at(0), (float) defaultValue.r);
	float g = ParseString(s.at(1), (float) defaultValue.g);
	float b = ParseString(s.at(2), (float) defaultValue.b);

	// check for alpha
	float a;
	if(s.size() > 3)
	{
		a = ParseString(s.at(3), (float) defaultValue.a);
	}
	else
	{
		a = (float) defaultValue.a;
	}

	Rgba result;
	result.SetFromFloats(r,g,b,a);
	return result;
}

uint GetLargestLengthInStrings(Strings listToCheck)
{
	uint result = 0;

	for(int i = 0; i < listToCheck.size(); i++)
	{
		if(result < listToCheck.at(i).length())
		{
			result = (uint) listToCheck.at(i).length();
		}
	}

	//DebugRenderLog(0.f, std::to_string(result));

	return result;
}

std::string GetLargestStringInStrings(Strings listToCheck)
{
	std::string result = "";

	for(int i = 0; i < listToCheck.size(); i++)
	{
		if(result.length() < listToCheck.at(i).length())
		{
			result = listToCheck.at(i);
		}
	}

	return result;
}

