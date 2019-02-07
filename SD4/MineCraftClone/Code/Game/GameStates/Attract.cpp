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

	if(m_startedBackgroundMusic == false)
	{
		//m_startedBackgroundMusic = true;
		//PlayLoopingSound("bg");
	}
}

void Attract::Render() const
{
	//=============================================================
	// Set up camera

	Shader* test = Shader::CreateOrGetShader("default");
	test->SetCullMode(CULLMODE_NONE);
	test->SetFrontFace(WIND_COUNTER_CLOCKWISE);
	g_theRenderer->SetShader(test);

	g_theRenderer->SetCamera(g_theRenderer->m_defaultUICamera);
	// Prepare for draw
	g_theRenderer->ClearDepth(1.f);
	g_theRenderer->EnableDepth(COMPARE_ALWAYS, true);

	Texture* background = g_theRenderer->CreateOrGetTexture("Data/Images/mineCraftBG.jpg");
	g_theRenderer->DrawTexturedAABB2(background, AABB2(-50.f, 50.f));
	
	RenderHoverText();

	g_theRenderer->SetShader();
}

void Attract::RenderHoverText() const
{
	
	//=============================================================
	// Draw
	Vector2 textPivot = Vector2(-16.f, 25.f);
	float cellHeight = 8.f;
	float offsetY = textPivot.y;
	
	// Play
	if(m_currentMenuItem == PLAY_BUTTON)
		g_theRenderer->DrawText2D(Vector2(textPivot.x, textPivot.y), "Play", cellHeight, Rgba::GREEN);
	else
		g_theRenderer->DrawText2D(Vector2(textPivot.x, textPivot.y), "Play", cellHeight, Rgba::WHITE);

	// Level Editor
	//if(m_currentMenuItem == MAP_EDITOR_BUTTON)
	//	g_theRenderer->DrawText2D(Vector2(textPivot.x * 1.5f,  textPivot.y - offsetY), "Level Editor", cellHeight * .5f, Rgba::GREEN);
	//else
	//	g_theRenderer->DrawText2D(Vector2(textPivot.x * 1.5f,  textPivot.y - offsetY), "Level Editor", cellHeight * .5f, Rgba::WHITE);

	// Quit
	if(m_currentMenuItem == QUIT_BUTTON)
		g_theRenderer->DrawText2D(Vector2(textPivot.x,  textPivot.y - (offsetY * 2.f)), "Quit", cellHeight, Rgba::GREEN);
	else
		g_theRenderer->DrawText2D(Vector2(textPivot.x,  textPivot.y - (offsetY * 2.f)), "Quit", cellHeight, Rgba::WHITE);


}

void Attract::KeyboardInput()
{
	if(IsDevConsoleOpen())
		return;
	
	// Change selection
	if(WasKeyJustReleased(KEYBOARD_DOWN_ARROW))
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

	if(WasKeyJustReleased(KEYBOARD_UP_ARROW))
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
			g_theGame->m_playingState->StartUp();
		}
		
		if(m_currentMenuItem == QUIT_BUTTON)
			g_theApp->m_isQuitting = true;
	}
}
