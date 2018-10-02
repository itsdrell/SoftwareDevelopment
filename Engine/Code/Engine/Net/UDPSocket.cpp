#include "UDPSocket.hpp"
#include "Engine/Internal/WindowsCommon.h"
#include "../Core/Tools/DevConsole.hpp"
#include "../Core/Utils/StringUtils.hpp"
#include "../Math/MathUtils.hpp"

UDPTest* UDPTest::s_instance = nullptr;
//===============================================================================================
bool UDPSocket::Bind( NetAddress &addr, uint16_t port_range /*= 0U*/ )
{
	// create the socket 
	SOCKET my_socket = socket( AF_INET,	// IPv4 to send...
		SOCK_DGRAM,							// ...Datagrams... 
		IPPROTO_UDP );						// ...using UDP.

	GUARANTEE_OR_DIE( my_socket != INVALID_SOCKET, false ); 

	// so for range, you start at an index and increase by the range (so 10 with range 4 is 10, 11, 12, 13)
	// and do that till you connect
	// Shown - just trying one; 

	for(uint i = 0; i < port_range; i++)
	{
		sockaddr_storage sock_addr;
		size_t sock_addr_len;
		
		//NetAddressToSocketAddress( (sockaddr*)&sock_addr, &sock_addr_len, addr );
		addr.ToSocketAddress( (sockaddr*)&sock_addr, &sock_addr_len );

		// try to bind - if it succeeds - great.  If not, try the next port in the range.
		int result = ::bind( my_socket, (sockaddr*)&sock_addr, (int)sock_addr_len );
		if (0 == result) 
		{
			m_handle = (Socket_t)my_socket; 
			m_address = addr; 
			return true; 
		} 
		else
		{
			addr.m_port++;
		}
	}

	return false; 
}

size_t UDPSocket::SendTo(NetAddress const & addr, void const * data, size_t const byte_count)
{
	if(IsClosed())
		return 0U;
	
	sockaddr_storage saddr;
	size_t addr_len;
	addr.ToSocketAddress((sockaddr*)&saddr, &addr_len);

	SOCKET sock = (SOCKET) m_handle;
	int sent = ::sendto(sock, 
		(const char*) data,
		(int) byte_count, 
		0, 
		(sockaddr*) &saddr, 
		addr_len);

	if(sent > 0)
	{
		return (size_t) sent;
	}
	else
	{
		if(HasFatalError())
			Close();
		else
			return 0U;
	}

	return 1U;
}

size_t UDPSocket::ReceiveFrom(NetAddress * out_addr, void * buffer, size_t const max_read_size)
{
	if(m_isRunning == false)
		return 0U;

	sockaddr_storage fromaddr; // who am I getting data from
	size_t addr_len = sizeof(sockaddr_storage);
	SOCKET sock = (SOCKET) m_handle;

	int recvd = ::recvfrom( sock,
		(char*) buffer,
		(int) max_read_size,
		0,
		(sockaddr*) &fromaddr,
		(int*) &addr_len);

	if(recvd > 0)
	{
		out_addr->FromSocketAddress((sockaddr*)&fromaddr);
		return recvd;
	}
	else
	{
		if(HasFatalError())
			Close();
		
		return 0U;
	}
}


//===============================================================================================
// pls delete
//===============================================================================================


UDPTest::UDPTest()
{

}

UDPTest* UDPTest::GetInstance()
{
	if(s_instance == nullptr)
		s_instance = new UDPTest();
	else
		return s_instance;
}

bool UDPTest::Start()
{
	// get an address to use; 
	//NetAddress addr;
	//bool check = NetAddress::GetBindableAddress( &addr, GAME_PORT );  
	//if (check == 0U) 
	//{
	//	DevConsole::AddConsoleDialogue( "No addresses" ); 
	//	return false;
	//}
	//
	//if (!m_socket.Bind( addr, 10 )) 
	//{
	//	DevConsole::AddConsoleDialogue( "Failed to bind." );
	//	return false;  
	//} else {
	//	m_socket.SetBlocking(false); // if you have cached options, you could do this
	//								  // in the constructor; 
	//	DevConsole::AddConsoleDialogue( Stringf("Socket bound: %s", m_socket.GetAddress().ToString().c_str() ));
		return true; 
	//}
}

void UDPTest::Update()
{
	
	//Byte buffer[PACKET_MTU]; 
	//
	//NetAddress from_addr; 
	//size_t read = m_socket.ReceiveFrom( &from_addr, buffer, PACKET_MTU ); 
	//
	//if (read > 0U) 
	//{
	//
	//	uint max_bytes = Min( (uint) read, 128U ); 
	//	String output = "0x"; 
	//	output.reserve(max_bytes * 2U + 3U);
	//
	//	char* iter = (char*) output.data(); 
	//	iter += 2U; // skip the 0x
	//	for (uint i = 0; i < read; ++i) 
	//	{
	//		sprintf_s( (char*) iter, 3U, "%02X", buffer[i] ); 
	//		iter += 2U; 
	//	}
	//	
	//	*iter = NULL; 
	//	//output.(); 
	//	
	//	DevConsole::AddConsoleDialogue( Stringf("Received from %s;%s", 
	//		from_addr.ToString().c_str(), 
	//		output.c_str() )); 
	//}
}
