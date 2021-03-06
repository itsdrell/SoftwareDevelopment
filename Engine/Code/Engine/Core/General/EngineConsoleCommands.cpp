#include "EngineConsoleCommands.hpp"
#include "Engine/Internal/WindowsCommon.h"
#include "Engine/Core/Tools/LogSystem.hpp"
#include "Engine/Core/Utils/StringUtils.hpp"
#include "Engine/Core/Tools/DevConsole.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Platform/Window.hpp"
#include "../../Async/Threading.hpp"
#include "../../Net/Net.hpp"
#include "../../Net/TCPSocket.hpp"
#include "../Tools/RemoteCommandService.hpp"
#include "../../Net/UDPSocket.hpp"
#include "../Platform/File.hpp"
#include "../../Net/NetSession.hpp"
#include "../../Math/MathUtils.hpp"

//====================================================================================
void RegisterEngineCommands()
{
	// Just some default Commands 
	CommandRegister("help","Type: help","Shows all commands", ShowAllRegisteredCommands);
	CommandRegister("clear","Type: clear", "Clears the Screen", ClearConsole);
	CommandRegister("saveLog","Type: save_log <path or none>", "Saves Command Log to File", SaveConsoleToFile);
	CommandRegister("setTitle","Type: setTitle <title can use spaces even>", "Sets the apps title message", SetAppTitle);
	CommandRegister("runScript","Type: runScript <Script file name, not full path>", "Runs script from file", RunScript);
	//CommandRegister("echo_with_color","Type: echo_with_color r,g,b,a ''string''", "Prints a colored string", PrintStringWithAColor);
	
	CommandRegister("spawnProcess", "", "Spawns a new process", SpawnProcess);
	
	// Network
	CommandRegister("getAddresName", "", "Get IP Address", GetAddressName);
	CommandRegister("net_sim_loss", "", "Loss range 0-100", AddSimulatedLoss);
	CommandRegister("net_sim_lag", "", "Set min and max lag", AddSimulateLag);
	CommandRegister("net_set_heart_rate", "", "Set heartbeat", SetHeartbeatRate);
	CommandRegister("net_set_session_send_rate", "", "Set Session send rate", SetSessionSendRate);
	CommandRegister("net_set_connection_send_rate", "", "Set Connection send rate", SetConnectionSendRate);

	// remote command
	CommandRegister("rc", "", "idx message", RCSSendMessage);
	CommandRegister("rca", "", "command", RCSSendMessageToAll);
	CommandRegister("rcb", "", "command", RCSSendMessageToAllButMe);
	CommandRegister("rcJoin", "", "address:port", RCSJoin);
	CommandRegister("rcHost", "", "port", RCSHost);
	CommandRegister("rcEcho", "", "bool", RCSToggleEcho);

	// test
	CommandRegister("test_stop", "", "", StopUDPTest);
	CommandRegister("send", "", "<address> <message>", SendMessageUDP);

	// tools
	CommandRegister("warp", "", "", FormatLevelNames);

}

//--------------------------------------------------------------------------
void GetAddressName(Command & cb)
{
	UNUSED(cb);
	
	char my_name[256];
	::gethostname( my_name, 256);
	
	// service is like "http" or "ftp", which translates to a port (80 or 21).  We'll just use port 80 for this example;
	char const* service = "80";

	// no host name - can't resolve; 
	if(StringIsNullOrEmpty(my_name))
	{
		return;
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
		return;
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
			
			
			DevConsole::AddConsoleDialogue(Stringf("My Address: %s", out ));
		}
		
		iter = iter->ai_next; 
	}

	// freeing up
	::freeaddrinfo( result ); 


}

//--------------------------------------------------------------------------
void TestConnect(Command& cb)
{
	String addr_str = cb.GetNextString();
	String ip;
	String port;

	String msg = cb.GetNextString();

	if(StringIsNullOrEmpty(addr_str.c_str()) || StringIsNullOrEmpty(msg.c_str()))
	{
		DevConsole::AddErrorMessage("Invalid input. Missing data");
		return;
	}

	Strings addres = SplitString(addr_str, ":");
	ip = addres[0];
	port = addres[1];

	sockaddr_storage saddr;

	int addrlen;
	if( !GetAddressForHost( (sockaddr*)&saddr, &addrlen, ip.c_str(), port.c_str()))
	{
		DevConsole::AddErrorMessage("Could not resolve");
		return;
	}


	SOCKET sock = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if( sock == INVALID_SOCKET)
	{
		DevConsole::AddErrorMessage("Could not create socket");
		return;
	}

	int result = ::connect( sock, (sockaddr*)&saddr, (int)addrlen);
	if(result == SOCKET_ERROR)
	{
		DevConsole::AddErrorMessage("Could not connect");
		::closesocket(sock);
		return;
	}

	DevConsole::AddConsoleDialogue( "Connected." ); 

	// you send raw bytes over the network in whatever format you want; 
	char payload[256]; 
	String s = msg;
	::send( sock, msg.c_str(), (int) s.length(), 0 ); 

	// with TCP/IP, data sent together is not guaranteed to arrive together.  
	// so make sure you check the return value.  This will return SOCKET_ERROR
	// if the host disconnected, or if we're non-blocking and no data is there. 
	size_t recvd = ::recv( sock, payload, 256 - 1U, 0 ); 

	// it may not be null terminated and I'm printing it, so just for safety. 
	payload[recvd] = NULL; 
	DevConsole::AddConsoleDialogue( Stringf("Received: %s", payload )); 

	// cleanup
	::closesocket(sock); 



}

