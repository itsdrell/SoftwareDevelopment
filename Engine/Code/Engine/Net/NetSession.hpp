#pragma once
#include "NetAddress.hpp"
#include "PacketChannel.hpp"
#include <map>
#include "NetMessage.hpp"
#include "..\Math\Ranges\IntRange.hpp"
#include "..\Core\Tools\Stopwatch.hpp"
#include "Engine\Net\NetConnection.hpp"


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
class NetSession;

//====================================================================================
// Type Defs + Defines
//====================================================================================
#define NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS 16
#define GAME_PORT ("10084")

#define SESSION_SEND_JOIN_RATE (.1f)
#define SESSION_TIMEOUT_RATE (10.f)

//====================================================================================
// ENUMS
//====================================================================================
enum eNetCoreMessage : uint8_t
{
	NETMSG_PING = 0,				// unreliable, connectionless
	NETMSG_PONG, 					// unreliable, connectionless
	NETMSG_HEARTBEAT,				// unreliable
	NETMSG_JOIN_REQUEST,			// unreliable
	NETMSG_JOIN_DENY,				// unreliable
	NETMSG_JOIN_ACCEPT,				// reliable in order
	NETMSG_NEW_CONNECTION,			// reliable in order
	NETMSG_JOIN_FINISHED,			// reliable in order
	NETMSG_UPDATE_CONN_STATE,		// reliable in order


	NETMSG_CORE_COUNT,
};

//-----------------------------------------------------------------------------------------------
enum eSessionState
{
	SESSION_DISCONNECTED = 0,  // Session can be modified     
	SESSION_BOUND,             // Bound to a socket - can send and receive connectionless messages.  No connections exist
	SESSION_CONNECTING,        // Attempting to connecting - waiting for response from the host
	SESSION_JOINING,           // Has established a connection, waiting final setup information/join completion
	SESSION_READY,             // We are fully in the session
};
String NetSessionStateToString( eSessionState theState );

//-----------------------------------------------------------------------------------------------
enum eSessionError
{
	SESSION_OK,
	SESSION_ERROR_USER,                 // user disconnected
	SESSION_ERROR_INTERNAL,             // socket error; 

	SESSION_ERROR_JOIN_DENIED,          // generic deny error (release)
	SESSION_ERROR_JOIN_DENIED_NOT_HOST, // debug - tried to join someone who isn't hosting
	SESSION_ERROR_JOIN_DENIED_CLOSED,   // debug - not in a listen state
	SESSION_ERROR_JOIN_DENIED_FULL,     // debug - session was full 
};

//-----------------------------------------------------------------------------------------------
enum eNetMessageOption : uint 
{
	NETMESSAGE_OPTION_CONNECTIONLESS	= BIT_FLAG(0), // task14
	NETMESSAGE_OPTION_RELIABLE			= BIT_FLAG(1), // task15
	NETMESSAGE_OPTION_IN_ORDER			= BIT_FLAG(2), // task16

	// convenience
	NETMSSAGE_OPTION_RELIALBE_IN_ORDER	= NETMESSAGE_OPTION_RELIABLE | NETMESSAGE_OPTION_IN_ORDER, 
};
// typedef uint eNetMessageOptions; defined in net message


//====================================================================================
// Structs
//====================================================================================
struct NetSender
{
	NetSender() {}

	~NetSender();
	
	// just a connection for now but later will change
	NetSender(NetConnection& theConnection)
		: m_connection(&theConnection) {}

	NetSender(NetConnection* theConnection)
		: m_connection(theConnection) {}

	NetSession* GetSession() const { return m_connection->m_owningSession; }
	
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

	void Host( char const *my_id, const char* port, uint16_t port_range = 16U ); 
	void Join( char const *my_id, const NetAddress& hostAddress );
	void Disconnect(); 
	bool IsRunning() { return m_state != SESSION_DISCONNECTED; };

	bool ProcessJoinRequest( const NetConnectionInfo& connectionWantingToJoin );
	bool CanWeAcceptTheConnection( const NetConnectionInfo& connectionWantingToJoin );
	void SendJoinDeny( const NetConnectionInfo& connectionWantingToJoin );
	void SendJoinAccept( NetConnection& connectionWantingToJoin );
	void UpdateConnectionState( uint8_t stateToChangeTo, const NetConnection& theConnection);

	void SetError( eSessionError error, char const *str = nullptr ); 
	void ClearError(); 
	eSessionError GetLastError( std::string *out_str = nullptr ); // get last error has an implicit clear in it

	void Update(); 
	void SessionConnectingUpdate();
	void SessionJoiningUpdate();
	void SessionReadyUpdate();
	void CheckForDisconnects();

	// should be private, but for ease we'll keep public
	// connection management
	NetConnection* CreateConnection( const NetConnectionInfo& info ); 
	void DestroyConnection( NetConnection *cp );
	void DestroyAllConnections();
	uint8_t BindConnection( uint8_t idx, NetConnection *cp ); 
	bool IsConnectionIndexValid(const uint8_t m_sessionIndex);
	bool DoWeAlreadyHaveThisConnectionCreated( const NetConnectionInfo& theInfo );

	// message definitions
	bool RegisterMessageDefinition( uint8_t id, const String& name, NetMessage_cb cb, eNetMessageOptions option = 0 );
	bool IsMessageAlreadyRegistered(const String& id);
	
	NetMessageDefinition* GetMessageDefinition( int const &id );
	NetMessageDefinition* GetMessageDefinitionByIndex( uint8_t const idx ) const ; 
	NetMessageDefinition* GetMessageDefinitionByName( const String& name ) const;

	void SortDefinitions();

	// Connection management
	// deprecated
	//NetConnection* AddConnection( uint idx, NetAddress const &addr );  
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

	// Starting a session (finalizes definitions - can't add more once
	// the session is running)
private:
	bool Bind( const char* port, uint range_to_try = 0U );

	void SendDirectMessageTo( NetMessage &messageToSend, const NetAddress& address );

public:
	//===============================================================================================
	// trash pls
	void SendUnreliableTest();
	void SendReliableTest();

	// unreliable vars
	uint m_idx = 0;
	uint m_totalAmount = 0;
	uint m_currentAmount = 0U;
	Timer*	m_unreliableTimer = nullptr;

	// reliable vars
	uint m_reliableIdx = 0;
	uint m_reliableTotalAmount = 0;
	uint m_reliableCurrentAmount = 0U;
	Timer*	m_reliableTimer = nullptr;
	//===============================================================================================

public:
	PacketChannel*								m_channel = nullptr; // what we send/receive packets on;

	// state management
	eSessionState								m_state = SESSION_DISCONNECTED; 

	// error handling
	eSessionError								m_errorCode; 
	String										m_errorString; 

	// Changes to how we store connections;
	std::vector<NetConnection*>					m_AllConnections;
	NetConnection*								m_boundConnections[NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS]; // all connections I know about; 

	NetConnection*								m_myConnection   = nullptr;     // convenience pointer
	NetConnection*								m_hostConnection = nullptr;   // convenience pointer; 

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

	// Timers for joining and Timing out
	Timer*										m_sendJoinRequestTimer = nullptr;
	Timer*										m_timeOutTimer = nullptr;

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