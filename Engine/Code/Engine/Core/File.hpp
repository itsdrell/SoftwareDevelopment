#pragma once
// Loads a file to a memory buffer
// Free using free(ptr); 
#include <stdio.h>
#include "EngineCommon.hpp"

void* FileReadToNewBuffer( char const *filename );
void LogStringToFile(const char* filename, const char* text, bool overwrite = false);
Strings GetAllLinesFromFile( char const* filename );
uint GetLineLengthOfAFile( char const* filename );