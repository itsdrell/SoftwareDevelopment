#pragma once
#include "Engine/Math/IntVector2.hpp"
#include <vector>

class Renderer;

class HeatMap
{
public:

	HeatMap(const IntVector2& dimensionOfMap, float valueToGive = 999.f);

	// This is for debugging
	void Render(Renderer& theRendererToUse) const;

	// Helper
	void SetHeat(const IntVector2& tileCoords, float amount = 0.f);
	void AddHeat(const IntVector2& tileCoords, float amount = 0.f);
	float GetHeat(const IntVector2& tileCoords);
	bool isValidCell(const IntVector2& cellToCheck);
	bool DoWeChangeValue(IntVector2 currentCell, IntVector2 otherCell);
	void UpdateMap(IntVector2 initialSeed);

public:

	std::vector<float>			m_heatValues;
	float						m_defaultValue;
	IntVector2					m_dimensions;
	int							m_stepSize;
};