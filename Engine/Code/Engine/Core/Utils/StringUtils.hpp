#pragma once
#include <string>
#include <vector>
#include "Engine/Core/General/EngineCommon.hpp"
#include <stdint.h>

//===============================================================================================
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );

std::string Stringv(const char* format, va_list args);

//std::vector<std::string> GetTokensFromString(const char * stringToReadFrom, const char * seperator, int howManytokenYouWant);

// Breaks a string by white space
Strings BreakSentenceIntoWords(std::string text);

// Breaks a string into seperate strings based off the seperator
Strings SplitString(const std::string text, const char* seperator);

// Removes a character from a string
std::string RemoveCharacterFromString(std::string stringToChange, const char* thingToRemove);

// Counts the /n is a string
uint CountHowManyLinesAreInAString(std::string stringToCount);

// Remove white space lines from Strings
void RemoveWhiteSpaceLines( Strings& theStrings );

uint GetLargestLengthInStrings(Strings listToCheck);
std::string GetLargestStringInStrings(Strings listToCheck);

bool StringIsNullOrEmpty(const char* text);

//===============================================================================================
// Change string to value, catches errors and returns a default value
int ParseString( std::string text, int defaultValue );
bool ParseString( std::string text, bool defaultValue);
float ParseString( std::string text, float defaultValue );
Rgba ParseString( std::string text, Rgba defaultValue );
Vector2 ParseString( std::string text, Vector2 defaultValue);
IntVector2 ParseString( std::string& text, const IntVector2& defaultValue);

//===============================================================================================
// Templates
//===============================================================================================

//String GetBytesAsString(uint16_t data);

template<typename T>
String GetBytesAsString(T data)
{
	// size of data is how many times we wanna shift
	uint sizeOfData = sizeof(data) * 4;

	String value;
	T index = data;

	for(uint i = 0; i < sizeOfData; i++)
	{
		if((index & 0b0000'0001) == 1)
			value = "1" + value;
		else
			value = "0" + value;

		index >>= 1;
	}

	return value;
	
}

