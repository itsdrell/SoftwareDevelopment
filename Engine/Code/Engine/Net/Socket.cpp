#include "Socket.hpp"
#include <winsock2.h>

//-----------------------------------------------------------------------------------------------
bool HasFatalError()
{
	int theError = WSAGetLastError();

	if(theError == 0 || theError == WSAEWOULDBLOCK || theError == WSAEMSGSIZE )
		return false;

	return true;
}

//-----------------------------------------------------------------------------------------------
Socket::Socket()
{
}

//-----------------------------------------------------------------------------------------------
Socket::~Socket()
{
}

//-----------------------------------------------------------------------------------------------
bool Socket::SetBlocking(bool isBlocking)
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
void Socket::Close()
{
	::closesocket((SOCKET) m_handle); 
	m_handle = (void*) INVALID_SOCKET;
	m_isRunning = false;
}

//-----------------------------------------------------------------------------------------------
bool Socket::IsClosed() const
{
	return m_handle == (Socket_t)INVALID_SOCKET;
}

