#pragma once
#include "NetMessage.hpp"
#include "Engine/Net/NetSession.hpp"

//====================================================================================
// Forward Declare
//====================================================================================
class NetSession;

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
void RegisterCoreEngineNetMessages( NetSession& theSession );


//-----------------------------------------------------------------------------------------------
// Messages
bool OnHeartbeat( NetMessage& msg, const NetSender& from, NetSession* sessionToUse );
bool OnPing( NetMessage& msg, const NetSender& from, NetSession* sessionToUse );
bool OnPong( NetMessage& msg, const NetSender& from, NetSession* sessionToUse );

bool OnJoinRequest( NetMessage& msg, const NetSender& from, NetSession* sessionToUse  );
bool OnJoinDeny( NetMessage& msg, const NetSender& from, NetSession* sessionToUse );
bool OnJoinAccept( NetMessage& msg, const NetSender& from, NetSession* sessionToUse );
bool OnNewConnection( NetMessage& msg, const NetSender& from, NetSession* sessionToUse );
bool OnJoinFinished( NetMessage& msg, const NetSender& from, NetSession* sessionToUse );
bool OnUpdateConnState( NetMessage& msg, const NetSender& from, NetSession* sessionToUse );


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [10/11/2018]
//====================================================================================