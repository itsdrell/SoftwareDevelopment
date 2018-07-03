#include "HUD.hpp"
#include "Game\GameStates\Playing.hpp"
#include "Engine\Renderer\RenderableComponents\Material.hpp"
#include "Game\General\Tiles\Tile.hpp"
#include "Game\General\Tiles\TileDefinition.hpp"
#include "Engine\Core\General\EngineCommon.hpp"
#include "Engine\Renderer\Images\Sprites\SpriteSheet.hpp"
#include "..\GameObjects\Unit.hpp"
#include "..\..\SystemsAndTools\GameObject2D.hpp"
#include "Engine\Renderer\Images\Sprites\Sprite.hpp"
#include "Game\General\Map.hpp"
#include "Game\Main\Game.hpp"
#include "Game\GameStates\Playing.hpp"
#include "..\..\Main\GameCommon.hpp"
#include "..\GameObjects\Building.hpp"

void HUD::Render() const
{
	//--------------------------------------------------------------------------
	Renderer* r = Renderer::GetInstance();
	
	r->SetCamera(r->m_defaultUICamera);
	r->BindMaterial(Material::CreateOrGetMaterial("sprite"));

	// Prepare for draw
	r->ClearDepth(1.f);
	r->EnableDepth(COMPARE_ALWAYS, true);
	
	//--------------------------------------------------------------------------
	RenderPlayerDisplay();
	
	if(g_currentTile != nullptr)
	{
		RenderTileDisplay();
		RenderUnitDisplay();
		RenderAttackChance();
	}
}

void HUD::RenderPlayerDisplay() const
{
	Renderer* r = Renderer::GetInstance();
	TeamName theTeam = g_theGame->m_playingState->m_currentMap->m_turnOrder.GetCurrentTeamTurn();

	// variables
	AABB2 backgroundBounds	= AABB2(-48.f, 40.f, -30.f, 48.f);
	AABB2 iconBounds		= AABB2(backgroundBounds.mins.x, backgroundBounds.mins.y, -40.f, backgroundBounds.maxs.y);
	
	Vector2 nameTextLocation =  Vector2(		iconBounds.maxs.x + 1.f,		backgroundBounds.maxs.y - 1.5f);
	Vector2 moneyTextLocation = Vector2(		iconBounds.maxs.x + 1.f,		backgroundBounds.maxs.y - 4.5f);
	Vector2 turnTextLocation =  Vector2(		iconBounds.maxs.x + 1.f,		backgroundBounds.maxs.y - 7.5f);

	r->DrawAABB2( backgroundBounds, GetColorFromTeamName(theTeam));
	r->DrawTexturedAABB2( r->m_defaultTexture, iconBounds);
	r->DrawText2D( nameTextLocation, "PLAYER", 1.f);
	r->DrawText2D( moneyTextLocation, "MONEY", 1.f);
	r->DrawText2D( turnTextLocation, "TURN: X", 1.f);

}

void HUD::RenderTileDisplay() const
{
	Renderer* r = Renderer::GetInstance();
	Building* theBuilding = g_currentTile->m_building;

	AABB2 imageUvs = g_currentTile->m_definition->m_uvCoords;
	AABB2 backgroundBounds =	AABB2( -48.f, -48.f, -38.f, -25.f);
	AABB2 tileImageBounds =		AABB2(backgroundBounds.mins.x + 2.f, backgroundBounds.maxs.y - 12.f, backgroundBounds.maxs.x - 2.f, backgroundBounds.maxs.y - 2.f);

	Vector2 tileNamePos =	Vector2(	tileImageBounds.mins.x,	tileImageBounds.mins.y - 1.5f	);
	Vector2 teamPos =		Vector2(	tileImageBounds.mins.x,	tileImageBounds.mins.y - 4.5f	);
	Vector2 healthPos =		Vector2(	tileImageBounds.mins.x,	tileImageBounds.mins.y - 7.5f	);

	Rgba color = Rgba(255,255,255,200);
	Texture* theTexture = g_tileSpriteSheet.m_spriteSheetTexture;
	std::string nameText =  g_currentTile->m_definition->m_name;
	std::string teamName = "";
	std::string health = "";

	// if there is a building, we are gonna replace the info
	if(theBuilding != nullptr)
	{
		color = GetColorFromTeamName(theBuilding->m_team);
		imageUvs = AABB2(0.f, 1.f);
		theTexture = theBuilding->m_renderable->GetSprite()->m_image;
		nameText = theBuilding->m_name;
		teamName = TeamNameToString(theBuilding->m_team);
		health = "HP: " + theBuilding->m_health;
	}


	r->DrawAABB2(backgroundBounds, color);
	r->DrawTexturedAABB2(tileImageBounds, *theTexture, imageUvs.mins, imageUvs.maxs, Rgba::WHITE);
	r->DrawText2D(tileNamePos, nameText, 1.f);
	r->DrawText2D(teamPos, teamName, 1.f);
	r->DrawText2D(healthPos,health, 1.f);
	
}

