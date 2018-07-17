#pragma once
#include "Engine/Math/Vectors/IntVector2.hpp"
#include <vector>
#include "../../Math/Ranges/IntRange.hpp"

class Renderer;

//====================================================================================
// Defines / typedefs / consts expressions
//====================================================================================
constexpr float	BIG_NUMBER = 999.f;

//====================================================================================
//	Classes
//====================================================================================
class HeatMap
{
public:

	HeatMap(const IntVector2& dimensionOfMap, float valueToGive = BIG_NUMBER);

	// This is for debugging
	void Render(int cellSize) const;

	// Helper
	void SetHeat(const IntVector2& tileCoords, float amount = 0.f);
	void AddHeat(const IntVector2& tileCoords, float amount = 0.f);
	float GetHeat(const IntVector2& tileCoords);
	bool isValidCell(const IntVector2& cellToCheck);
	bool DoWeChangeValue(IntVector2 currentCell, IntVector2 otherCell);
	virtual void UpdateMap(IntVector2 initialSeed);

	void ResetHeatMap();


	std::vector<IntVector2>	GetAllTileCoordsWithHeatLessOrEqual(float heatValue);
	std::vector<IntVector2> GetAllTileCoordsWithHeatInRangeOf(const IntRange& range);

public:

	std::vector<float>			m_heatValues;
	float						m_defaultValue;
	IntVector2					m_dimensions;
	int							m_stepSize;
};