
#include "Net.hpp"
#include "Engine/Internal/WindowsCommon.h"
#include "Engine/Core/General/EngineCommon.hpp"
#include "Engine/Core/Tools/ErrorWarningAssert.hpp"
#include "TCPSocket.hpp"
#include "../Core/Tools/DevConsole.hpp"
#include "../Core/Utils/StringUtils.hpp"

//====================================================================================
STATIC bool Net::Startup()
{
	WORD version = MAKEWORD( 2, 2 ); // pick the version

	WSADATA data;
	int32_t error = WSAStartup( version, &data );
	
	ASSERT_OR_DIE( error == 0, "Net Startup failed");
	return (error == 0);
}

//--------------------------------------------------------------------------
STATIC bool Net::Shutdown()
{
	::WSACleanup();
	return false;
}

//-----------------------------------------------------------------------------------------------
void HostExampleWinSock(void* port)
{
	NetAddress addr;

	//uint16_t port = 12345; 

	String theport = (char*) port;

	// Bindable means - use AI_PASSIVE flag
	if (!NetAddress::GetBindableAddress( &addr, theport )) 
	{
		return; 
	}

	// now we have an address, we can try to bind it; 
	// first, we create a socket like we did before; 
	SOCKET sock = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ); 

	// Next, we bind it - which means we assign an address to it; 
	sockaddr_storage saddr;
	size_t addrlen; 
	addr.ToSocketAddress( (sockaddr*)&saddr, &addrlen ); 

	int result = ::bind( sock, (sockaddr*)&saddr, addrlen ); 
	if (result == SOCKET_ERROR) 
	{
		// failed to bind - if you want to know why, call WSAGetLastError()
		::closesocket(sock); 
		return; 
	}

	// we now have a bound socket - this means we can start listening on it; 
	// This allows the socket to queue up connections - like opening a call center.
	// The number passed is NOT the max number of connections - just the max number of people who can 
	// be waiting to connect at once (think of it has a call center with N people manning the phone, but many more 
	// people who calls may be forwarded to.  Once a call is forwarded, the person answering calls can now answer a new one)
	int max_queued = 16;  // probably pick a number that is <= max number of players in your game. 
	result = ::listen( sock, max_queued ); 
	if (result == SOCKET_ERROR) 
	{
		::closesocket(sock); 
		return; 
	}

	Byte buffer[KILOBYTE];

	// now we can "accept" calls. 
	bool is_running = true; 
	while (is_running) 
	{
		// this will create a new socket
		// and fill in the address associated with that socket (who are you talking to?)
		sockaddr_storage their_addr; 
		int their_addrlen = sizeof(sockaddr_storage); // important 
		SOCKET their_sock = ::accept( sock, (sockaddr*)&their_addr, &their_addrlen ); 

		if (sock != INVALID_SOCKET) 
		{
			// great, we have a socket - we can now send and receive on it just as before
			// with this test - we'll always listen, then send, but 
			int recvd = ::recv( their_sock, 
				(char*)buffer,           // what we read into
				256 - 1U,         // max we can read
				0U );             // flags (unused)

			if (recvd != SOCKET_ERROR) 
			{
				buffer[recvd] = NULL; // just cause I'm printing it
				DevConsole::AddConsoleDialogue(Stringf( "Received: %s", buffer )); 

				// me sending a response, the word PONG which is 5 long (when you include the null terminator)
				::send( their_sock, "Ya did it", 20, 0U ); 
			}

			// clean up after you're done; 
			::closesocket(their_sock); 
		}

		// something should tell me to stop servicing requests eventually
		// is_running = false; 
	}

	// and I'm done; 
	::closesocket(sock); 
}

//-----------------------------------------------------------------------------------------------
void HostExample(void* port)
{
	int max_queued = 16;  // probably pick a number that is <= max number of players in your game. 
	
	String theport = (char*) port;

	TCPSocket host; 
	if (!host.Listen( theport, max_queued )) 
	{
		DevConsole::AddErrorMessage(Stringf("Cannot listen on port %u", theport ));

	} 

	while (host.IsListening()) 
	{
		TCPSocket* them = host.Accept();
		if (them != nullptr) 
		{
			Byte buffer[KILOBYTE]; 
			uint received = them->Receive( buffer, KILOBYTE - 1U ); 

			if (received > 0U) 
			{
				buffer[received] = '\0'; // forseth had this as nullptr for some reason
				DevConsole::AddConsoleDialogue(Stringf( "Received: %s", buffer )); 

				them->Send( "Ya did it", 20 ); 
			}

			delete them; 
		}
	}

	host.Close(); 
	// END NEW-- 
}
