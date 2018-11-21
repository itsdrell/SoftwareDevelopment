#pragma once
#include "Engine/Net/EngineRegisteredNetMessages.hpp"

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
enum eNetGameMessage : uint8_t
{
	NETMSG_TEST_GAME_MESSAGE = NETMSG_CORE_COUNT, // allow us to resume count from where we left off in core
												  // ...

	// This is the test used for grading - it is fixed as 128 so that if you add
	// other messages, it won't interfer with our tests;
	NETMSG_UNRELIABLE_TEST = 128,
	NETMSG_RELIABLE_TEST = 129,
	NETMSG_SEQUENCE_TEST = 130,
};

//====================================================================================
// Standalone C Functions
//====================================================================================
void RegisterGameNetMessages( NetSession& theSession );


//-----------------------------------------------------------------------------------------------
// Messages
bool OnUnreliableTest( NetMessage& msg, const NetSender& from );
bool OnReliableTest( NetMessage& msg, const NetSender& from );
bool OnSequenceTest( NetMessage& msg, const NetSender& from  );

//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [10/25/2018]
//====================================================================================