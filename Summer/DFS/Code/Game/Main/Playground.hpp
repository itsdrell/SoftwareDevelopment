#pragma once
#include "Engine\Net\NetMessage.hpp"
#include "Engine\Net\NetSession.hpp"

//====================================================================================
// Forward Declare
//====================================================================================

//====================================================================================
// NameSpace 
//====================================================================================
namespace Playground // just doing a name space to learn about em could just be c func
{
	// happens during startup
	void RunTestOnce();

	// if we want to just let it happen a couple of times
	void RunTestOnUpdate();

}


//===============================================================================================
// Throw away C functions
//===============================================================================================
bool OnPing( NetMessage& msg, const NetSender& from);
bool OnPong( NetMessage& msg, const NetSender& from);
bool OnAdd( NetMessage& msg, const NetSender& from);

//====================================================================================
// Written by Zachary Bracken : [9/15/2018]
//====================================================================================