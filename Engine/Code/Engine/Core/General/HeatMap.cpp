#include "HeatMap.hpp"
#include <deque>
#include "Engine/Math/Vectors/Vector2.hpp"
#include "Engine/Renderer/Renderer.hpp"

HeatMap::HeatMap(const IntVector2& dimensionOfMap, float valueToGive)
{
	m_dimensions = dimensionOfMap;
	m_defaultValue = valueToGive;
	m_stepSize = 1;

	int theDimesnions = m_dimensions.x * m_dimensions.y;
	std::vector<float> temp (theDimesnions,m_defaultValue); // creates theDimension amount of the values

	m_heatValues = temp;

}

void HeatMap::Render(int cellSize) const
{
	//Vector2 padding = Vector2(cellSize,cellSize);	
	IntVector2 currentCell = IntVector2(0,0);
	
	for(int cellIndex = 0; cellIndex < (int)m_heatValues.size(); cellIndex++)
	{
		int value = (int)m_heatValues.at(cellIndex);
		std::string stringValue = std::to_string(value);

		// Do the Draw
		Renderer::GetInstance()->DrawText2D(currentCell.GetAsVector2() * (float) cellSize, stringValue, 1.0f);

		// Move the position
		if(currentCell.x >= m_dimensions.x -1.f)
		{
			int yy = currentCell.y + 1;
			currentCell = IntVector2(0,yy);
		}
		else
		{
			currentCell += (IntVector2::EAST);
		}
	}
}

void HeatMap::SetHeat(const IntVector2& tileCoords, float amount)
{
	int location = tileCoords.x + m_dimensions.x * tileCoords.y;
	m_heatValues.at(location) = amount;
}

void HeatMap::AddHeat(const IntVector2& tileCoords, float amount)
{
	int location = tileCoords.x + m_dimensions.x * tileCoords.y;
	m_heatValues.at(location) = amount; // this might be plus equal

	UpdateMap((IntVector2)tileCoords);
}

float HeatMap::GetHeat(const IntVector2& tileCoords)
{
	if(isValidCell(tileCoords))
	{
		int location = tileCoords.x + m_dimensions.x * tileCoords.y;
		return m_heatValues.at(location);
	}
	
	return m_defaultValue;
}

bool HeatMap::isValidCell(const IntVector2& cellToCheck)
{
	if(cellToCheck.x < 0){return false;}
	if(cellToCheck.y < 0){return false;}

	// these may need to be >=
	if(cellToCheck.x >= m_dimensions.x){return false;}
	if(cellToCheck.y >= m_dimensions.y){return false;}

	return true;
}

bool HeatMap::DoWeChangeValue(IntVector2 currentCell, IntVector2 otherCell)
{
	int currentValue = (int)GetHeat(currentCell) + m_stepSize;
	int otherValue = (int)GetHeat(otherCell);

	if(currentValue < otherValue)
		return true;
	
	return false;
}

void HeatMap::UpdateMap(IntVector2 initialSeed)
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
				SetHeat(northSeed,GetHeat(currentSeed) + m_stepSize);
				openSeeds.push_back(northSeed);
			}
			
		}

		IntVector2 southSeed = currentSeed + IntVector2::SOUTH;
		if(isValidCell(southSeed))
		{
			if(DoWeChangeValue(currentSeed,southSeed))
			{
				SetHeat(southSeed,GetHeat(currentSeed) + m_stepSize);
				openSeeds.push_back(southSeed);
			}
			
		}

		IntVector2 westSeed = currentSeed + IntVector2::WEST;
		if(isValidCell(westSeed))
		{
			if(DoWeChangeValue(currentSeed,westSeed))
			{
				SetHeat(westSeed,GetHeat(currentSeed) + m_stepSize);
				openSeeds.push_back(westSeed);

			}
			
		}

		IntVector2 eastSeed = currentSeed + IntVector2::EAST;
		if(isValidCell(eastSeed))
		{	
			if(DoWeChangeValue(currentSeed,eastSeed))
			{
				SetHeat(eastSeed,GetHeat(currentSeed) + m_stepSize);
				openSeeds.push_back(eastSeed);
			}
		}

		// Get ready for the next time
		openSeeds.pop_front(); // remove
	}
}

void HeatMap::ResetHeatMap()
{
	for(uint i = 0; i < m_heatValues.size(); i++)
	{
		m_heatValues.at(i) = BIG_NUMBER;
	}
}

std::vector<IntVector2> HeatMap::GetAllTileCoordsWithHeatLessOrEqual(float heatValue)
{
	std::vector<IntVector2> tileCoords;
	
	for(uint i = 0; i < m_heatValues.size(); i++)
	{
		float value = m_heatValues.at(i);

		if(value <= heatValue)
		{
			int y = i / m_dimensions.y;
			int x = i % m_dimensions.y;
			
			tileCoords.push_back(IntVector2(x,y));
		}
	}

	return tileCoords;
}

std::vector<IntVector2> HeatMap::GetAllTileCoordsWithHeatInRangeOf(const IntRange& range)
{
	std::vector<IntVector2> tileCoords;

	for(uint i = 0; i < m_heatValues.size(); i++)
	{
		float value = m_heatValues.at(i);

		if(range.IsInRange((int) value))
		{
			int y = i / m_dimensions.y;
			int x = i % m_dimensions.y;

			tileCoords.push_back(IntVector2(x,y));
		}
	}

	return tileCoords;
}
