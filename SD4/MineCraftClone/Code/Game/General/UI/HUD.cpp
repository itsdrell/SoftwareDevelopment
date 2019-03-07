#include "HUD.hpp"
#include "Game\General\World\World.hpp"
#include "Engine\Renderer\Systems\DebugRenderSystem.hpp"

//===============================================================================================
HUD::HUD()
{
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

	BlockDefinition* theDef = m_world->m_blockToPlace;

	Texture* background = g_blockSpriteSheet.m_spriteSheetTexture;
	//g_theRenderer->DrawTexturedAABB2(background, AABB2(-50.f, 50.f));
	g_theRenderer->DrawTexturedAABB2(
		AABB2(40.f, 40.f, 48.f, 48.f), 
		*background, 
		theDef->m_sideUVs.mins, 
		theDef->m_sideUVs.maxs, 
		Rgba::WHITE);


	//-----------------------------------------------------------------------------------------------
	g_theRenderer->SetCurrentTexture();
	g_theRenderer->DrawAABB2(AABB2(-49.8f, 45.f, -40.f, 49.8f), Rgba::BLACK);
	g_theRenderer->DrawText2D(Vector2(-49.f, 48.f), "Day: " + std::to_string(m_world->m_theWorldTime.GetDay()), 1.f);
	g_theRenderer->DrawText2D(Vector2(-49.f, 46.f), "Hour: " + std::to_string(m_world->m_theWorldTime.GetHour()), 1.f);

	g_theRenderer->SetShader();
	
	//DebugRenderLog(.1f, m_world->m_blockToPlace->m_name);

	
}
