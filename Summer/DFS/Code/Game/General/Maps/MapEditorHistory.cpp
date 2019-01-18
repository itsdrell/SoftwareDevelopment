#include "MapEditorHistory.hpp"
#include "Game\Main\Game.hpp"
#include "Game\General\Maps\Map.hpp"
#include "Game\General\GameObjects\Unit.hpp"
#include "Game\General\GameObjects\Building.hpp"
#include "Game\GameStates\MapEditor.hpp"

//===============================================================================================
UnitHistory::UnitHistory(Unit& unitToDestroy)
{
	m_unitToDestroy = &unitToDestroy;
}

//-----------------------------------------------------------------------------------------------
void UnitHistory::Undo()
{
	m_unitToDestroy->m_tileIAmOn->m_unit = nullptr;
	g_theGame->GetCurrentMap()->DeleteGameObjectFromMap(m_unitToDestroy);
}

//-----------------------------------------------------------------------------------------------
BuildingHistory::BuildingHistory(Building & buildingToDestroy)
{
	m_buildingToDestroy = &buildingToDestroy;
}

//-----------------------------------------------------------------------------------------------
void BuildingHistory::Undo()
{
	m_buildingToDestroy->m_tileReference->m_building = nullptr;
	g_theGame->GetCurrentMap()->DeleteGameObjectFromMap(m_buildingToDestroy);
}

//-----------------------------------------------------------------------------------------------
TileHistory::TileHistory(TileDefinition* previousDef, Tile& theTile)
{
	m_previousDefinition = previousDef;
	m_theTile = &theTile;
}

//-----------------------------------------------------------------------------------------------
void TileHistory::Undo()
{
	m_theTile->m_definition = m_previousDefinition;

	// maybe make this a "isDirty"
	g_theGame->m_mapEditorState->MarkRenderableAsDirty();
}
