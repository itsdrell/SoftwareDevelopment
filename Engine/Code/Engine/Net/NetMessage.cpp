#include "NetMessage.hpp"
#include "UDPSocket.hpp"
#include "NetSession.hpp"

//-----------------------------------------------------------------------------------------------
NetMessage::NetMessage(const char * name)
	: BytePacker(PACKET_MTU, LITTLE_ENDIAN)
{
	m_definitionName = String(name);
}

NetMessage::NetMessage()
	: BytePacker(PACKET_MTU, LITTLE_ENDIAN)
{

}

//-----------------------------------------------------------------------------------------------
bool NetMessage::RequiresConnection( const NetSession& theSession )
{
	NetMessageDefinition* theDef = theSession.GetMessageDefinitionByIndex(m_header.m_messageCallbackDefinitionIndex);

	if(theDef->m_option == NETMESSAGE_OPTION_CONNECTIONLESS)
		return false;

	return true;
}

