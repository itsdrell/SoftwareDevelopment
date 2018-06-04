#pragma warning(disable: 4100) // for commands
#include "Game/Game.hpp"
#include "Game/TheApp.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/General/EngineCommon.hpp"
#include "Game/GameCommon.hpp"
#include <vector>
#include "Engine/Core/General/Rgba.hpp"
#include "Engine/Math/Vectors/Vector2.hpp"
#include "Engine/Core/Tools/ErrorWarningAssert.hpp"
#include "Engine/Math/Vectors/Vector3.hpp"
#include "Engine/Core/Tools/DevConsole.hpp"
#include "Engine/Core/Tools/Command.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/Tools/Clock.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Core/General/Camera.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Lights.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/Renderer/TextureCube.hpp"
#include "Attract.hpp"
#include "Playing.hpp"
#include "Engine/Core/Tools/Stopwatch.hpp"


//////////////////////////////////////////////////////////////////////////

Game* g_theGame = nullptr;

//////////////////////////////////////////////////////////////////////////
// Command Tests (no console)
void QuitGame(Command& thecommand)
{
	g_theApp->m_isQuitting = true;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor so lets make what we need
Game::Game()
{
	m_console = new DevConsole(g_theRenderer);
	g_theGameClock = new Clock(g_theMasterClock);

	CommandRegister("quit","Type: quit","Exits Application", QuitGame);

	//=============================================================
	// Create all the states
	m_currentState = LOADING;
	m_attractState = new Attract();
	m_playingState = new Playing();
	
}

void Game::StartUp()
{
	

	m_loadingScreenTimer = new Timer(g_theGameClock);
	m_loadingScreenTimer->SetTimer(3.f);

	m_console->StartUp();
	
}


void Game::Update()
{
	

	switch (m_currentState)
	{
	case NONE:
		break;
	case LOADING:
		UpdateLoadingScreen();
		break;
	case ATTRACT:
		m_attractState->Update();
		break;
	case PLAY:
		m_playingState->Update();
		break;
	case NUM_OF_GAME_STATES:
		break;
	default:
		break;
	}


	
	CheckKeyBoardInputs();
	m_console->Update(); // using engine clock?
}

void Game::UpdateLoadingScreen()
{
	if(m_loadingScreenTimer->HasElapsed())
		m_currentState = ATTRACT;
}

void Game::ClockDebug()
{
	//////////////////////////////////////////////////////////////////////////
	// this is gonna be used for clock debug
	if(g_theMasterClock->m_debugMode == true)
	{
		if(WasKeyJustPressed(G_THE_LETTER_L))
		{
			g_theMasterClock->IncreaseScale();
		}

		if(WasKeyJustPressed(G_THE_LETTER_K))
		{
			g_theMasterClock->DescreaseScale();
		}

		if(WasKeyJustPressed(G_THE_LETTER_P))
		{
			g_theMasterClock->TogglePaused();
			g_theMasterClock->SetScale(0.f);
			DevConsole::AddConsoleDialogue(ConsoleDialogue("TIME HAS BEEN TOGGLED AND SCALE RESET", Rgba::YELLOW));
		}

		if(WasKeyJustPressed(KEYBOARD_BACKSPACE))
		{
			g_theMasterClock->m_debugMode = false;
			DevConsole::AddConsoleDialogue(ConsoleDialogue("DEBUG MODE DISABLED", Rgba::YELLOW));
		}

		if(WasKeyJustPressed(G_THE_LETTER_O))
		{
			g_theMasterClock->StepForwardAFrame();
		}
	}

}



void Game::Render() const
{
	
	switch (m_currentState)
	{
	case NONE:
		break;
	case LOADING:
		RenderLoadingScreen();
		break;
	case ATTRACT:
		m_attractState->Render();
		break;
	case PLAY:
		m_playingState->Render();
		break;
	case NUM_OF_GAME_STATES:
		break;
	default:
		break;
	}

	//////////////////////////////////////////////////////////////////////////
	// Show the console, we return instantly if its not open
	m_console->Render();
	
}

void Game::RenderLoadingScreen() const
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


void Game::CheckKeyBoardInputs()
{
	// dont check for input unless its not open
	if(IsDevConsoleOpen()) return;
	
	if(WasKeyJustPressed(G_TILDE))
	{
		m_console->Toggle(); // Turns off or on the Console
	}


	//////////////////////////////////////////////////////////////////////
	ClockDebug();
	
}


