#include "Neighborhood.hpp"
#include "Engine/Math/Vectors/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Main/GameCommon.hpp"
#include <algorithm>

//===============================================================================================
Neighborhood::Neighborhood(int radius)
{
	for (int yPos = -radius; yPos < radius + 1; yPos++)
	{
		for (int xPos = -radius; xPos < radius + 1; xPos++)
		{
			Vector2 worldPos = Vector2(xPos * CHUNK_SIZE_X, yPos * CHUNK_SIZE_Y);
			float distance = GetDistanceSquared(Vector2::ZERO, worldPos);
			if (distance < (CHUNK_ACTIVATION_DISTANCE * CHUNK_ACTIVATION_DISTANCE))
			{
				m_blockCoords.push_back(IntVector2(xPos, yPos));
			}
		}
	}

	std::sort(m_blockCoords.begin(), m_blockCoords.end(), SortBlockCoordsByDistanceFromOrigin);
}

//===============================================================================================
bool SortBlockCoordsByDistanceFromOrigin(IntVector2 a, IntVector2 b)
{
	Vector2 aWorldPos = Vector2(a.x * CHUNK_SIZE_X, a.y * CHUNK_SIZE_Y);
	Vector2 bWorldPos = Vector2(b.x * CHUNK_SIZE_X, b.y * CHUNK_SIZE_Y);

	float aDistance = GetDistanceSquared(Vector2::ZERO, aWorldPos);
	float bDistance = GetDistanceSquared(Vector2::ZERO, bWorldPos);

	return (aDistance < bDistance);
}
