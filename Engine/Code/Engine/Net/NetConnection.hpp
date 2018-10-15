#pragma once
#include "NetAddress.hpp"
#include "..\Core\Tools\Stopwatch.hpp"
#include "Engine\Net\NetPacket.hpp"


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
	void Flush();

	uint16_t GetNextAckToSend();

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

private: 
	
	// ack related stuff
	// sending - updated during a send/flush
	uint16_t					m_nextSentAck                    = 0U; 

	// receiving - updated during a process_packet
	uint16_t					m_lastReceivedAck                = INVALID_PACKET_ACK; 
	uint16_t					m_previousReceivedAckBitfield   = 0; 

	// Analytics
	uint						m_lastSendTimeMS;
	uint						m_lastRecievedTimeMS;

	// note these variables are unrelated to the debug sim on the session
	// but will end up reflecting those numbers.
	float						m_loss = 0.0f;       // loss rate we perceive to this connection
	float						m_rtt  = 0.0f;        // latency perceived on this connection


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