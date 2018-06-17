#include "Defeat.hpp"
#include "..\Main\GameCommon.hpp"
#include "Engine\Math\Geometry\AABB2.hpp"
#include "..\Main\Game.hpp"
#include "Engine\Core\Tools\Clock.hpp"

Defeat::Defeat()
{
	m_respawnTimer = new Timer(g_theGameClock);
	m_respawnTimer->SetTimer(10.f);
}

void Defeat::Update()
{
	KeyboardInput();
}

void Defeat::KeyboardInput()
{
	
	if(m_respawnTimer->HasElapsed())
	{
		if(WasKeyJustPressed(KEYBOARD_SPACE))
		{
			g_theGame->m_currentState = PLAY;
		}
	}
	
}

void Defeat::Render() const
{
	//=============================================================
	// Set up camera
	g_theRenderer->SetCamera(g_theRenderer->m_defaultUICamera);

	// Prepare for draw
	g_theRenderer->ClearDepth(1.f);
	g_theRenderer->EnableDepth(COMPARE_ALWAYS, true);


	g_theRenderer->DrawAABB2(AABB2(-200.f, -200.f, 200.f, 200.f), Rgba::RED);

	if(m_respawnTimer->HasElapsed())
	{
		g_theRenderer->DrawText2D(Vector2(-30.f, 10.f), "Click space to Try again", 2.f, Rgba::WHITE);

	}
	else
	{
		g_theRenderer->DrawText2D(Vector2(-20.f, 10.f), "DEFEAT", 8.f, Rgba::WHITE);
		g_theRenderer->DrawText2D(Vector2(-25.f, 4.f), "Respawn in : " + std::to_string(m_respawnTimer->GetElapsedTime()), 2.f, Rgba::WHITE);

	}
}