//-----------------------------------------------------------------------------------------------
void AddSimulateLag(Command & cb)
{
	int min = 0;
	int max = 0;

	if(IsIndexValid(1, cb.m_commandArguements))
		min = atoi(cb.GetNextString().c_str());

	if(IsIndexValid(2, cb.m_commandArguements))
		max = atoi(cb.GetNextString().c_str());

	NetSession::GetInstance()->SetSimulatedLatency(min,max);

	DevConsole::AddConsoleDialogue(Stringf("Added a min %i and max %i latency to packet", min, max));

}

//-----------------------------------------------------------------------------------------------
void AddSimulatedLoss(Command & cb)
{
	float lossAmount = 0.f;

	if(IsIndexValid(1, cb.m_commandArguements))
		lossAmount = (float) atoi(cb.GetNextString().c_str());

	lossAmount = ClampFloat(lossAmount, 0.f, 100.f);

	NetSession::GetInstance()->SetSimulateLoss(lossAmount);

	DevConsole::AddConsoleDialogue(Stringf("Added %f amount of loss (make sure it's 0-100)", lossAmount));
}

//-----------------------------------------------------------------------------------------------
void SetHeartbeatRate(Command& cb)
{
	float heartbeatRate = 1.f; // in hz

	if(IsIndexValid(1, cb.m_commandArguements))
		heartbeatRate = (float) atof(cb.GetNextString().c_str());

	heartbeatRate = ClampFloat(heartbeatRate, 0.f, 100000.f);

	NetSession::GetInstance()->SetHeartbeat(heartbeatRate);

	DevConsole::AddConsoleDialogue(Stringf("Set heartbeat to: %f ", heartbeatRate));
}

//-----------------------------------------------------------------------------------------------
void SetSessionSendRate(Command & cb)
{
	float flushRate = 0.f; // in hz

	if(IsIndexValid(1, cb.m_commandArguements))
		flushRate = (float) atof(cb.GetNextString().c_str());

	flushRate = ClampFloat(flushRate, 0.f, 100000.f);

	NetSession::GetInstance()->SetSessionFlushRate(flushRate);

	DevConsole::AddConsoleDialogue(Stringf("Set Session flush rate to: %f ", flushRate));
}

//-----------------------------------------------------------------------------------------------
void SetConnectionSendRate(Command & cb)
{
	int idx = 0;
	float flushRate = 0.f; // in hz

	if(IsIndexValid(1, cb.m_commandArguements))
		idx = atoi(cb.GetNextString().c_str());

	if(IsIndexValid(2, cb.m_commandArguements))
		flushRate = (float) atof(cb.GetNextString().c_str());

	flushRate = ClampFloat(flushRate, 0.f, 100000.f);
	idx = ClampInt(idx, 0, NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS);

	NetSession::GetInstance()->SetSessionFlushRate(flushRate);

	DevConsole::AddConsoleDialogue(Stringf("Set Connection %i flush rate to: %f ", idx, flushRate));
}

//-----------------------------------------------------------------------------------------------
void SpawnProcess(Command& cb)
{
	
	int amountToSpawn = atoi(cb.GetNextString().c_str());
	
	// default to always one
	if(amountToSpawn == 0)
		amountToSpawn = 1;

	// Do the spawn with ~~black magic~~
	for(uint i = 0; i < (uint) amountToSpawn; i++)
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory( &pi, sizeof(pi) );

		wchar_t buffer[MAX_PATH]; 
		GetModuleFileName(NULL, buffer, MAX_PATH) ;
		CreateProcess(buffer, 0, 0, FALSE, 0, 0, 0, 0, &si, &pi);
	}

}

