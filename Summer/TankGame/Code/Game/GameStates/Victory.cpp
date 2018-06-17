#include "Victory.hpp"
#include "..\Main\GameCommon.hpp"
#include "Engine\Math\Geometry\AABB2.hpp"
#include "..\..\DFS\Code\Game\Main\Game.hpp"

void Victory::Update()
{
	KeyboardInput();
}

void Victory::KeyboardInput()
{
	if(WasKeyJustPressed(KEYBOARD_SPACE))
	{
		g_theGame->m_currentState = ATTRACT;
	}
}

void Victory::Render() const
{
	//=============================================================
	// Set up camera
	g_theRenderer->SetCamera(g_theRenderer->m_defaultUICamera);

	// Prepare for draw
	g_theRenderer->ClearDepth(1.f);
	g_theRenderer->EnableDepth(COMPARE_ALWAYS, true);


	g_theRenderer->DrawAABB2(AABB2(-200.f, -200.f, 200.f, 200.f), Rgba::BLUE);

	g_theRenderer->DrawText2D(Vector2(-16.f, 10.f), "VICTORY", 8.f, Rgba::WHITE);

}
