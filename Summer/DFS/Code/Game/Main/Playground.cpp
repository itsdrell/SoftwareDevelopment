#include "Playground.hpp"
#include "Engine\Net\NetSession.hpp"
#include "Engine\Net\NetConnection.hpp"
#include "Engine\Core\Tools\DevConsole.hpp"
#include "Engine\Core\Utils\StringUtils.hpp"

//-----------------------------------------------------------------------------------------------
// Put stuff in here if you just wanna test it once and itll happen in startup
void Playground::RunTestOnce()
{
	
}

//-----------------------------------------------------------------------------------------------
// Put stuff in here if you wanna see something happen multiple times
void Playground::RunTestOnUpdate()
{
	
}

//===============================================================================================
bool OnPing( NetMessage & msg, const NetSender & from)
{
	char str[20];  
	msg.ReadString( str , 20U); 

	DevConsole::AddConsoleDialogue( Stringf("Received ping from %s: %s", 
		from.m_connection->m_address.ToString().c_str(), 
		str )); 

	// ping responds with pong
	NetMessage pong( "pong" ); 
	from.m_connection->Send( pong ); 

	// all messages serve double duty
	// do some work, and also validate
	// if a message ends up being malformed, we return false
	// to notify the session we may want to kick this connection; 
	return true; 
}

bool OnPong( NetMessage & msg, const NetSender & from)
{
	char str[20]; 
	msg.ReadString( str , 20U);  

	DevConsole::AddConsoleDialogue( Stringf( "Received ping from %s: %s", 
		from.m_connection->m_address.ToString().c_str(), 
		str)); 

	return true;
}

bool OnAdd( NetMessage & msg, const NetSender & from)
{
	float val0;
	float val1; 
	float sum;

	if (!msg.ReadBytes( &val0, sizeof(float) ) || !msg.ReadBytes( &val1, sizeof(float) )) {
		// this probaby isn't a real connection to send us a bad message
		return false; 
	}

	sum = val0 + val1; 
	DevConsole::AddConsoleDialogue( Stringf("Add: %f + %f = %f", val0, val1, sum )); 

	// would could send back a response here if we want;
	// ...

	return true; 
}
