#include "EngineRegisteredNetMessages.hpp"
#include "..\Core\Tools\DevConsole.hpp"
#include "Engine\Net\NetConnection.hpp"
#include "Engine\Net\NetAddress.hpp"
#include "..\Core\Utils\StringUtils.hpp"
#include "Engine\Net\NetSession.hpp"



//===============================================================================================
void RegisterCoreEngineNetMessages( NetSession& theSession )
{
	theSession.RegisterMessageDefinition(	NETMSG_HEARTBEAT,			"heartbeat",			OnHeartbeat);
	theSession.RegisterMessageDefinition(	NETMSG_PING,				"ping",					OnPing,				NETMESSAGE_OPTION_CONNECTIONLESS ); 
	theSession.RegisterMessageDefinition(	NETMSG_PONG,				"pong",					OnPong,				NETMESSAGE_OPTION_CONNECTIONLESS );

	theSession.RegisterMessageDefinition(	NETMSG_JOIN_REQUEST,		"joinRequest",			OnJoinRequest,		NETMESSAGE_OPTION_CONNECTIONLESS );
	theSession.RegisterMessageDefinition(	NETMSG_JOIN_DENY,			"joinDeny",				OnJoinDeny,			NETMESSAGE_OPTION_CONNECTIONLESS );
	
	theSession.RegisterMessageDefinition(	NETMSG_JOIN_ACCEPT,			"joinAccept",			OnJoinAccept,		NETMSSAGE_OPTION_RELIALBE_IN_ORDER);
	theSession.RegisterMessageDefinition(	NETMSG_NEW_CONNECTION,		"newConnection",		OnNewConnection,	NETMSSAGE_OPTION_RELIALBE_IN_ORDER);
	theSession.RegisterMessageDefinition(	NETMSG_JOIN_FINISHED,		"joinFinished",			OnJoinFinished,		NETMSSAGE_OPTION_RELIALBE_IN_ORDER);
	theSession.RegisterMessageDefinition(	NETMSG_UPDATE_CONN_STATE,	"updateConState",		OnUpdateConnState,	NETMSSAGE_OPTION_RELIALBE_IN_ORDER);
	
	theSession.RegisterMessageDefinition(	NETMESG_HANGUP,				"hangup",				OnHangup);
}

//-----------------------------------------------------------------------------------------------
bool OnHeartbeat(NetMessage& msg, const NetSender& from )
{
	uint sendersTime;
	msg.ReadBytes(&sendersTime, sizeof(uint));

	float seconds = (float)sendersTime * .001f;

	from.GetSession()->ProcessHeartbeatTime(sendersTime);
	
	DevConsole::AddConsoleDialogue("Received a heartbeat from: " + from.m_connection->m_address.ToString() + " and their time is: " + std::to_string(seconds));
	
	return true;
}

//-----------------------------------------------------------------------------------------------
bool OnPing( NetMessage & msg, const NetSender & from )
{
	char str[20];  
	msg.ReadBytes(str, msg.GetWrittenByteCount());

	DevConsole::AddConsoleDialogue( Stringf("Received ping from %s: %s", 
		from.m_connection->m_address.ToString().c_str(), 
		str )); 

	// ping responds with pong
	NetMessage* pong = new NetMessage( "pong" ); 
	from.m_connection->Send( *pong ); 

	// all messages serve double duty
	// do some work, and also validate
	// if a message ends up being malformed, we return false
	// to notify the session we may want to kick this connection; 
	return true; 
}

//-----------------------------------------------------------------------------------------------
bool OnPong( NetMessage & msg, const NetSender & from )
{

	//char str[20]; 
	//msg.ReadString( str , 20U);  
	//msg.ReadBytes(str, msg.GetWrittenByteCount());

	UNUSED(msg);

	DevConsole::AddConsoleDialogue( Stringf( "Received Pong from %s", 
		from.m_connection->m_address.ToString().c_str())); 

	return true;
}

//-----------------------------------------------------------------------------------------------
bool OnJoinRequest(NetMessage & msg, const NetSender & from )
{	
	NetConnectionInfo newConnection;
	newConnection.m_address = from.m_connection->m_address;

	// read the char name from packet
	char* name = nullptr;
//	msg.ReadString(name,16U);
	strcpy_s(newConnection.m_id, "idk");
	
	return from.GetSession()->ProcessJoinRequest(newConnection);
}

//-----------------------------------------------------------------------------------------------
bool OnJoinDeny(NetMessage & msg, const NetSender & from )
{	
	UNUSED(msg);
	
	from.GetSession()->m_state = SESSION_DISCONNECTED;
	from.m_connection->m_state = NET_CONNECTION_STATUS_DISCONNECTED;
	
	return true;
}

//-----------------------------------------------------------------------------------------------
bool OnJoinAccept(NetMessage & msg, const NetSender & from )
{
	uint8_t idx;
	msg.ReadBytes(&idx, 1U);

	NetSession* theSession = from.GetSession();
	//theSession->BindConnection(idx, theSession->m_myConnection);
	theSession->m_boundConnections[idx] = theSession->m_myConnection;

	theSession->m_state = SESSION_READY;
	
	return false;
}

//-----------------------------------------------------------------------------------------------
bool OnNewConnection(NetMessage & msg, const NetSender & from )
{
	// for peer to peer telling everyone about new connections
	return false;
}

//-----------------------------------------------------------------------------------------------
bool OnJoinFinished(NetMessage & msg, const NetSender & from )
{
	DevConsole::AddConsoleDialogue("Join is Finished!");

	NetSession* theSession = from.GetSession();
	theSession->m_myConnection->m_state = NET_CONNECTION_STATUS_CONNECTED;

	return false;
}

//-----------------------------------------------------------------------------------------------
bool OnUpdateConnState(NetMessage & msg, const NetSender & from )
{
	uint8_t connectionStateToChangeTo;
	msg.ReadBytes(&connectionStateToChangeTo, 1U);

	from.GetSession()->UpdateConnectionState(connectionStateToChangeTo, *from.m_connection);


	// we may not have a connection yet so just send a message to em?
	//NetMessage* newMessage = new NetMessage("updateConState");
	//newMessage->WriteBytes(1U, &m_myConnection->m_state);
	//
	//current->Send(*newMessage);
	
	return false;
}

//-----------------------------------------------------------------------------------------------
bool OnHangup(NetMessage& msg, const NetSender& from)
{
	NetSession* theSession = from.GetSession();

	DevConsole::AddConsoleDialogue(Stringf("Connection at index %u hung up and was disconnected", from.m_connection->m_indexInSession));

	NetConnection* conenctioToDisconect = theSession->GetConnection(from.m_connection->m_indexInSession);
	conenctioToDisconect->Disconnect();
	//theSession->DestroyConnection(from.m_connection);

	return true;
}
