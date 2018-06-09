#include "Tile.hpp"

Tile::Tile()
{
	m_position = IntVector2::ZERO;
	m_definition = nullptr;
}

Tile::Tile(const IntVector2 & pos, TileDefinition & def)
{
	m_position = pos;
	m_definition = &def;
}
