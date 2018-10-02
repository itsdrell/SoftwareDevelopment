#pragma once
#include "..\Core\General\BytePacker.hpp"


//====================================================================================
// Forward Declare
//====================================================================================
class NetSession;
class NetMessage;
struct NetSender;

//====================================================================================
// Type Defs + Defines
//====================================================================================
typedef bool (*NetMessage_cb) ( NetMessage& theMessage, const NetSender& theSender );


//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================
// this is what we send. Making it a struct now because we might add things to it!
struct NetMessageHeader
{
	NetMessageHeader() {}
	
	NetMessageHeader(uint8_t callbackDefinitionIndex)
		: m_messageCallbackDefinitionIndex(callbackDefinitionIndex) {}
	
	uint8_t		m_messageCallbackDefinitionIndex = 0U;
};

// this is for server side!
struct NetMessageDefinition
{
	NetMessageDefinition(int ID, const String& callbackName, NetMessage_cb callback)
		: m_callbackID(ID)
		, m_callbackName(callbackName)
		, m_callback(callback) {}

	int					m_callbackID;
	String				m_callbackName;
	NetMessage_cb		m_callback;
};


//====================================================================================
// Classes
//====================================================================================
//-----------------------------------------------------------------------------------------------
// A message is a specific piece of information
// we're sending to someone else that they should know how to proces;
//
// This of it as the individual items within a shipment.  
// USE LITTLE_ENDIAN
class NetMessage : public BytePacker
{
public:
	NetMessage(); // for receiving
	NetMessage( const char* name );  // for sending

	//===============================================================================================
	// ONLY PAYLOAD
	//-----------------------------------------------------------------------------------------------

public:
	// this is an optimization forseth said we don;t need atm
	//byte_t m_local_buffer[MESSAGE_MTU]; // MESSAGE_MTU I use 1KB for, arbitrarily

	// Ways to identity the definition
	// when construction, you'll be passed a name.  On send
	// you can look up the actual definition by name.
	//
	// On receive, you get an index, and can ask the 
	// session for hte definition for that index; 
	String								m_definitionName;

	//NetMessageDefinition*				m_theDefinition = nullptr;
	NetMessageHeader					m_header;
//    uint8_t							m_definitionIndex; 

}; 

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [9/22/2018]
//====================================================================================