//-----------------------------------------------------------------------------------------------
void RCSSendMessage(Command& cb)
{
	std::string idx = cb.GetNextString();
	//std::string echo = cb.GetNextString();
	std::string message = cb.GetRestOfCommand();

	uint theIdx = (uint) atoi(idx.c_str());
	//bool isEcho = false;
	//
	//if(echo == "true")
	//	isEcho = true;

	SendAMessage(theIdx, false, message.c_str());

}

//-----------------------------------------------------------------------------------------------
void RCSSendMessageToAll(Command & cb)
{
	std::string message = cb.GetCommandParameters();

	uint sizeOfConnesctions = (uint) RemoteCommandService::GetInstance()->m_connections.size();
	for(uint i = 0; i < sizeOfConnesctions; i++)
	{
		SendAMessage(i, false, message.c_str());
	}

	// do it on me
	CommandRunScript(message.c_str());
}

//-----------------------------------------------------------------------------------------------
void RCSSendMessageToAllButMe(Command & cb)
{
	std::string message = cb.GetCommandParameters();

	uint sizeOfConnesctions = (uint) RemoteCommandService::GetInstance()->m_connections.size();
	for(uint i = 0; i < sizeOfConnesctions; i++)
	{
		SendAMessage(i, false, message.c_str());
	}
}

//-----------------------------------------------------------------------------------------------
void RCSJoin(Command & cb)
{
	String address = cb.GetNextString();

	RemoteCommandService* rcs = RemoteCommandService::GetInstance();

	rcs->m_tryToJoinAddress = address;
	rcs->m_currentState = REMOTE_COMMAND_TRY_TO_JOIN;

	DevConsole::AddConsoleDialogue("Trying to join at: " + address);
}

//-----------------------------------------------------------------------------------------------
void RCSHost(Command & cb)
{
	RemoteCommandService* rcs = RemoteCommandService::GetInstance();
	
	String port = REMOTE_COMMAND_PORT;
	if(cb.m_commandArguements.size() > 1)
		port = cb.GetNextString();

	rcs->m_hostWithPortAddress = port;
	rcs->m_currentState = REMOTE_COMMAND_TRY_TO_HOST;

	DevConsole::AddConsoleDialogue("Trying to host at: " + port);
}

//-----------------------------------------------------------------------------------------------
void RCSToggleEcho(Command & cb)
{
	// see if we set the echo value or just do a toggle
	if(cb.m_commandArguements.size() > 1)
	{
		// get the value we wanna set
		String value = cb.GetNextString();
		
		bool flag = false;
		if(value == "true")
			flag = true;

		RemoteCommandService::GetInstance()->m_processingEchos = flag;
	}
	else
	{
		RemoteCommandService::GetInstance()->m_processingEchos = !RemoteCommandService::GetInstance()->m_processingEchos;
	}
}

//===============================================================================================
// delete these

void StopUDPTest(Command & cb)
{
	UNUSED(cb);
	UDPTest::GetInstance()->Stop();
}

void SendMessageUDP(Command & cb)
{
	NetAddress addr(cb.GetNextString().c_str());
	//String str = cb.GetNextString().c_str();

	//if( !NetAddress::GetBindableAddress(&addr, str))
	//{
	//	DevConsole::AddErrorMessage("Requires address");
	//	return;
	//}

	String msg = cb.GetRestOfCommand();
	if(StringIsNullOrEmpty(msg.c_str()))
	{
		DevConsole::AddErrorMessage("Requires a message");
		return;
	}

	DevConsole::AddConsoleDialogue("Sending message");
	UDPTest::GetInstance()->SendTo(addr, msg.data(), (uint) msg.length());
}

//-----------------------------------------------------------------------------------------------
// This is for my capstone team. Formatting all the levels into one string for the batch file
void FormatLevelNames(Command& cb)
{
	UNUSED(cb);

	String prefix = "/Game/Levels/Production/";
	String uiPrefix = "/Game/Levels/Menus/";
	String fullText = (
		uiPrefix + "PLVL_MainMenu" + "+" + 
		uiPrefix + "PLVL_LevelSelect" + "+" + 
		uiPrefix + "PLVL_Credits" + "+");

	tinyxml2::XMLDocument doc;
	doc.LoadFile( "Data/Misc/LevelsApprovedForBuild - Levels.xml" );

	tinyxml2::XMLElement* rootElement = doc.RootElement();
	GUARANTEE_OR_DIE(rootElement != nullptr, "Could not read: Level approved doc");

	tinyxml2::XMLElement* indexElement = rootElement->FirstChildElement();
	while( indexElement )
	{
		String levelName = indexElement->Name();
		fullText += (prefix + levelName + "/PLVL_" + levelName);

		if(indexElement->NextSiblingElement() != nullptr)
			fullText += "+";

		indexElement = indexElement->NextSiblingElement();
	}


	LogStringToFile("Data/Misc/AllLevels.txt", fullText.c_str(), true);

}

