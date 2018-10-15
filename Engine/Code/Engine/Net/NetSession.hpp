#pragma once
#include "NetAddress.hpp"
#include "PacketChannel.hpp"
#include <map>
#include "NetMessage.hpp"
#include "..\Math\Ranges\IntRange.hpp"


//====================================================================================
//									NOTES:
//____________________________________________________________________________________
// 
// hz = 1 / seconds
// seconds = 1 / hz
//
//====================================================================================

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


//-----------------------------------------------------------------------------------------------
struct TimeStampedPacket
{
	TimeStampedPacket(int delay, NetPacket* thePacket, const NetAddress& theAddress );

	int					m_delay;
	double				m_timeToBeProcessed;
	NetAddress			m_whoSentThePacket;
	NetPacket*			m_packetToBeProcessed;
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
	void AddPacketToQueue( NetPacket& packet, const NetAddress& sender);
	void ProcessTimeStampedPackets();

	void ProcessOutgoing();
	void SendPacket( const NetPacket& packet );

	void Render() const;

	NetConnection* GetMyConnection() const;
	uint GetMySessionIndex() const { return m_connectionsIndexInSession; }
	uint8_t GetConnectionForPacket(const PacketHeader& theHeader, const NetAddress& theAddressOfSender);
	NetConnection* GetConnection(int idx) const;
	NetConnection* GetConnectionFromAddress(const NetAddress& sender) const;

	void SetSimulateLoss( float lossAmount) { m_lossAmount = lossAmount; }
	void SetSimulatedLatency( int minAddedLatencyMS, int maxAddedLatencyMS = 0);
	void SetHeartbeat( float hz );
	void SetSessionFlushRate( float hz );
	void SetConnectionFlushRate( uint idx, float hz );

	float GetFlushRate() { return m_sessionFlushRate; }


public:
	NetConnection*								m_connections[NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS]; // all connections I know about; 
	PacketChannel								m_channel; // what we send/receive packets on;

private:
	// Sim Loss and latency
	float										m_lossAmount = 0.f; // 0 means you lose none, 100.f you lose all
	IntRange									m_latencyRange;

	// heartbeat rate
	float										m_heartbeatRate = .2f; // this is in hz

	// send rate
	float										m_sessionFlushRate = 20.f; // hz

	// My connections index in the session
	uint										m_connectionsIndexInSession = 0U;

	// time stamped packets
	std::vector<TimeStampedPacket>				m_timeStampedPacketQueue;

	// gonna keep the list here, instead of in NetMessageDefinition so that
	// multiple sessions can have different callbacks!
	std::vector<NetMessageDefinition*>			m_messageCallbacks;

public:
	static NetSession* s_mainNetSession;
};


//====================================================================================
// Standalone C Functions
//====================================================================================
bool CompareTimeStampedPacket(const TimeStampedPacket& a, const TimeStampedPacket& b);

//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [9/22/2018]
//====================================================================================