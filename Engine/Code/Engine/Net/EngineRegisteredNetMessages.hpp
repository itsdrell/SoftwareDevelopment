#pragma once
#include "NetMessage.hpp"
#include "Engine/Net/NetSession.hpp"

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
void RegisterEngineNetMessages( NetSession& theSession );


//-----------------------------------------------------------------------------------------------
// Messages
bool OnHeartbeat( NetMessage& msg, const NetSender& from);

//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [10/11/2018]
//====================================================================================