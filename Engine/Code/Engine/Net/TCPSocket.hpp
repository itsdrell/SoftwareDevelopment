#pragma once
#include "..\Core\General\EngineCommon.hpp"
#include "NetAddress.hpp"

//====================================================================================
// Forward Declare
//====================================================================================


//====================================================================================
// Type Defs + Defines
//====================================================================================

// this is so that we don't have to include the header 
typedef void* Socket_t;

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
	TCPSocket(); 
	TCPSocket( Socket_t& theSocket );
	~TCPSocket(); 

	// - - - - - -
	// Options
	// - - - - - -
	// ...todo

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
	size_t		Send( const char* data, size_t const dataByteSize ); 
	
	// returns how much received
	size_t		Receive( void *buffer, size_t const maxByteSize ); 

	// - - - - - -
	// HELPERS
	// - - - - - -
	bool		IsClosed() const; 
	bool		IsListening() const { return m_isRunning; }

public:
	Socket_t		m_handle; 

	// if you're a listening, the address is YOUR address
	// if you are connecting (or socket is from an accept)
	// this address is THEIR address;  (do not listen AND connect on the same socket)
	NetAddress		m_address; 

	bool			m_isRunning;

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