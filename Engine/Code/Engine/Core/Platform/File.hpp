#pragma once
// Loads a file to a memory buffer
// Free using free(ptr); 
#include <stdio.h>
#include "Engine/Core/General/EngineCommon.hpp"



//====================================================================================
// Forward Declare
//====================================================================================


//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================


//====================================================================================
// Standalone C Functions
//====================================================================================
void* FileReadToNewBuffer( char const *filename );
void LogStringToFile(const char* filename, const char* text, bool overwrite = false);
void LogStringsToFile(char const* filename, const Strings& theStrings, bool overwrite = false );
Strings GetAllLinesFromFile( char const* filename );
uint GetLineLengthOfAFile( char const* filename );

//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [8/27/2018]
//====================================================================================

