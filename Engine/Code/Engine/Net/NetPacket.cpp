#include "Engine/Net/NetPacket.hpp"
#include "Engine/Net/UDPSocket.hpp"

NetPacket::NetPacket()
	: BytePacker(PACKET_MTU, LITTLE_ENDIAN) // Set to max size you can send over LAN
{
	// move the write head over two bytes because we wont write the header till the end
	AdvanceWriteHead(2U);
}

//-----------------------------------------------------------------------------------------------
void NetPacket::WriteHeader()
{
	// set the first two bytes
	PacketHeader* bufferAsPacketHeader = (PacketHeader*) GetConstBuffer();
	bufferAsPacketHeader[0] = m_header;
	
	//WriteBytes(sizeof(header), &header);
}

//-----------------------------------------------------------------------------------------------
bool NetPacket::ReadHeader( PacketHeader* header)
{
	
	// this only works if we do it at the beginning
	// array of packet header[] = buffer
	// read index 0
	size_t sizeRead = ReadBytes(header, sizeof(PacketHeader));

	if(sizeRead < sizeof(header))
		return false;

	return true;
}

//-----------------------------------------------------------------------------------------------
bool NetPacket::WriteMessage(const NetMessage & msg)
{
	// Get a temp buffer to read into	
	BytePacker tempPacket(LITTLE_ENDIAN);

	// get the size of the message
	size_t sizeOfMsg = msg.GetWrittenByteCount();
	size_t totalSize = sizeOfMsg + (2U) + sizeof(NetMessageHeader);
	
	// make sure the size of the Packet fits into this buffer
	// if it can't just exit 
	if(GetWritableByteCount() < totalSize)
	{
		// Don't add to packet
		return false;
	}

	// we have room! add to this packet

	// write the size of the message
	tempPacket.WriteBytes(2, &sizeOfMsg);
	
	// write the header of the message
	tempPacket.WriteBytes(sizeof(NetMessageHeader), &msg.m_header);

	// write the payload
	tempPacket.WriteBytes(sizeOfMsg, msg.GetConstBuffer(), false);
	
	// Add to packet
	WriteBytes(totalSize, tempPacket.GetBuffer(), false);

	// keep track of how many messages are in this
	m_header.m_unreliableCount++;

}

//-----------------------------------------------------------------------------------------------
bool NetPacket::ReadMessage(NetMessage* out_msg)
{

	// so theoretically, by the time you call this function,
	// the next thing you read will be size, header, payload
	// in that order. The read head should be at the right place


	// Read the size of msg
	uint16_t size;
	size_t howMuchRead = ReadBytes(&size, 2);
	if(howMuchRead < (2U)) { return false; }
	
	// Read the header
	howMuchRead = ReadBytes(&out_msg->m_header, sizeof(NetMessageHeader), false);
	if(howMuchRead < sizeof(NetMessageHeader)) { return false; }

	// Read the message
	void* messageBuffer = out_msg->GetBuffer();
	howMuchRead = ReadBytes(&messageBuffer, size, false);
	out_msg->SetTotalWrittenByteCount(howMuchRead);
	if(howMuchRead < size) { return false; }

	return true;
}

//-----------------------------------------------------------------------------------------------
bool NetPacket::IsValid()
{
	// Get the packet header
	PacketHeader theHeader;
	size_t amountRead = ReadBytes(&theHeader, sizeof(PacketHeader));

	// packet is empty == garbage
	if(amountRead == 0U)
		return false;

	// get how many sizes
	uint amounfOfMessagesToCheckFor = theHeader.m_unreliableCount;

	// for that many sizes
	for(uint i = 0; i < amounfOfMessagesToCheckFor; i++)
	{
		// read the message size (and header)
		uint16_t sizeOfMessage;
		size_t result = ReadBytes(&sizeOfMessage, 2U);

		// we didn't get the amount we needed, message is garbage
		if(result < 2U)
		{
			return false;
		}
		
		// advance that much (check is valid)
		AdvanceReadHead(sizeOfMessage);
	}
	
	// make sure we stopped at the buffers writable head
	if(!(GetReadableHead() == (GetWritableHead() - 1U)))
	{
		// something went wrong
		return false;
	}

	// message is O K
	ResetRead();
	return true;
}
