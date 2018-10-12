#pragma once
#include "NetAddress.hpp"
#include "..\Core\Tools\Stopwatch.hpp"


//====================================================================================
// Forward Declare
//====================================================================================
class NetMessage;
class NetSession;

//====================================================================================
// Type Defs + Defines
//====================================================================================
#define INVALID_CONNECTION_INDEX 0xff

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

	// clean up
	void ClearOutgoingMessages();

	void Send(NetMessage& messageToSend);

	void CheckHeartbeatTimer();
	void SetHeartbeatTimer( float hz );
	void SetFlushRate( float hz );
	void CompareFlushRatesAndSet();

public:
	// list of all unreliables sent to this connection
	// that will be packed into a connection;
	std::vector<NetMessage*>	m_outboundUnreliables; 

	NetAddress					m_address; // address associtaed with this connectin; 


	// Timers
	Timer*						m_heartbeatTimer;
	Timer*						m_flushRateTimer;
	float						m_flushRateInHz = 0.f; // default value

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