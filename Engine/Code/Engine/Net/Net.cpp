
#include "Net.hpp"
#include "Engine/Internal/WindowsCommon.h"
#include "Engine/Core/General/EngineCommon.hpp"
#include "Engine/Core/Tools/ErrorWarningAssert.hpp"

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
