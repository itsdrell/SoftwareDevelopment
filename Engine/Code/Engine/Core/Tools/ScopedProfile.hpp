#pragma once
#include "Engine/Core/General/EngineCommon.hpp"


//=============================================================
// Forward Declare
//=============================================================


//=============================================================
// Type Defs + Defines
//=============================================================


//=============================================================
// ENUMS
//=============================================================


//=============================================================
// Structs
//=============================================================

//====================================================================================
// How to use: Put something like this at the start of function 
//	ScopedProfile LoadLength = ScopedProfile();

//=============================================================
// Classes
//=============================================================
class ScopedProfile
{
public:
	ScopedProfile(std::string name);
	~ScopedProfile();

public:
	std::string		m_name;
	uint64_t		m_startTime;
};

//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
