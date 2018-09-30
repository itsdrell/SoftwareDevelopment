#pragma once
#include "Engine\Core\Tools\Command.hpp"

//====================================================================================
// Forward Declare
//====================================================================================
class UnitDefinition;

//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================


//====================================================================================
// Standalone C Functions
//====================================================================================

void RegisterGameCommands();

void EndTurn(Command& theCommand);
void HaveAUnitWait(Command& theCommand);
void CaptureBuilding(Command& theCommand);
void AddUnit(Command& theCommand);
void AddBuilding(Command& theCommand);
void AddEffect(Command& theCommand);
void CloseOpenMenu(Command& theCommand);
void PurchaseUnit(Command& theCommand);
void AddAllUnitTypesToMap(Command& theCommand);
void UseCOPower(Command& theCommand);
void DebugGrid(Command& theCommand);
void KillAllUnitsOfTeam(Command& theCommand);

// net session stuff
void AddConnection( Command& theCommand );
void SendPing( Command& theCommand );
void SendAdd( Command& theCommand );

//====================================================================================
// Externs
//====================================================================================
extern UnitDefinition* g_unitToSpawn;


//====================================================================================
// Written by Zachary Bracken : [6/25/2018]
//====================================================================================