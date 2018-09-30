#include "NetMessage.hpp"

//-----------------------------------------------------------------------------------------------
NetMessage::NetMessage(const char * name)
	: BytePacker()
{
	m_definitionName = String(name);
}

