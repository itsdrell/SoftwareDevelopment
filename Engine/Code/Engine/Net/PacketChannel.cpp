#include "PacketChannel.hpp"

//===============================================================================================
// Packet Channel
PacketChannel::PacketChannel()
{
	m_socket = new UDPSocket();
}

//-----------------------------------------------------------------------------------------------
PacketChannel::~PacketChannel()
{
	m_socket->Close();
	delete m_socket;
	m_socket = nullptr;
}

//-----------------------------------------------------------------------------------------------
int PacketChannel::Bind(NetAddress& addr, uint range_to_try)
{
	bool result = m_socket->Bind(addr, (uint16_t) range_to_try);

	if(result == false)
		return -1;
	
	m_socket->SetBlocking(false);
	return 1;
}

//-----------------------------------------------------------------------------------------------
void PacketChannel::Send(NetAddress const & to, NetPacket const & packet)
{
	m_socket->SendTo(to, packet.GetConstBuffer(), packet.GetWrittenByteCount());
}

//-----------------------------------------------------------------------------------------------
bool PacketChannel::Receive(NetAddress const & from, NetPacket const & packet)
{
	UNUSED(from);
	UNUSED(packet);

	return false;
}
