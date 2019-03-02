#pragma once


//====================================================================================
// Forward Declare
//====================================================================================
class World;

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
class HUD
{
public:
	HUD(); 

public:
	void Render() const;

public:
	World*		m_world; // till we get a player
};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [2/28/2019]
//====================================================================================