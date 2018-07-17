#pragma once
#include "Engine/Core/General/HeatMap.hpp"

//====================================================================================
// Forward Declare
//====================================================================================


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
class GameHeatMap : public HeatMap
{
public:
	GameHeatMap(const IntVector2& tileCoords, float amount = 999)
		: HeatMap(tileCoords, amount) {}

	virtual void UpdateMap(IntVector2 initialSeed) override;

public:

};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [7/16/2018]
//====================================================================================