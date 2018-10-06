#pragma once
#include "NetAddress.hpp"
#include "PacketChannel.hpp"
#include <map>
#include "NetMessage.hpp"

//====================================================================================
// Forward Declare
//====================================================================================
class NetConnection;

//====================================================================================
// Type Defs + Defines
//====================================================================================

//====================================================================================
// ENUMS
//====================================================================================
#define NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS 16
#define GAME_PORT ("10084")

//====================================================================================
// Structs
//====================================================================================
struct NetSender
{
	NetSender() {}
	
	// just a connection for now but later will change
	NetSender(NetConnection& theConnection)
		: m_connection(&theConnection) {}
	
	NetConnection*	m_connection = nullptr;
};

//====================================================================================
// Classes
//====================================================================================
class NetSession 
{
	// contains a list of connections; 
public:
	NetSession();
	
	~NetSession();
	
public:

	static NetSession* GetInstance() { return s_mainNetSession; }

	// message definitions
	bool RegisterMessageDefinition( const String& id, NetMessage_cb cb );
	bool IsMessageAlreadyRegistered(const String& id);
	
	NetMessageDefinition* GetMessageDefinition( int const &id );
	NetMessageDefinition* GetMessageDefinitionByIndex( uint8_t const idx ) ; 
	NetMessageDefinition* GetMessageDefinitionByName( const String& name );

	void SortDefinitions();

	// Starting a session (finalizes definitions - can't add more once
	// the session is running)
	void Bind( const char* port, uint range_to_try = 0U );

	// Connection management
	NetConnection* AddConnection( uint idx, NetAddress const &addr );  
	void CloseAllConnections(); 

	// updates
	void ProcessIncoming();
	void ProcessPacket( NetPacket& packet , const NetAddress& sender);

	void ProcessOutgoing();
	void SendPacket( const NetPacket& packet );

	void Render() const;

	uint8_t GetMyConnection() const;
	uint8_t GetConnectionForPacket(const PacketHeader& theHeader, const NetAddress& theAddressOfSender);
	NetConnection* GetConnection(int idx) const;
	NetConnection* GetConnectionFromAddress(const NetAddress& sender) const;


public:
	NetConnection*								m_connections[NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS]; // all connections I know about; 
	PacketChannel								m_channel; // what we send/receive packets on;

	// gonna keep the list here, instead of in NetMessageDefinition so that
	// multiple sessions can have different callbacks!
	std::vector<NetMessageDefinition*>			m_messageCallbacks;


	static NetSession* s_mainNetSession;
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