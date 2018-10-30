#include "GameRegisteredNetMessages.hpp"
#include "Engine\Core\Tools\DevConsole.hpp"


//===============================================================================================
void RegisterGameNetMessages( NetSession& theSession )
{
	theSession.RegisterMessageDefinition(NETMSG_UNRELIABLE_TEST, "unreliable_test", OnUnreliableTest);
	theSession.RegisterMessageDefinition(NETMSG_RELIABLE_TEST, "reliable_test", OnReliableTest, NETMESSAGE_OPTION_RELIABLE);

	// EXTRA - make sure this still works (id'less messages)
	// They still get sorted alphabetically, but are inserted around the fixed
	// ones in the final lookup.  
	//m_session->register_message( "test", OnTest ); 
}

//-----------------------------------------------------------------------------------------------
bool OnUnreliableTest(NetMessage & msg, const NetSender & from)
{
	
	DevConsole::GetInstance()->AddConsoleDialogue("Got something unreliable");
	
	return false;
}

//-----------------------------------------------------------------------------------------------
bool OnReliableTest(NetMessage& msg, const NetSender& from)
{
	DevConsole::GetInstance()->AddConsoleDialogue("Got something reliable");
	return false;
}