void HUD::RenderUnitDisplay() const
{
	if(g_currentTile->m_unit == nullptr)
		return;
	
	Renderer* r = Renderer::GetInstance();
	Unit& currentUnit = *g_currentTile->m_unit;

	AABB2 backgroundBounds = AABB2( -36.f, -48.f, -26.f, -25.f);
	AABB2 unitImageBounds =	AABB2(backgroundBounds.mins.x + 2.f, backgroundBounds.maxs.y - 12.f, backgroundBounds.maxs.x - 2.f, backgroundBounds.maxs.y - 2.f);

	Vector2 tileNamePos =	Vector2(	unitImageBounds.mins.x,	unitImageBounds.mins.y - 1.5f	);
	Vector2 teamPos =		Vector2(	unitImageBounds.mins.x,	unitImageBounds.mins.y - 4.5f	);
	Vector2 healthPos =		Vector2(	unitImageBounds.mins.x,	unitImageBounds.mins.y - 7.5f	);

	Sprite currentSprite = *currentUnit.m_renderable->GetSprite();

	r->DrawAABB2( backgroundBounds, GetColorFromTeamName(currentUnit.m_team));
	r->DrawTexturedAABB2(unitImageBounds, *currentSprite.m_image, currentSprite.m_uv.mins, currentSprite.m_uv.maxs, Rgba::WHITE);
	r->DrawText2D(tileNamePos, currentUnit.m_name, 1.f);
	r->DrawText2D(teamPos, TeamNameToString(currentUnit.m_team), 1.f);
	r->DrawText2D(healthPos, "HP: " + std::to_string(currentUnit.m_health), 1.f);
}

void HUD::RenderAttackChance() const
{
	Renderer* r = Renderer::GetInstance();
	
	// if we are in attack mode
	PlayState currentState = g_theGame->m_playingState->m_currentPlayState;

	if(currentState == ACTION)
	{
		// make sure we have a unit there
		if(nullptr != g_currentTile->m_unit /*&& nullptr != g_theGame->m_playingState->m_selectedUnit*/)
		{
			// if the selected unit's type != the hover target type
			Unit& hoveredUnit = *g_currentTile->m_unit;
			Unit& currentUnit = *g_theGame->m_playingState->m_currentMap->m_selectedUnit;

			if(hoveredUnit.m_team != currentUnit.m_team)
			{
				AABB2 backgroundBounds =	AABB2( -36.f, -24.f, -26.f, -10.f);
				AABB2 textBounds =			AABB2( -36.f, -24.f, -26.f, -14.f);

				std::string chance = std::to_string((int) CalculateWinChance(currentUnit, hoveredUnit));
				
				// Draw Image
				r->DrawAABB2(backgroundBounds, Rgba::WHITE);
				r->DrawAABB2(textBounds, Rgba::RED);

				r->DrawText2D( Vector2(	backgroundBounds.mins.x + 2.f,		backgroundBounds.maxs.y - 3.f)	, "DAMAGE", 1.f, Rgba::BLACK);
				r->DrawText2D( Vector2(	textBounds.GetCenter().x - 1.5f,	textBounds.GetCenter().y)		, chance, 1.f);
			}
		}

	}


}
