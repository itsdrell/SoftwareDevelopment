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
TCPSocket::TCPSocket(const char* netAddressString)
{
	m_isRunning = true;

	m_handle = (Socket_t) ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	m_address = NetAddress(netAddressString);
}

//-----------------------------------------------------------------------------------------------
TCPSocket::TCPSocket(const Socket_t& theSocket, const NetAddress& theAddress)
{
	m_isRunning = true;

	m_handle = theSocket;
	m_address = theAddress;
}

//-----------------------------------------------------------------------------------------------
TCPSocket::~TCPSocket()
{
	::closesocket((SOCKET) m_handle); 
}

//-----------------------------------------------------------------------------------------------
bool TCPSocket::SetBlockType(bool isBlocking)
{
	// we default to blocking
	u_long blocking;

	if(isBlocking)
		blocking = 0; // blocking
	else
		blocking = 1; // non blocking

	int result = ::ioctlsocket( (SOCKET) m_handle, FIONBIO, &blocking );

	if(result == 0)
	{
		m_isBlocking = isBlocking;
	}
	
	return (result == 0 ); 
	
}

//-----------------------------------------------------------------------------------------------
STATIC bool TCPSocket::HasFatalError()
{
	int theError = WSAGetLastError();

	if(theError == 0 || theError == WSAEWOULDBLOCK || theError == WSAEMSGSIZE )
		return false;

	return true;
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

	int result = (int) ::bind( (SOCKET) m_handle, (sockaddr*)&saddr, (int) addrlen ); 
	if (result == SOCKET_ERROR && HasFatalError()) 
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
	if (result == SOCKET_ERROR || HasFatalError()) 
	{
		Close(); 
		return false; 
	}
	
	SetBlockType(false);
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

	if(theSocket == INVALID_SOCKET)
	{
		//Close();
		return nullptr;
	}

	TCPSocket* theirSocket = new TCPSocket((Socket_t&) theSocket, NetAddress((sockaddr*) &their_addr)); 
	
	return theirSocket;
}

//-----------------------------------------------------------------------------------------------
bool TCPSocket::Connect(NetAddress const &addr)
{
	// this needs to be a blocking call
	SetBlockType(true);
	
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
	if (result == SOCKET_ERROR && HasFatalError()) 
	{
		DevConsole::AddErrorMessage( "Could not connect" ); 
		Close();
		SetBlockType(false);
		return false; 
	}

	DevConsole::AddConsoleDialogue( "Connected." ); 
	SetBlockType(false);
	return true;
}

//-----------------------------------------------------------------------------------------------
void TCPSocket::Close()
{
	::closesocket((SOCKET) m_handle); 
	m_handle = (void*) INVALID_SOCKET;
	m_isRunning = false;
}

//-----------------------------------------------------------------------------------------------
size_t TCPSocket::Send(const void*data, size_t const dataByteSize)
{
	int sent = (int) ::send( (SOCKET) m_handle, (char*) data, (int) dataByteSize, 0); // options are  MSG_DONTROUTE or MSG_OOB
	
	if (sent == SOCKET_ERROR && HasFatalError()) 
	{
		// there are non-fatal errors 
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
		(int) maxByteSize,         // max we can read
		0 );             // flags (unused)
	
	
	// If you get nothing, it is called a graceful close that we need to be aware of
	if(HasFatalError() || recvd == 0)
		Close();

	return recvd;
}

//-----------------------------------------------------------------------------------------------
bool TCPSocket::IsClosed() const
{
	return !m_isRunning;
}

