#include "NetAddress.hpp"
#include "Engine/Internal/WindowsCommon.h"
#include "Engine/Core/Tools/LogSystem.hpp"
#include "Engine/Core/Utils/StringUtils.hpp"
#include "../Core/Tools/DevConsole.hpp"

//====================================================================================
// Net Address
//-----------------------------------------------------------------------------------------------
NetAddress::NetAddress(sockaddr const * addr)
{
	bool check = FromSocketAddress(addr);
	
	if(check == false)
	{
		DevConsole::AddErrorMessage(Stringf("Could not created NetAddress at: %s", addr));
	}
}

//-----------------------------------------------------------------------------------------------
NetAddress::NetAddress(char const * string)
{
	String input = string;
	Strings split = SplitString(input, ":");

	sockaddr saddr;
	int addrelen;
	GetAddressForHost((sockaddr*)&saddr , &addrelen, split[0].c_str(), split[1].c_str());

	bool check = FromSocketAddress(&saddr);

	if(check == false)
	{
		DevConsole::AddErrorMessage(Stringf("Could not created NetAddress at: %s", string));
	}
}


//-----------------------------------------------------------------------------------------------
bool NetAddress::FromSocketAddress(const sockaddr* addr)
{
	if(addr->sa_family != AF_INET)
	{
		return false;
	}

	// if IPv6 - also check AF_INET6
	const sockaddr_in *ipv4 = ( const sockaddr_in* ) addr;

	uint ip = ipv4->sin_addr.S_un.S_addr;
	uint port = ::ntohs( ipv4->sin_port);

	m_address = ip;
	m_port = (uint16) port;
	return true;
}

//-----------------------------------------------------------------------------------------------
bool NetAddress::ToSocketAddress(sockaddr* addr, size_t* out_size) const
{
	*out_size = sizeof(sockaddr_in);

	sockaddr_in* ipv4 = (sockaddr_in*) addr;
	memset( ipv4, 0, sizeof(sockaddr_in) );

	ipv4->sin_family = AF_INET;
	ipv4->sin_addr.S_un.S_addr = m_address;
	ipv4->sin_port = ::htons(m_port);
	return true;
}

//-----------------------------------------------------------------------------------------------
String NetAddress::ToString() const
{
	uint8_t *array = (uint8_t*)&m_address; 
	String address = Stringf( "%u.%u.%u.%u:%u", 
		array[0], 
		array[1], 
		array[2], 
		array[3], 
		m_port ); 

	return address;
}

//-----------------------------------------------------------------------------------------------
String NetAddress::GetPortAsString() const
{
	String port = Stringf( "%u", m_port ); 

	return port;
}

