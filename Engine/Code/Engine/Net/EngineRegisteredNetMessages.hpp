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
bool OnHeartbeat( NetMessage& msg, const NetSender& from );
bool OnPing( NetMessage& msg, const NetSender& from );
bool OnPong( NetMessage& msg, const NetSender& from );

bool OnJoinRequest( NetMessage& msg, const NetSender& from  );
bool OnJoinDeny( NetMessage& msg, const NetSender& from );
bool OnJoinAccept( NetMessage& msg, const NetSender& from );
bool OnNewConnection( NetMessage& msg, const NetSender& from );
bool OnJoinFinished( NetMessage& msg, const NetSender& from );
bool OnUpdateConnState( NetMessage& msg, const NetSender& from );

bool OnHangup( NetMessage& msg, const NetSender& from );


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [10/11/2018]
//====================================================================================