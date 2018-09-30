#pragma once
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Net/NetPacket.hpp"
#include "Engine/Net/UDPSocket.hpp"
#include "Engine/Core/General/EngineCommon.hpp"

//====================================================================================
// Forward Declare
//====================================================================================


//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
// collection of UDP sockets to communicate on
// can allocate and free packets
class PacketChannel 
{
public:

	PacketChannel();
	~PacketChannel();

	// tries to bind to an additional UDP Socket
	// returns the index assigned to this socket.  
	// returns -1 if it failed to bind; 
	int Bind( NetAddress &addr , uint range_to_try); 
	void Send( NetAddress const &to, const NetPacket& packet ); 

	// Returns whether 
	bool Receive( NetAddress const &from, NetPacket const &packet ); 

public:
	// probably at least one UDPSocket
	UDPSocket*				m_socket;

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