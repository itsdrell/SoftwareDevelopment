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


typedef uint eNetMessageOptions; 
//====================================================================================
// ENUMS
//====================================================================================
enum NetMessageChannelIndexName
{
	MESSAGE_CHANNEL_DEFAULT = 0,
	CORE,
	NUMBER_OF_NETMESSAGE_CHANNELS
};

//====================================================================================
// Structs
//====================================================================================
// this is what we send. Making it a struct now because we might add things to it!
struct NetMessageHeader
{
	NetMessageHeader() {}
	
	NetMessageHeader(uint8_t callbackDefinitionIndex)
		: m_messageCallbackDefinitionIndex(callbackDefinitionIndex) {}

	NetMessageHeader(uint8_t callbackDefinitionIndex, uint16_t reliableID)
		: m_messageCallbackDefinitionIndex(callbackDefinitionIndex)
		, m_reliableID(reliableID) {}
	
	uint8_t		m_messageCallbackDefinitionIndex = 0U;
	uint16_t	m_reliableID = 0U;
	uint16_t	m_sequenceID = 0U;
};

// this is for server side!
struct NetMessageDefinition
{
	NetMessageDefinition(int ID, const String& callbackName, NetMessage_cb callback, eNetMessageOptions option, NetMessageChannelIndexName channelIndex = MESSAGE_CHANNEL_DEFAULT )
		: m_callbackID(ID)
		, m_callbackName(callbackName)
		, m_callback(callback)
		, m_option(option)
		, m_channelIndex(channelIndex)	{}

	int										m_callbackID;
	String									m_callbackName;
	NetMessage_cb							m_callback;
	eNetMessageOptions						m_option;
	NetMessageChannelIndexName				m_channelIndex;
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
	NetMessage( const NetMessage& copyFrom );

	bool RequiresConnection( const NetSession& theSession );

	uint GetHeaderSize() const;
	bool IsReliable() const;
	bool IsReliableInOrder() const;
	void ResetAge();


public:
	String								m_definitionName;
	NetMessageDefinition*				m_definition = nullptr; // this is set in NetConnection::Send();

	NetMessageHeader					m_header;

	uint16_t							m_reliable_id = 0;
	uint								m_lastSentTimeMS = 0U;


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