#pragma once


//====================================================================================
// Forward Declare
//====================================================================================
class CommandingOfficer;

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
class Controller
{
public:
	Controller() {}

	virtual void Update() = 0;

public:
	CommandingOfficer*			m_pawn;
};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [7/14/2018]
//====================================================================================