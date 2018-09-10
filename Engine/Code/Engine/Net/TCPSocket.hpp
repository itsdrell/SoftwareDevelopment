#pragma once
#include "..\Core\General\EngineCommon.hpp"
#include "NetAddress.hpp"
#include "Engine/Net/Socket.hpp"

//====================================================================================
// Forward Declare
//====================================================================================


//====================================================================================
// Type Defs + Defines
//====================================================================================

// this is so that we don't have to include the header 

//typedef SOCKET Socket_t;

//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class TCPSocket
{
public:
	// 
	TCPSocket(); 
	TCPSocket( Socket_t& theSocket );
	TCPSocket( const char* netAddressString);
	TCPSocket( const Socket_t& theSocket, const NetAddress& theAddress);
	~TCPSocket(); 

	// - - - - - -
	// Options
	// - - - - - -
	// ...todo
	bool SetBlockType(bool isBlocking);
	static bool HasFatalError();

	// - - - - - -
	// STARTING/STOPPING
	// - - - - - -
	bool		Listen( String port, uint maxQueued ); 
	TCPSocket*	Accept(); 

	// for joining
	bool		Connect( NetAddress const &addr ); 

	// when finished
	void		Close(); 

	// - - - - - -
	// TRAFFIC
	// - - - - - -
	// returns how much sent
	size_t		Send( const void* data, size_t const dataByteSize ); 
	
	// returns how much received
	size_t		Receive( void *buffer, size_t const maxByteSize ); 

	// - - - - - -
	// HELPERS
	// - - - - - -
	bool		IsClosed() const; 
	bool		IsListening() const { return m_isRunning; }

	
	template <typename T>
	size_t Receive( T* out )
	{
		return receive( out, sizeof(T));
	}

	//-----------------------------------------------------------------------------------------------
public:
	Socket_t		m_handle; 

	// if you're a listening, the address is YOUR address
	// if you are connecting (or socket is from an accept)
	// this address is THEIR address;  (do not listen AND connect on the same socket)
	NetAddress		m_address; 

	bool			m_isRunning;
	bool			m_isBlocking = true;

};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [8/23/2018]
//====================================================================================