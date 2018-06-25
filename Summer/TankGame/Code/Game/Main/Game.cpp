#pragma warning(disable: 4100) // for commands
#include "Game/Main/Game.hpp"
#include "Game/Main/TheApp.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Images/Sprites/SpriteSheet.hpp"
#include "Engine/Core/General/EngineCommon.hpp"
#include "Game/Main/GameCommon.hpp"
#include <vector>
#include "Engine/Core/General/Rgba.hpp"
#include "Engine/Math/Vectors/Vector2.hpp"
#include "Engine/Core/Tools/ErrorWarningAssert.hpp"
#include "Engine/Math/Vectors/Vector3.hpp"
#include "Engine/Core/Tools/DevConsole.hpp"
#include "Engine/Core/Tools/Command.hpp"
#include "Engine/Renderer/Images/Textures/Texture.hpp"
#include "Engine/Core/Tools/Clock.hpp"
#include "Engine/Renderer/Systems/MeshBuilder.hpp"
#include "Engine/Core/General/Camera.hpp"
#include "Engine/Renderer/Systems/DebugRenderSystem.hpp"
#include "Engine/Renderer/RenderableComponents/Material.hpp"
#include "Engine/Renderer/Systems/Lights.hpp"
#include "Engine/Renderer/Systems/Scene.hpp"
#include "Engine/Renderer/Systems/ForwardRenderingPath.hpp"
#include "Engine/Renderer/Images/Textures/TextureCube.hpp"
#include "Game/GameStates/Attract.hpp"
#include "Game/GameStates/Playing.hpp"
#include "Engine/Core/Tools/Stopwatch.hpp"
#include "../GameStates/Loading.hpp"
#include "../GameStates/ReadyUp.hpp"
#include "Engine/Math/Geometry/AABB3.hpp"
#include "Engine/Math/Matrices/Matrix33.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "../GameStates/Victory.hpp"
#include "Game/GameStates/Defeat.hpp"
#include "../GameStates/Playing.hpp"
#include "Game/GameSpecific/GameMap.hpp"

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
	m_loadingState = new Loading();
	m_readyUpState = new ReadyUp();
	m_victoryState = new Victory();
	m_defeatState = new Defeat();
}

void Game::StartUp()
{
	
	m_console->StartUp();

}


void Game::Update()
{
	

	switch (m_currentState)
	{
	case NONE:
		break;
	case LOADING:
		m_loadingState->Update();
		break;
	case ATTRACT:
		m_attractState->Update();
		break;
	case PLAY:
		m_playingState->Update();
		break;
	case READY_UP_NERDS:
		m_readyUpState->Update();
		break;
	case VICTORY:
		m_victoryState->Update();
		break;
	case DEFEAT:
		m_defeatState->Update();
		break;
	case NUM_OF_GAME_STATES:
		break;
	default:
		break;
	}


	
	CheckKeyBoardInputs();
	m_console->Update(); // using engine clock?
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
		m_loadingState->Render();
		break;
	case ATTRACT:
		m_attractState->Render();
		break;
	case PLAY:
		m_playingState->Render();
		break;
	case READY_UP_NERDS:
		m_readyUpState->Render();
		break;
	case VICTORY:
		m_victoryState->Render();
		break;
	case DEFEAT:
		m_defeatState->Render();
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

void Game::GoToDefeatState()
{
	m_currentState = DEFEAT;
	m_defeatState->Enter();
}

void Game::GoToVictoryState()
{
	m_currentState = VICTORY;
}

bool Game::Raycast(Contact3* contact, Ray3 theRay)
{
	return m_playingState->m_map->Raycast(contact, theRay);
}

