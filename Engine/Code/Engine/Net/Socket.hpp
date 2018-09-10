#pragma once
#include "..\Core\General\BytePacker.hpp"
#include "NetAddress.hpp"


//====================================================================================
// Forward Declare
//====================================================================================


//====================================================================================
// Type Defs + Defines
//====================================================================================
// net/socket.hpp
// this is just so it is a typed pointer - but
// internally we cast socket_t to SOCKET
typedef void* Socket_t;

//====================================================================================
// ENUMS
//====================================================================================
enum eSocketOptionBit : unsigned int 
{
	SOCKET_OPTION_BLOCKING = BIT_FLAG(0),

	// some other options that may be useful to support
	// SOCKET_OPTION_BROADCAST - socket can broadcast messages (if supported by network)
	// SOCKET_OPTION_LINGER - wait on close to make sure all data is sent
	// SOCKET_OPTION_NO_DELAY - disable nagle's algorithm
};
typedef uint eSocketOptions; 

//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
// Rip out things from TCPSocket and put it to a base leve; 
class Socket 
{
public:
	Socket(); 
	virtual ~Socket(); // closes

	bool SetBlocking( bool isBlocking ); 

	void Close(); // closesocket and set m_handle = INVALID_SOCKET
	bool IsClosed() const; // m_handle == (socket_t)INVALID_SOCKET; 

	NetAddress	GetAddress() const { return m_address; }
	Socket_t	GetHandle() const { return m_handle; }

protected:


public:
	NetAddress			m_address; // address assocated with this socket; 
	Socket_t			m_handle; // initialized to INVALID_SOCKET

	// used if you want to set options while closed, 
	// used to keep track so you can apply_options() after the socket
	// gets created; 
	eSocketOptions		m_options; 

	bool				m_isBlocking;
	bool				m_isRunning = true;
};


//====================================================================================
// Standalone C Functions
//====================================================================================

// fatal errors cause the socket to close; 
bool HasFatalError(); 

//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [9/10/2018]
//====================================================================================





