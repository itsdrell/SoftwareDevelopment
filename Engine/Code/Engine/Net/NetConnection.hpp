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

#define TRACKED_PACKET_WINDOW_HISTORY (128)

//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================
struct PacketTracker
{
	uint16_t		m_ackNumber = INVALID_PACKET_ACK;
	uint			m_sentMS = 0U;
};

//====================================================================================
// Classes
//====================================================================================
class NetConnection
{
	// the connection's index (luid - Local Unique IDentifier)
	// meaning once a connection is in the sessions array - do not move it
	friend class NetSession;

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

	bool OnReceivePacket( const PacketHeader& header, NetPacket* packet);
	void ConfirmPacketReceived( uint16_t ack );

	uint16_t GetNextAckToSend();
	void IncrementSendAck();

	PacketTracker* AddPacketTracker( uint16_t packetACK );
	PacketTracker* GetTracker( uint16_t ack );


	// clean up
	void ClearOutgoingMessages();

	void Send(NetMessage& messageToSend);

	void CheckHeartbeatTimer();
	void SetHeartbeatTimer( float hz );
	void SetFlushRate( float hz );
	void CompareFlushRatesAndSet();
	void UpdateLoss();

	// analytics 
	uint16_t GetLastRecievedAck() { return m_lastReceivedAck; }
	uint GetLastReceivedTimeInMS() { return m_lastRecievedTimeMS; }
	float GetLoss();
	float GetRTT();

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
	uint16_t					m_nextAckToSend                    = 0U; // this is zero because the next after invalid is 0
	uint16_t					m_lastSentAck						= INVALID_PACKET_ACK; // this is for the UI


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


	// our ring buffer
	PacketTracker				m_trackers[TRACKED_PACKET_WINDOW_HISTORY];

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