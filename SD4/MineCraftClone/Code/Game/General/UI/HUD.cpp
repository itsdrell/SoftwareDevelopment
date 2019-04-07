#include "HUD.hpp"
#include "Game\General\World\World.hpp"
#include "Engine\Renderer\Systems\DebugRenderSystem.hpp"
#include "Game\General\Entities\Player.hpp"
#include "..\GameCamera.hpp"

//===============================================================================================
HUD::HUD(Player* thePlayer)
{
	m_player = thePlayer;
}

//-----------------------------------------------------------------------------------------------
void HUD::Render() const
{
	Shader* test = Shader::CreateOrGetShader("default");
	test->SetCullMode(CULLMODE_NONE);
	test->SetFrontFace(WIND_COUNTER_CLOCKWISE);
	g_theRenderer->SetShader(test);
	g_theRenderer->BindRenderState(test->m_state);

	g_theRenderer->SetCamera(g_theRenderer->m_defaultUICamera);
	// Prepare for draw
	g_theRenderer->ClearDepth(1.f);
	g_theRenderer->EnableDepth(COMPARE_ALWAYS, true);

	// DRAW
	RenderBlockToPlace();
	RenderTime();
	RenderDebugHelpers();

	g_theRenderer->SetShader();
	
	//DebugRenderLog(.1f, m_world->m_blockToPlace->m_name);

	
}

//-----------------------------------------------------------------------------------------------
void HUD::RenderBlockToPlace() const
{
	BlockDefinition* theDef = m_player->m_blockToPlace;

	Texture* background = g_blockSpriteSheet.m_spriteSheetTexture;
	//g_theRenderer->DrawTexturedAABB2(background, AABB2(-50.f, 50.f));
	g_theRenderer->DrawTexturedAABB2(
		AABB2(40.f, 40.f, 48.f, 48.f),
		*background,
		theDef->m_sideUVs.mins,
		theDef->m_sideUVs.maxs,
		Rgba::WHITE);
}

//-----------------------------------------------------------------------------------------------
void HUD::RenderTime() const
{
	g_theRenderer->SetCurrentTexture();
	g_theRenderer->DrawAABB2(AABB2(-49.8f, 45.f, -40.f, 49.8f), Rgba::BLACK);
	g_theRenderer->DrawText2D(Vector2(-49.f, 48.f), "Day: " + std::to_string(m_player->m_worldTheyAreIn->m_theWorldTime.GetDay()), 1.f);
	g_theRenderer->DrawText2D(Vector2(-49.f, 46.f), "Hour: " + std::to_string(m_player->m_worldTheyAreIn->m_theWorldTime.GetHour()), 1.f);
}

//-----------------------------------------------------------------------------------------------
void HUD::RenderDebugHelpers() const
{
	String playerPhysicsMode = "(F1) Physics Mode " + PhysicsModeToString(m_player->m_physicsType);
	g_theRenderer->DrawText2D(Vector2(-49.f, -46.f), playerPhysicsMode, 1.7f);

	String theMode = "(F2) Camera Mode: " + CameraModeToString(m_player->m_worldTheyAreIn->m_gameCamera->m_mode);
	g_theRenderer->DrawText2D(Vector2(-49.f, -48.f), theMode, 1.7f);

	String velocity = "Velocity: " + m_player->m_velocity.ToString();
	g_theRenderer->DrawText2D(Vector2(-49.f, -44.f), velocity, 1.7f);

	String text = "Position: " + m_player->m_position.ToString();
	g_theRenderer->DrawText2D(Vector2(-49.f, -42.f), text, 1.7f);

	String isOnGround = "IsOnGround: " + std::to_string(m_player->m_isOnGround);
	g_theRenderer->DrawText2D(Vector2(20.f, -48.f), isOnGround, 1.7f);
}
