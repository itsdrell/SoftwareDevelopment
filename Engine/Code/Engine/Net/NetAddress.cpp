#include "NetAddress.hpp"
#include "Engine/Internal/WindowsCommon.h"
#include "Engine/Core/Tools/LogSystem.hpp"
#include "Engine/Core/Utils/StringUtils.hpp"

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
