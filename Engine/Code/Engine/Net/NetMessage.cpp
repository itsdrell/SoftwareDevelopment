#include "NetMessage.hpp"
#include "UDPSocket.hpp"

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

