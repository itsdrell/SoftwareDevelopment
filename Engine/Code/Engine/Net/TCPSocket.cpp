#include "TCPSocket.hpp"
#include "Engine\Internal\WindowsCommon.h"
#include "..\Core\Utils\StringUtils.hpp"
#include "..\Core\Tools\DevConsole.hpp"


//===============================================================================================
//	TCPSocket
//-----------------------------------------------------------------------------------------------
TCPSocket::TCPSocket()
{
	m_isRunning = true;

	m_handle = (Socket_t) ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	m_address = NetAddress();
}

//-----------------------------------------------------------------------------------------------
TCPSocket::TCPSocket(Socket_t& theSocket)
{
	m_isRunning = true;
	m_address = NetAddress();
	m_handle = theSocket;
}

//-----------------------------------------------------------------------------------------------
TCPSocket::~TCPSocket()
{
	::closesocket((SOCKET) m_handle); 
}

//-----------------------------------------------------------------------------------------------
bool TCPSocket::Listen(String port, uint maxQueued)
{
	// Bindable means - use AI_PASSIVE flag
	if (!NetAddress::GetBindableAddress( &m_address, port )) 
	{
		Close();
		return false; 
	}

	// Next, we bind it - which means we assign an address to it; 
	sockaddr_storage saddr;
	size_t addrlen; 
	m_address.ToSocketAddress( (sockaddr*)&saddr, &addrlen ); 

	int result = ::bind( (SOCKET) m_handle, (sockaddr*)&saddr, addrlen ); 
	if (result == SOCKET_ERROR) 
	{
		// failed to bind - if you want to know why, call WSAGetLastError()
		Close(); 
		return false; 
	}

	// we now have a bound socket - this means we can start listening on it; 
	// This allows the socket to queue up connections - like opening a call center.
	// The number passed is NOT the max number of connections - just the max number of people who can 
	// be waiting to connect at once (think of it has a call center with N people manning the phone, but many more 
	// people who calls may be forwarded to.  Once a call is forwarded, the person answering calls can now answer a new one)
	int max_queued = maxQueued;  // probably pick a number that is <= max number of players in your game. 
	result = ::listen( (SOCKET) m_handle, max_queued ); 
	if (result == SOCKET_ERROR) 
	{
		Close(); 
		return false; 
	}
	
	
	return true;
}

//-----------------------------------------------------------------------------------------------
TCPSocket* TCPSocket::Accept()
{
	// this will create a new socket
	// and fill in the address associated with that socket (who are you talking to?)
	sockaddr_storage their_addr; 
	int their_addrlen = sizeof(sockaddr_storage); // important 

	SOCKET theSocket = ::accept( (SOCKET) m_handle, (sockaddr*)&their_addr, &their_addrlen ); 
	TCPSocket* theirSocket = new TCPSocket((Socket_t&) theSocket); // emily said this is dumb, should pass address
	
	return theirSocket;
}

//-----------------------------------------------------------------------------------------------
bool TCPSocket::Connect(NetAddress const &addr)
{
	// sockaddr storage is a sockaddr struct that
	// is large enough to fit any other sockaddr struct
	// sizeof(sockaddr_storage) >= sizeof(any other sockaddr)
	sockaddr_storage saddr;
	size_t addrlen; 

	addr.ToSocketAddress((sockaddr*)&saddr, &addrlen);
	
	// we then try to connect.  This will block until it either succeeds
	// or fails (which possibly requires a timeout).  We will talk about making
	// this non-blocking later so it doesn't hitch the game (more important 
	// when connecting to remote hosts)
	int result = ::connect( (SOCKET) m_handle, (sockaddr*)&saddr, (int)addrlen ); 
	if (result == SOCKET_ERROR) 
	{
		DevConsole::AddErrorMessage( "Could not connect" ); 
		Close();
		return false; 
	}

	DevConsole::AddConsoleDialogue( "Connected." ); 
	return true;
}

//-----------------------------------------------------------------------------------------------
void TCPSocket::Close()
{
	::closesocket((SOCKET) m_handle); 
	m_isRunning = false;
}

//-----------------------------------------------------------------------------------------------
size_t TCPSocket::Send(const char *data, size_t const dataByteSize)
{
	int sent = ::send( (SOCKET) m_handle, data, dataByteSize, MSG_OOB); // options are  MSG_DONTROUTE or MSG_OOB
	
	if (sent == SOCKET_ERROR) 
	{
		// there are non-fatal errors - but we'll go over them 
		// on Monday.  For now, you can assume any error with blocking
		// is a disconnect; 
		Close(); 
		return 0U; 
	} 
	else 
	{ 
		return sent;
	}
}

//-----------------------------------------------------------------------------------------------
size_t TCPSocket::Receive(void *buffer, size_t const maxByteSize)
{	
	size_t recvd = ::recv( (SOCKET) m_handle, 
		(char*) buffer,           // what we read into
		maxByteSize,         // max we can read
		0U );             // flags (unused)
	
	return recvd;
}

//-----------------------------------------------------------------------------------------------
bool TCPSocket::IsClosed() const
{
	return false;
}

