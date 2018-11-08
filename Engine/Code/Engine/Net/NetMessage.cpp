#include "NetMessage.hpp"
#include "UDPSocket.hpp"
#include "NetSession.hpp"
#include "..\Core\Tools\Clock.hpp"

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
NetMessage::NetMessage(const NetMessage& copyFrom)
{
	// set variables
	m_definitionName = copyFrom.m_definitionName;
	m_definition = copyFrom.m_definition;
	m_header = copyFrom.m_header;
	m_reliable_id = copyFrom.m_reliable_id;
	m_lastSentTimeMS = copyFrom.m_lastSentTimeMS;

	// change buffer
	SwapBuffer(copyFrom);
}

//-----------------------------------------------------------------------------------------------
bool NetMessage::RequiresConnection( const NetSession& theSession )
{
	NetMessageDefinition* theDef = theSession.GetMessageDefinitionByIndex(m_header.m_messageCallbackDefinitionIndex);

	if(theDef->m_option == NETMESSAGE_OPTION_CONNECTIONLESS)
		return false;

	return true;
}

//-----------------------------------------------------------------------------------------------
uint NetMessage::GetHeaderSize() const
{
	uint size = 1U;

	if( IsReliable() )
	{
		size += sizeof(uint16_t); // reliable id
	}

	if( IsReliableInOrder() )
	{
		size += (sizeof(uint16_t) * 2); // reliable and sequence ID 
	}

	return size;
}

//-----------------------------------------------------------------------------------------------
bool NetMessage::IsReliable() const
{
	if(m_definition->m_option == NETMESSAGE_OPTION_RELIABLE)
		return true;

	return false;
}

//-----------------------------------------------------------------------------------------------
bool NetMessage::IsReliableInOrder() const
{
	if(m_definition->m_option == NETMSSAGE_OPTION_RELIALBE_IN_ORDER)
		return true;

	return false;
}

//-----------------------------------------------------------------------------------------------
void NetMessage::ResetAge()
{
	m_lastSentTimeMS = GetTimeInMilliseconds();
}

