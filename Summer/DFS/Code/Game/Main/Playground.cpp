#include "Playground.hpp"
#include "Engine\Net\NetSession.hpp"
#include "Engine\Net\NetConnection.hpp"
#include "Engine\Core\Tools\DevConsole.hpp"
#include "Engine\Core\Utils\StringUtils.hpp"
#include "Engine\Core\Platform\Time.hpp"

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

bool OnAdd( NetMessage & msg, const NetSender & from)
{
	float val0 = 0.f;
	float val1 = 0.f; 
	float sum = 0.f;

	bool checkForFirstNumber = msg.ReadBytes( &val0, sizeof(float) );
	bool checkForSecondNumber = msg.ReadBytes( &val1, sizeof(float) );

	if (!checkForFirstNumber || !checkForSecondNumber) {
		// this probaby isn't a real connection to send us a bad message
		return false; 
	}

	sum = val0 + val1; 
	DevConsole::AddConsoleDialogue( Stringf("Add: %f + %f = %f", val0, val1, sum )); 

	
	// send response
	NetMessage* response = new NetMessage( "add_response" ); 
	response->WriteBytes(sizeof(float), &val0);
	response->WriteBytes(sizeof(float), &val1);
	response->WriteBytes(sizeof(float), &sum);

	from.m_connection->Send( *response ); 

	return true; 
}

//-----------------------------------------------------------------------------------------------
bool OnAddResponse(NetMessage& msg, const NetSender& from)
{
	float val0 = 0.f;
	float val1 = 0.f; 
	float sum = 0.f;

	bool checkForFirstNumber = msg.ReadBytes( &val0, sizeof(float) );
	bool checkForSecondNumber = msg.ReadBytes( &val1, sizeof(float) );
	bool checkForSum = msg.ReadBytes(&sum, sizeof(float));

	if (!checkForFirstNumber || !checkForSecondNumber || !checkForSum) 
	{
		// this probaby isn't a real connection to send us a bad message
		return false; 
	}

	DevConsole::AddConsoleDialogue( Stringf("Answer from %s : %f + %f = %f", 
		from.m_connection->m_address.ToString().c_str(), 
		val0, 
		val1, 
		sum )); 
	
	return true;
}
