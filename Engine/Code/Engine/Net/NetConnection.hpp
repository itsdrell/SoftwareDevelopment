#pragma once
#include "NetAddress.hpp"


//====================================================================================
// Forward Declare
//====================================================================================
class NetMessage;
class NetSession;

//====================================================================================
// Type Defs + Defines
//====================================================================================
#define INVALID_CONNECTION_INDEX 0

//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class NetConnection
{
	// the connection's index (luid - Local Unique IDentifier)
	// meaning once a connection is in the sessions array - do not move it

public:

	NetConnection() {}
	NetConnection(uint8_t idx, const NetAddress& theAddress, NetSession* owningSession);

	~NetConnection();

	// If we have unreliables, send as many packets as we need
	// to send all of them (conditions for sending a packet may change later, 
	// so may want to split the internals to "should_send_packet() and "send_packet"
	// aka flush
	void ProcessOutgoing(); 

	void Send(NetMessage& messageToSend);

public:
	// list of all unreliables sent to this connection
	// that will be packed into a connection;
	std::vector<NetMessage*>	m_outboundUnreliables; 

	NetAddress					m_address; // address associtaed with this connectin; 

	// how this relates to the session; 
	NetSession*					m_owningSession; 
	uint8_t						m_indexInSession;
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