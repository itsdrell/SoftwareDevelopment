#pragma once
#include "Engine\Core\Tools\Command.hpp"


//====================================================================================
// Standalone C Functions
//====================================================================================
void RegisterEngineCommands();

//--------------------------------------------------------------------------
// GENERAL
void ShowAllRegisteredCommands(Command& thecommand);
void ClearConsole(Command& thecommand);
void SaveConsoleToFile(Command& thecommand);
void SetAppTitle(Command& thecommand);
void RunScript(Command& thecommand);


//--------------------------------------------------------------------------
// NETWORKING
void GetAddressName( Command& cb );
void TestConnect( Command& cb );
void TestHost( Command& cb );
void Connect( Command& cb ); // better one

void AddSimulateLag( Command& cb );
void AddSimulatedLoss( Command& cb );
void SetHeartbeatRate( Command& cb );
void SetSessionSendRate( Command& cb );
void SetConnectionSendRate( Command& cb );

void SpawnProcess( Command& cb );

//-----------------------------------------------------------------------------------------------
// Remote Command Service
void RCSSendMessage( Command& cb );
void RCSSendMessageToAll( Command& cb);
void RCSSendMessageToAllButMe( Command& cb);
void RCSJoin( Command& cb);
void RCSHost( Command& cb);
void RCSToggleEcho( Command& cb);


//-----------------------------------------------------------------------------------------------
// test
void StopUDPTest(Command& cb);
void SendMessageUDP(Command& cb);


//-----------------------------------------------------------------------------------------------
// Tools
void FormatLevelNames(Command& cb);

//====================================================================================
// Written by Zachary Bracken : [8/20/2018]
//====================================================================================