#include "MapEditorHistory.hpp"
#include "Game\Main\Game.hpp"
#include "Game\General\Maps\Map.hpp"
#include "Game\General\GameObjects\Unit.hpp"
#include "Game\General\GameObjects\Building.hpp"
#include "Game\GameStates\MapEditor.hpp"

//===============================================================================================
UnitHistory::UnitHistory(const IntVector2& tilePos)
	: HistoryItem(tilePos)
{
}

//-----------------------------------------------------------------------------------------------
void UnitHistory::Undo()
{
	Tile* theTile = g_theGame->GetCurrentMap()->GetTile(m_position);
	g_theGame->GetCurrentMap()->DeleteGameObjectFromMap(theTile->m_unit);
	theTile->m_unit = nullptr;
}

//-----------------------------------------------------------------------------------------------
BuildingHistory::BuildingHistory(const IntVector2& tilePos)
	: HistoryItem(tilePos)
{
}

//-----------------------------------------------------------------------------------------------
void BuildingHistory::Undo()
{	
	Tile* theTile = g_theGame->GetCurrentMap()->GetTile(m_position);
	g_theGame->GetCurrentMap()->DeleteGameObjectFromMap(theTile->m_building);
	theTile->m_building = nullptr;
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
	g_theGame->m_mapEditorState->MarkRenderableAsDirty();
}

//-----------------------------------------------------------------------------------------------
DeleteHistory::DeleteHistory(UndoType theType, TeamName theTeam, const String& definitionName, const IntVector2& tilePos)
	: HistoryItem(tilePos)
{
	m_type = theType;
	m_team = theTeam;
	m_definitionName = definitionName;
}

//-----------------------------------------------------------------------------------------------
void DeleteHistory::Undo()
{
	Map* currentMap = g_theGame->GetCurrentMap();
	
	switch (m_type)
	{
	case UNDO_TYPE_UNIT:
		currentMap->CreateUnit(m_definitionName, m_team, m_position);
		break;
	case UNDO_TYPE_BUILDING:
		currentMap->CreateBuilding(m_definitionName, m_team, m_position);
		break;
	case UNDO_TYPE_TILE:
		currentMap->ChangeTile(m_definitionName, m_position);
		break;
	default:
		break;
	}
}
