#include "Attract.hpp"
#include "Game/Main/GameCommon.hpp"
#include "Engine\Math\Geometry/AABB2.hpp"
#include "Engine\Core\Platform/Window.hpp"
#include "Engine\Renderer\Systems/DebugRenderSystem.hpp"
#include "Game/Main/TheApp.hpp"
#include "Engine/Core/General/Camera.hpp"
#include "Engine\Core\Tools/DevConsole.hpp"
#include "Game/Main/Game.hpp"
#include "Game/GameStates/Playing.hpp"

Attract::Attract()
{
	m_currentMenuItem = PLAY_BUTTON;
}

void Attract::Update()
{
	KeyboardInput();
}

void Attract::Render() const
{
	//=============================================================
	// Set up camera
	g_theRenderer->SetCamera(g_theRenderer->m_defaultUICamera);

	// Prepare for draw
	g_theRenderer->ClearDepth(1.f);
	g_theRenderer->EnableDepth(COMPARE_ALWAYS, true);

	Texture* background = g_theRenderer->CreateOrGetTexture("Data/Images/background1.jpg");
	//Texture* background = g_theRenderer->CreateOrGetTexture("Data/Images/background2.png");
	g_theRenderer->DrawTexturedAABB2(background, AABB2(-50.f, 50.f));
	//g_theRenderer->DrawAABB2(AABB2(-200.f, -200.f, 200.f, 200.f), Rgba::BLUE);

	RenderHoverText();
}

void Attract::RenderHoverText() const
{
	
	//=============================================================
	// Draw

	float cellHeight = 8.f;
	float offsetX = -16.f;
	float offsetY = 10.f;
	
	// Play
	if(m_currentMenuItem == PLAY_BUTTON)
		g_theRenderer->DrawText2D(Vector2(offsetX, offsetY), "Play", cellHeight, Rgba::GREEN);
	else
		g_theRenderer->DrawText2D(Vector2(offsetX, offsetY), "Play", cellHeight, Rgba::WHITE);

	// Quit
	if(m_currentMenuItem == QUIT_BUTTON)
		g_theRenderer->DrawText2D(Vector2(offsetX, - offsetY), "Quit", cellHeight, Rgba::GREEN);
	else
		g_theRenderer->DrawText2D(Vector2(offsetX, - offsetY), "Quit", cellHeight, Rgba::WHITE);


}

void Attract::KeyboardInput()
{
	if(IsDevConsoleOpen())
		return;
	
	// Change selection
	if(WasKeyJustReleased(KEYBOARD_UP_ARROW) || WasKeyJustReleased(KEYBOARD_DOWN_ARROW))
	{
		if(m_currentMenuItem == PLAY_BUTTON)
		{
			m_currentMenuItem = QUIT_BUTTON;
			return;
		}

		if(m_currentMenuItem == QUIT_BUTTON)
		{
			m_currentMenuItem = PLAY_BUTTON;
			return;
		}

	}

	// Do the option
	if(WasKeyJustPressed(KEYBOARD_ENTER))
	{
		if(m_currentMenuItem == PLAY_BUTTON)
		{
			g_theGame->m_currentState = PLAY;
		}
		
		if(m_currentMenuItem == QUIT_BUTTON)
			g_theApp->m_isQuitting = true;
		
		
	}
}
