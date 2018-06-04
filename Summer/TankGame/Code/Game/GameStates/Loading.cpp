#include "Loading.hpp"
#include "..\Main\GameCommon.hpp"
#include "Engine\Math\Geometry\AABB2.hpp"
#include "..\Main\Game.hpp"
#include "Game\GameStates\Playing.hpp"

Loading::Loading()
{
	m_readyToLoad = false;
}

void Loading::LoadAssets()
{

	g_theGame->m_playingState->StartUp();

	// Make sure we see it before switching
	Sleep(1.f);
	g_theGame->m_currentState = ATTRACT;
}

void Loading::Update()
{
	
	// Wait one frame before doing the load
	if(m_readyToLoad == false)
		m_readyToLoad = true;
	else
		LoadAssets();
	
}

void Loading::Render()
{
	//=============================================================
	// Set up camera
	g_theRenderer->SetCamera(g_theRenderer->m_defaultUICamera);

	// Prepare for draw
	g_theRenderer->ClearDepth(1.f);
	g_theRenderer->EnableDepth(COMPARE_ALWAYS, true);

	g_theRenderer->DrawAABB2(AABB2(-200.f, -200.f, 200.f, 200.f), Rgba::GREEN);

	//=============================================================
	float cellHeight = 8.f;
	float offsetX = -30.f;

	// Play

	g_theRenderer->DrawText2D(Vector2(offsetX, 0.f), "Loading", cellHeight, Rgba::WHITE);
}
