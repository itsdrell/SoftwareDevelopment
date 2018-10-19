#pragma once
#include "..\Core\General\BytePacker.hpp"
#include "Engine\Net\NetMessage.hpp"


//====================================================================================
// Forward Declare
//====================================================================================
class NetSession;

//====================================================================================
// Type Defs + Defines
//====================================================================================
#define INVALID_PACKET_ACK (0xffff)

//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================
struct PacketHeader
{
	PacketHeader() {}
	
	PacketHeader(uint8_t theSendersConnectionIndex, uint8_t unreliableCount)
		: m_senderConnectionIndex(theSendersConnectionIndex)
		, m_unreliableCount(unreliableCount) {}
	
	uint8_t m_senderConnectionIndex = 0U; // conn idx of the sender of this packer

	uint16_t m_ack = INVALID_PACKET_ACK;
	uint16_t m_lastRecievedAck = INVALID_PACKET_ACK;
	uint16_t m_previousRecievedAckBitfield;

	uint8_t m_unreliableCount = 0U; // number of unreliable messages in this container; 
}; 

//====================================================================================
// Classes
//====================================================================================

// A packet is a package sent over the network
// It allows you to write/read data to it
// also contains who the recipient and/or sender are
// Contains extra functionality for writing headers/messages.
//
// Think of it as the box goods are sent in; 
// UES LITTLE_ENDIAN
class NetPacket : public BytePacker
{
public:

	NetPacket();

	// this is for when write a message, we auto make room for the header
	// and when we read, it is not already moved!
	NetPacket(bool advanceWriteHeadForHeader);

	//void WriteHeader( const PacketHeader &header );
	void WriteHeader(); // writes it own header
	bool ReadHeader(  PacketHeader* header ); 

	// writes/reads the following
	// - uint16_t header_and_msg_size (header size is always 1 for now, msg size is msg.get_total_written_bytes())
	// - header
	//   - uint8_t msg_definition_index  
	bool WriteMessage( const NetMessage& msg ); 
	bool ReadMessage( NetMessage *out_msg ); 

	void ResetMessageCount() { m_header.m_unreliableCount = 0U; }

	bool IsValid(const NetSession& theSession);

public:
	// we will just allocate the buffer with the 
	// packet to prevent hitting the heap - be 
	// sure to use the valid BytePacker constructor; 
	//byte_t m_local_buffer[PACKET_MTU]; 

	// information so we know who it came from
	// when receiving, and who we're sending to when sending
	// ...
	PacketHeader			m_header;
	uint8_t					m_indexToWhoWeAreSendingTo = 0U;
};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [9/22/2018]
//====================================================================================