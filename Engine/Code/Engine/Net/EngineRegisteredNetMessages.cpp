#include "EngineRegisteredNetMessages.hpp"
#include "..\Core\Tools\DevConsole.hpp"
#include "Engine\Net\NetConnection.hpp"
#include "Engine\Net\NetAddress.hpp"
#include "..\Core\Utils\StringUtils.hpp"



//===============================================================================================
void RegisterCoreEngineNetMessages( NetSession& theSession )
{
	theSession.RegisterMessageDefinition(	NETMSG_HEARTBEAT,	"heartbeat",	OnHeartbeat);
	theSession.RegisterMessageDefinition(	NETMSG_PING,		"ping",			OnPing,		NETMESSAGE_OPTION_CONNECTIONLESS ); 
	theSession.RegisterMessageDefinition(	NETMSG_PONG,		"pong",			OnPong,		NETMESSAGE_OPTION_CONNECTIONLESS );
}

//-----------------------------------------------------------------------------------------------
bool OnHeartbeat(NetMessage& msg, const NetSender& from)
{
	UNUSED(msg);
	
	DevConsole::AddConsoleDialogue("Received a heartbeat from: " + from.m_connection->m_address.ToString());
	return true;
}

//-----------------------------------------------------------------------------------------------
bool OnPing( NetMessage & msg, const NetSender & from)
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
bool OnPong( NetMessage & msg, const NetSender & from)
{

	//char str[20]; 
	//msg.ReadString( str , 20U);  
	//msg.ReadBytes(str, msg.GetWrittenByteCount());

	UNUSED(msg);

	DevConsole::AddConsoleDialogue( Stringf( "Received Pong from %s", 
		from.m_connection->m_address.ToString().c_str())); 

	return true;
}