//-----------------------------------------------------------------------------------------------
NetAddress NetAddress::GetLocalAddress(const char* port)
{
	char my_name[256];
	::gethostname( my_name, 256);

	// no host name - can't resolve; 
	if(StringIsNullOrEmpty(my_name))
	{
		DevConsole::AddErrorMessage("Error creating NetAddress for Local");
		return NetAddress();
	}

	// there is a lot of ways to communicate with our machine
	// and many addresses associated with it - so we need to 
	// provide a hint to the API to filter down to only the addresses we care about; 
	addrinfo hints;
	memset( &hints, 0, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	addrinfo* result;
	int status = getaddrinfo( my_name, port, &hints, &result);

	if(status != 0)
	{
		LogTaggedPrintf("net", "Failed to find addressed for [%s:%s]. Error[%s]", 
			my_name, port, ::gai_strerror(status));
		return NetAddress();
	}

	addrinfo* iter = result;
	while(iter != nullptr)
	{
		// you can farther filter here if you want, or return all of them and try them in order; 
		// for example, if you're using VPN, you'll get two unique addresses for yourself; 
		// if you're using AF_INET, the address is a sockaddr_in; 
		if (iter->ai_family == AF_INET) 
		{
			sockaddr_in* ipv4 = (sockaddr_in*)(iter->ai_addr); 
			// we have an address - print it!

			// if you look at the bytes - you can "see" the address, but backwards... we'll get to that
			// (port too)
			char out[256]; 
			inet_ntop( ipv4->sin_family, &(ipv4->sin_addr), out, 256 ); 

			NetAddress address = NetAddress((sockaddr*) ipv4);

			LogTaggedPrintf( "net", "My Address: %s", out ); 
			//DevConsole::AddConsoleDialogue(Stringf("My Address: %s", out ));

			::freeaddrinfo( result );
			return address;
		}

		iter = iter->ai_next; 
	}

	// freeing up
	::freeaddrinfo( result );
	return NetAddress();
}

//-----------------------------------------------------------------------------------------------
std::string NetAddress::GetLocalAddressAsString(const char* port)
{
	char my_name[256];
	::gethostname( my_name, 256);

	std::string theAddress = "ERROR";

	// no host name - can't resolve; 
	if(StringIsNullOrEmpty(my_name))
	{
		return theAddress;
	}

	// there is a lot of ways to communicate with our machine
	// and many addresses associated with it - so we need to 
	// provide a hint to the API to filter down to only the addresses we care about; 
	addrinfo hints;
	memset( &hints, 0, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	addrinfo* result;
	int status = getaddrinfo( my_name, port, &hints, &result);

	if(status != 0)
	{
		LogTaggedPrintf("net", "Failed to find addressed for [%s:%s]. Error[%s]", 
			my_name, port, ::gai_strerror(status));
		
		return theAddress;
	}

	addrinfo* iter = result;
	while(iter != nullptr)
	{
		// you can farther filter here if you want, or return all of them and try them in order; 
		// for example, if you're using VPN, you'll get two unique addresses for yourself; 
		// if you're using AF_INET, the address is a sockaddr_in; 
		if (iter->ai_family == AF_INET) 
		{
			sockaddr_in* ipv4 = (sockaddr_in*)(iter->ai_addr); 
			// we have an address - print it!

			// if you look at the bytes - you can "see" the address, but backwards... we'll get to that
			// (port too)
			char out[256]; 
			inet_ntop( ipv4->sin_family, &(ipv4->sin_addr), out, 256 ); 
			LogTaggedPrintf( "net", "My Address: %s", out ); 

			theAddress = std::string(out);
		}

		iter = iter->ai_next; 
	}

	// freeing up
	::freeaddrinfo( result ); 

	return theAddress + ":" + port;
}

//-----------------------------------------------------------------------------------------------
bool NetAddress::GetBindableAddress(NetAddress* outAddress, String port)
{
	char my_name[256];
	::gethostname( my_name, 256);

	// service is like "http" or "ftp", which translates to a port (80 or 21).  We'll just use port 80 for this example;
	char const* service = port.c_str();

	// no host name - can't resolve; 
	if(StringIsNullOrEmpty(my_name))
	{
		return false;
	}

	// there is a lot of ways to communicate with our machine
	// and many addresses associated with it - so we need to 
	// provide a hint to the API to filter down to only the addresses we care about; 
	addrinfo hints;
	memset( &hints, 0, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	addrinfo* result;
	int status = getaddrinfo( my_name, service, &hints, &result);

	if(status != 0)
	{
		LogTaggedPrintf("net", "Failed to find addressed for [%s:%s]. Error[%s]", 
			my_name, service, ::gai_strerror(status));
		return false;
	}

	addrinfo* iter = result;
	while(iter != nullptr)
	{
		// you can farther filter here if you want, or return all of them and try them in order; 
		// for example, if you're using VPN, you'll get two unique addresses for yourself; 
		// if you're using AF_INET, the address is a sockaddr_in; 
		if (iter->ai_family == AF_INET) 
		{
			sockaddr_in* ipv4 = (sockaddr_in*)(iter->ai_addr); 
			// we have an address - print it!

			// if you look at the bytes - you can "see" the address, but backwards... we'll get to that
			// (port too)
			char out[256]; 
			inet_ntop( ipv4->sin_family, &(ipv4->sin_addr), out, 256 ); 

			*outAddress = NetAddress((sockaddr*) ipv4);

			LogTaggedPrintf( "net", "My Address: %s", out ); 
			//DevConsole::AddConsoleDialogue(Stringf("My Address: %s", out ));

			::freeaddrinfo( result );
			return true;
		}

		iter = iter->ai_next; 
	}

	// freeing up
	::freeaddrinfo( result );
	return false;
}

//-----------------------------------------------------------------------------------------------
bool NetAddress::operator==(const NetAddress& compare) const
{
	return ((m_address == compare.m_address) && (m_port == compare.m_port));
}

//====================================================================================
// extern
//--------------------------------------------------------------------------
bool GetAddressForHost(sockaddr * out, int * out_addrlen, char const * hostname, char const * service)
{
	//no host name - can't resolve; 
	if(StringIsNullOrEmpty(hostname))
	{
		return false;
	}

	// there is a lot of ways to communicate with our machine
	// and many addresses associated with it - so we need to 
	// provide a hint to the API to filter down to only the addresses we care about; 
	addrinfo hints;
	memset( &hints, 0, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	//hints.ai_flags = AI_PASSIVE;

	addrinfo* result;
	int status = getaddrinfo( hostname, service, &hints, &result);

	if(status != 0)
	{
		LogTaggedPrintf("net", "Failed to find addressed for [%s:%s]. Error[%s]", 
			hostname, service, ::gai_strerror(status));
		return false;
	}

	bool foundOne = false;
	addrinfo* iter = result;
	while(iter != nullptr)
	{
		// you can farther filter here if you want, or return all of them and try them in order; 
		// for example, if you're using VPN, you'll get two unique addresses for yourself; 
		// if you're using AF_INET, the address is a sockaddr_in; 
		if (iter->ai_family == AF_INET) 
		{
			sockaddr_in* ipv4 = (sockaddr_in*)(iter->ai_addr); 

			memcpy( out, ipv4, sizeof(sockaddr_in));
			*out_addrlen = sizeof(sockaddr_in);
			foundOne = true;
			break;


		}
		iter = iter->ai_next; 
	}

	// freeing up
	::freeaddrinfo( result ); 

	return foundOne;
}
