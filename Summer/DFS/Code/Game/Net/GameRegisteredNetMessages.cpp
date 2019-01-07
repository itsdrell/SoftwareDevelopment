#include "GameRegisteredNetMessages.hpp"
#include "Engine\Core\Tools\DevConsole.hpp"
#include "Engine\Core\Utils\StringUtils.hpp"
#include "Engine\Net\NetSession.hpp"


//===============================================================================================
void RegisterGameNetMessages( NetSession& theSession )
{
	theSession.RegisterMessageDefinition(NETMSG_UNRELIABLE_TEST, "unreliable_test", OnUnreliableTest);
	theSession.RegisterMessageDefinition(NETMSG_RELIABLE_TEST, "reliable_test", OnReliableTest, NETMESSAGE_OPTION_RELIABLE);
	theSession.RegisterMessageDefinition(NETMSG_SEQUENCE_TEST, "sequence_test", OnSequenceTest, NETMSSAGE_OPTION_RELIALBE_IN_ORDER);


	// EXTRA - make sure this still works (id'less messages)
	// They still get sorted alphabetically, but are inserted around the fixed
	// ones in the final lookup.  
	//m_session->register_message( "test", OnTest ); 
}

//-----------------------------------------------------------------------------------------------
bool OnUnreliableTest(NetMessage & msg, const NetSender & from )
{
	UNUSED(msg);
	UNUSED(from);

	DevConsole::GetInstance()->AddConsoleDialogue("Got something unreliable");
	
	return true;
}

//-----------------------------------------------------------------------------------------------
bool OnReliableTest(NetMessage& msg, const NetSender& from )
{
	UNUSED(msg);
	UNUSED(from);
	
	DevConsole::GetInstance()->AddConsoleDialogue("Got something reliable");
	return true;
}

//-----------------------------------------------------------------------------------------------
bool OnSequenceTest(NetMessage& msg, const NetSender& from )
{
	UNUSED(from);
	
	uint val0 = 0U;
	uint val1 = 0U; 

	msg.ReadBytes( &val0, sizeof(uint) );
	msg.ReadBytes( &val1, sizeof(uint) );

	DevConsole::GetInstance()->AddConsoleDialogue(Stringf("Got something reliable IN ORDER %u/%u", val0, val1));
	return true;
}
