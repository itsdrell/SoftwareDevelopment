#pragma once
#include "Socket.hpp"
#include "NetAddress.hpp"


// Make appropriate changes to TCPSocket if you want
// though nothing *required* in class will use TCPSocket again.

// when searching for addresses - you are no longer looking for AF_INET
// you look for 

class UDPSocket : public Socket
{
public: 

	UDPSocket() {}

	bool bind( NetAddress const &addr, // address I want to listen for traffic on
		uint16_t port_range = 0U );		  // how many additional ports to bind on (so you bind from [addr.port,addr.port + port_range])

										  // return 0 on failure, otherwise how large is the next datagram
										  // TODO in class
	size_t SendTo( NetAddress const &addr, void const *data, size_t const byte_count );
	size_t ReceiveFrom( NetAddress *out_addr, void *buffer, size_t const max_read_size );
};

// cpp


//===============================================================================================
// Delete pls
//===============================================================================================

// class test
#define GAME_PORT ("10084")
#define ETHERNET_MTU 1500  // maximum transmission unit - determined by hardware part of OSI model.
// 1500 is the MTU of EthernetV2, and is the minimum one - so we use it; 
#define PACKET_MTU (ETHERNET_MTU - 40 - 8) 

// IPv4 Header Size: 20B
// IPv6 Header Size: 40B
// TCP Headre Size: 20B-60B
// UDP Header Size: 8B 
// Ethernet: 28B, but MTU is already adjusted for it
// so packet size is 1500 - 40 - 8 => 1452B (why?)

class UDPTest
{
public:
	UDPTest();

	static UDPTest* GetInstance();

	bool Start();

	void Stop() { m_socket.Close(); }

	void SendTo( NetAddress const &addr, void const *buffer, uint byte_count ) { m_socket.SendTo( addr, buffer, byte_count ); }

	void Update();

public:
	// if you have multiple address, you can use multiple sockets
	// but you have to be clear on which one you're sending from; 
	UDPSocket			m_socket; 

	static UDPTest*		s_instance;
};
