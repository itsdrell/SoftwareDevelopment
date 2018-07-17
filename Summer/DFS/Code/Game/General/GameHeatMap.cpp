#include "GameHeatMap.hpp"
#include "Engine/Math/Vectors/IntVector2.hpp"
#include "Game/General/Tiles/Tile.hpp"
#include "Game/General/Map.hpp"
#include "Game/General/GameObjects/Unit.hpp"
#include "Game/General/Tiles/TileDefinition.hpp"
#include <deque>

void GameHeatMap::UpdateMap(IntVector2 initialSeed)
{
	std::deque<IntVector2> openSeeds;
	openSeeds.push_back(initialSeed);

	while((int)openSeeds.size() != 0)
	{
		IntVector2 currentSeed = openSeeds.at(0);

		// Check the 4 directions around the cell
		IntVector2 northSeed = currentSeed + IntVector2::NORTH;
		if(isValidCell(northSeed))
		{
			if(DoWeChangeValue(currentSeed,northSeed))
			{
				Tile* theTile = g_theCurrentMap->GetTile(northSeed);
				float heatStep = g_theCurrentMap->m_selectedUnit->GetCostForTileType(theTile->m_definition->m_name);
				
				SetHeat(northSeed,GetHeat(currentSeed) + heatStep);
				openSeeds.push_back(northSeed);
			}

		}

		IntVector2 southSeed = currentSeed + IntVector2::SOUTH;
		if(isValidCell(southSeed))
		{
			if(DoWeChangeValue(currentSeed,southSeed))
			{
				Tile* theTile = g_theCurrentMap->GetTile(southSeed);
				float heatStep = g_theCurrentMap->m_selectedUnit->GetCostForTileType(theTile->m_definition->m_name);
				
				SetHeat(southSeed,GetHeat(currentSeed) + heatStep);
				openSeeds.push_back(southSeed);
			}

		}

		IntVector2 westSeed = currentSeed + IntVector2::WEST;
		if(isValidCell(westSeed))
		{
			if(DoWeChangeValue(currentSeed,westSeed))
			{
				Tile* theTile = g_theCurrentMap->GetTile(westSeed);
				float heatStep = g_theCurrentMap->m_selectedUnit->GetCostForTileType(theTile->m_definition->m_name);
				
				SetHeat(westSeed,GetHeat(currentSeed) + heatStep);
				openSeeds.push_back(westSeed);

			}

		}

		IntVector2 eastSeed = currentSeed + IntVector2::EAST;
		if(isValidCell(eastSeed))
		{	
			if(DoWeChangeValue(currentSeed,eastSeed))
			{
				Tile* theTile = g_theCurrentMap->GetTile(eastSeed);
				float heatStep = g_theCurrentMap->m_selectedUnit->GetCostForTileType(theTile->m_definition->m_name);
				
				SetHeat(eastSeed,GetHeat(currentSeed) + heatStep);
				openSeeds.push_back(eastSeed);
			}
		}

		// Get ready for the next time
		openSeeds.pop_front(); // remove
	}
}
