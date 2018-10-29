#include "Engine/Net/NetPacket.hpp"
#include "Engine/Net/UDPSocket.hpp"
#include "NetSession.hpp"
#include "NetConnection.hpp"

NetPacket::NetPacket()
	: BytePacker(PACKET_MTU, LITTLE_ENDIAN) // Set to max size you can send over LAN
{
	// move the write head over two bytes because we wont write the header till the end
	AdvanceWriteHead(sizeof(PacketHeader));
}

//-----------------------------------------------------------------------------------------------
NetPacket::NetPacket(bool advanceWriteHeadForHeader)
	: BytePacker(PACKET_MTU, LITTLE_ENDIAN) // Set to max size you can send over LAN
{
	if(advanceWriteHeadForHeader)
		AdvanceWriteHead(sizeof(PacketHeader));

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
	size_t sizeOfMessageAndHeader = sizeOfMsg + sizeof(NetMessageHeader);
	size_t totalLength = sizeOfMessageAndHeader + 2U;
	
	// make sure the size of the Packet fits into this buffer
	// if it can't just exit 
	if(GetWritableByteCount() < (totalLength))
	{
		// Don't add to packet
		return false;
	}

	// we have room! add to this packet

	// write the size of the message
	tempPacket.WriteBytes(2U, &sizeOfMessageAndHeader);
	
	// write the header of the message
	tempPacket.WriteBytes(sizeof(NetMessageHeader), &msg.m_header);

	// write the payload
	tempPacket.WriteBytes(sizeOfMsg, msg.GetConstBuffer(), false);
	
	// Add to packet
	WriteBytes(totalLength, tempPacket.GetBuffer(), false);

	// keep track of how many messages are in this
	m_header.m_unreliableCount++;

	return true;

}

//-----------------------------------------------------------------------------------------------
bool NetPacket::ReadMessage(NetMessage* out_msg)
{

	// so theoretically, by the time you call this function,
	// the next thing you read will be size, header, payload
	// in that order. The read head should be at the right place


	// Read the size of msg
	uint16_t size;
	size_t howMuchRead = ReadBytes(&size, 2U);
	if(howMuchRead != 2U) { return false; }
	
	// Read the header
	NetMessageHeader theHeader;
	size_t amountToRead = sizeof(NetMessageHeader); // this should be 1 atm
	size_t howMuchReadForHeader = ReadBytes(&theHeader, amountToRead);
	if(howMuchReadForHeader != amountToRead) { return false; }
	out_msg->m_header = theHeader;

	// Read the message
	void* messageBuffer = out_msg->GetBuffer();   // IF ANYTHING IS WRONG IT'S PROBABLY THE -1
	size_t bufferRead = ReadBytes(messageBuffer, (size - 1));
	out_msg->SetTotalWrittenByteCount(bufferRead);
	if(bufferRead != (size - 1)) { return false; }

	return true;
}

//-----------------------------------------------------------------------------------------------
bool NetPacket::IsValid(const NetSession& theSession)
{
	// Get the packet header
	PacketHeader theHeader;
	size_t amountRead = ReadBytes(&theHeader, sizeof(PacketHeader));


	// WE DO THIS NOW PER MESSAGE
	// see if the packet is lying about who they are 
	//(since we already check to see if the connection is valid)
	//if(theSession.GetConnection(theHeader.m_senderConnectionIndex) == nullptr 
	//	&& theHeader.m_senderConnectionIndex != INVALID_CONNECTION_INDEX)
	//	return false;


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
	if(!(GetReadableHead() == (GetWritableHead() /*- 1U*/)))
	{
		// something went wrong
		return false;
	}

	// message is O K
	ResetRead();
	return true;
}