//===============================================================================================

//-----------------------------------------------------------------------------------------------
void TestHost(Command& cb)
{
	UNUSED(cb);
	ThreadCreateAndDetach((thread_cb) HostExample, "12345");
}

//-----------------------------------------------------------------------------------------------
void Connect(Command& cb)
{
	
	String addr_str = cb.GetNextString();
	String msg = cb.GetRestOfCommand();

	if(StringIsNullOrEmpty(addr_str.c_str()) || StringIsNullOrEmpty(msg.c_str()))
	{
		DevConsole::AddErrorMessage("Invalid input. Missing data");
		return;
	}

	NetAddress theAddress = NetAddress(addr_str.c_str());


	TCPSocket socket; // defaults to blocking 
	if (socket.Connect( theAddress )) 
	{
		DevConsole::AddConsoleDialogue( "Connected." ); 
		socket.Send( msg.c_str(), msg.size() + 1 ); 

		char payload[256]; 
		size_t recvd = socket.Receive( payload, 256 ); 
		payload[recvd] = NULL;
		DevConsole::AddConsoleDialogue( Stringf("Received: %s", payload) ); 

		socket.Close(); 
	} 
	else 
	{
		DevConsole::AddErrorMessage( "Could not connect." );
	}

}

//--------------------------------------------------------------------------
void ShowAllRegisteredCommands(Command& thecommand)
{	
	UNUSED(thecommand);
	
	// Get all the names to print
	Strings whatToPrint = CommandRegistration::GetAllCommandsAndTheirHelp();

	// Add them to our display
	for(int i = 0; i < (int)whatToPrint.size(); i++)
	{

		ConsoleDialogue newDialogue;

		// Creates a white header and then rainbow after that!
		if(i == 0)
			newDialogue = ConsoleDialogue(whatToPrint.at(i),Rgba::WHITE);
		else
			newDialogue = ConsoleDialogue(whatToPrint.at(i),GetRainbowColor(i - 1, (int)whatToPrint.size() - 1));

		// Add them to the display
		DevConsole::AddConsoleDialogue(newDialogue);
	}

	PlayOneShot("iAmHere");
	DevConsole::GetInstance()->m_dekuTimer->SetTimer(2.f);
}

//--------------------------------------------------------------------------
void ClearConsole(Command& thecommand)
{
	UNUSED(thecommand);
	DevConsole::ClearConsoleOutput();

}

//--------------------------------------------------------------------------
void SaveConsoleToFile(Command& thecommand)
{
	std::string path;

	// they entered a path
	if(thecommand.m_commandArguements.size() > 1)
		path = thecommand.m_commandArguements[1];
	else
		path = "Log/Console.log";

	std::vector<ConsoleDialogue> history(DevConsole::GetHistory());

	std::ofstream outputFile;

	// try to open file + clear anything inside it
	outputFile.open(path.c_str(), std::fstream::trunc);

	// See if we opened it
	if(outputFile.is_open() == false)
	{
		DevConsole::AddErrorMessage("Could not Save!");
		return;
	}

	// if we can, start printing
	for(int i = 0; i < (int)history.size(); i++)
	{
		outputFile << history.at(i).m_text;
		outputFile << "\n";
	}

	// Let user know and close the file
	DevConsole::AddConsoleDialogue(ConsoleDialogue("Saved Successfully! :D ", Rgba::GREEN));
	outputFile.close();
}

// Command input is: name rgbaColor string
// void PrintStringWithAColor(Command& thecommand)
// {
// 	// First make a color (uses argument 1)
// 	
// 	Rgba colorToShow = ParseString(thecommand.m_commandArguements.at(1).c_str(), Rgba::WHITE);
// 
// 	// Then make the string
// 	const char* whatToDelete = "\"";
// 
// 	std::string stringToShow = RemoveCharacterFromString(thecommand.m_commandArguements.at(2), whatToDelete);
// 
// 	// Create output
// 	ConsoleDialogue newDialogue = ConsoleDialogue(stringToShow,colorToShow);
// 
// 	// Add them to the display
// 	DevConsole::AddConsoleDialogue(newDialogue);
// }

//--------------------------------------------------------------------------
void SetAppTitle(Command& thecommand)
{
	std::string newTitle;

	for(int i = 1; i < thecommand.m_commandArguements.size(); i++)
	{
		newTitle.append(thecommand.m_commandArguements.at(i) + " ");
	}

	Window::GetInstance()->SetTitle(newTitle.c_str());
}

//--------------------------------------------------------------------------
void RunScript(Command& thecommand)
{
	CommandRunScriptFromFile(thecommand.m_commandArguements.at(1).c_str());
}