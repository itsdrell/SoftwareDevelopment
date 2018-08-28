#pragma once
#include "..\Core\General\EngineCommon.hpp"

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
struct sockaddr; // gets around including

//====================================================================================
// Classes
//====================================================================================
class NetAddress
{
public:
	NetAddress() {} 
	NetAddress( sockaddr const *addr ); 
	NetAddress( char const *string ); 


	bool FromSocketAddress(const sockaddr* addr);
	bool ToSocketAddress( sockaddr* addr, size_t* out_size) const;

	String ToString() const; 

	static NetAddress GetLocalAddress();
	static bool GetBindableAddress( NetAddress* outAddress, String port);

public:
	uint		m_address = 0U;
	uint16		m_port = 0U;
};

//====================================================================================
// Standalone C Functions
//====================================================================================
bool GetAddressForHost( sockaddr* out, int* out_addrlen, char const* hostname, char const* service = "12345");


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [8/20/2018]
//====================================================================================