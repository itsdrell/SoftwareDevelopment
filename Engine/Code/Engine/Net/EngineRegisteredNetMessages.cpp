#include "EngineRegisteredNetMessages.hpp"
#include "..\Core\Tools\DevConsole.hpp"
#include "Engine\Net\NetConnection.hpp"
#include "Engine\Net\NetAddress.hpp"



//===============================================================================================
void RegisterEngineNetMessages( NetSession& theSession )
{
	theSession.RegisterMessageDefinition("heartbeat", OnHeartbeat);
}

//-----------------------------------------------------------------------------------------------
bool OnHeartbeat(NetMessage& msg, const NetSender& from)
{
	UNUSED(msg);
	
	DevConsole::AddConsoleDialogue("Received a heartbeat from: " + from.m_connection->m_address.ToString());
	return true;
}
