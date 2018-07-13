#include "Engine/Core/Platform/File.hpp"
#include <stdlib.h>     /* malloc, free, rand */
#include <iostream>
#include <fstream>
#include "Engine/Core/Tools/ErrorWarningAssert.hpp"

void* FileReadToNewBuffer( char const *filename )
{
	// changed this from fopen to fopen_s cause compiler didnt think it was safe
	FILE *pf;
	
	// error codes https://msdn.microsoft.com/en-us/library/t3ayayh1.aspx
	/*errno_t check = */fopen_s( &pf , filename, "r" ); 
	
	if (pf == nullptr) {
		return nullptr; 
	}

	size_t size = 0U; 
	fseek(pf, 0L, SEEK_END);
	size = ftell(pf);

	fseek(pf, 0L, SEEK_SET); 

	// these chars were byte_t but im assuming he meant char
	char *buffer = (char*) malloc(size + 1); 

	size_t readBytes = fread( buffer, 1, size, pf );
	fclose(pf);

	buffer[readBytes] = 0;
	return buffer;  
}

void LogStringToFile(const char* filename, const char* text, bool overwrite)
{
	std::ofstream outputFile;

	// try to open file http://www.cplusplus.com/reference/fstream/fstream/open/
	if(overwrite == false)
		outputFile.open(filename, std::fstream::out | std::fstream::app);
	else
		outputFile.open(filename, std::fstream::out | std::fstream::trunc); // erase the file (for clean up)

	// See if we opened it
	if(outputFile.is_open() == false)
	{
		// this can be called if the file isn't checked out btw :l
		ERROR_RECOVERABLE("Couldn't log to: " + std::string(filename));
		return;
	}

	std::string log = std::string(text);

	outputFile << text;

	outputFile.close();
}

Strings GetAllLinesFromFile(char const* filename)
{
	Strings result;
	
	std::ifstream infile(filename);

	std::string line;
	while (std::getline(infile, line))
	{
		result.push_back(line);
	}

	return result;
}

uint GetLineLengthOfAFile(char const* filename)
{
	uint result = 0;
	
	std::ifstream infile(filename);

	std::string line;
	while (std::getline(infile, line))
	{
		result++;
	}

	return result;
}
