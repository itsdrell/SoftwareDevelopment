#include "ReadyUp.hpp"
#include "..\Main\GameCommon.hpp"
#include "Engine\Math\Geometry\AABB2.hpp"
#include "..\Main\Game.hpp"

ReadyUp::ReadyUp()
{
}

void ReadyUp::Update()
{
	if(WasKeyJustPressed(KEYBOARD_ENTER))
	{
		g_theGame->m_currentState = PLAY;
	}
}

void ReadyUp::Render() const
{
	//=============================================================
	// Set up camera
	g_theRenderer->SetCamera(g_theRenderer->m_defaultUICamera);

	// Prepare for draw
	g_theRenderer->ClearDepth(1.f);
	g_theRenderer->EnableDepth(COMPARE_ALWAYS, true);

	g_theRenderer->DrawAABB2(AABB2(-200.f, -200.f, 200.f, 200.f), Rgba::RED);

	//=============================================================
	float cellHeight = 8.f;
	float offsetX = -30.f;

	// Play

	g_theRenderer->DrawText2D(Vector2(offsetX, 0.f), "HIT ENTER", cellHeight, Rgba::WHITE);
}
