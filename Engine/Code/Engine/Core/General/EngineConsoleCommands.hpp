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

void SpawnProcess( Command& cb );

//====================================================================================
// Written by Zachary Bracken : [8/20/2018]
//====================================================================================