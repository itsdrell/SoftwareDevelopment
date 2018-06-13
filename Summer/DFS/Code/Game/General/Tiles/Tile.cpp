#include "Tile.hpp"

Tile::Tile()
{
	m_position = IntVector2::ZERO;
	m_definition = nullptr;
	m_tileSize = 16;
}

Tile::Tile(const IntVector2 & pos, TileDefinition & def, int tileCellSize )
{
	m_position = pos;
	m_definition = &def;
	m_tileSize = tileCellSize;
}

bool Tile::IsPointInsideTile(IntVector2& pos)
{
	int offset = (m_tileSize / 2);
	IntVector2 mins = pos - IntVector2(offset);
	IntVector2 maxs = pos + IntVector2(offset);
	
	if(m_position.x > mins.x && m_position.x < maxs.x && m_position.y > mins.y && m_position.y < maxs.y)
		return true;

	return false;
}

