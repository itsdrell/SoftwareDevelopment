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
#include "Game/General/ConsoleCommands.hpp"
#include "Engine/Net/UDPSocket.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Game/Main/Playground.hpp"
#include "Engine/Net/EngineRegisteredNetMessages.hpp"
#include "../Net/GameRegisteredNetMessages.hpp"
#include "Engine/Core/General/EngineLuaFunctionBindings.hpp"
#include "Game/Main/EngineBuildPreferences.hpp"
#include "Game/GameStates/MapEditor.hpp"


//////////////////////////////////////////////////////////////////////////

Game* g_theGame = nullptr;
Game* Game::s_theGame = nullptr;

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

	RegisterCommands();

	// Create all the states
	m_currentState = LOADING;
	m_attractState = new Attract();
	m_playingState = new Playing();
	m_loadingState = new Loading();
	//m_mapEditorState = new MapEditor();
	
	m_luaMain = new LuaScript("Main");
	LuaStartUp(m_luaMain);

	s_theGame = this;
}

//--------------------------------------------------------------------------
Game::~Game()
{
	delete m_attractState;
	m_attractState = nullptr;

	delete m_playingState;
	m_playingState = nullptr;

	delete m_loadingState;
	m_loadingState = nullptr;

	delete m_loadingScreenTimer;
	m_loadingScreenTimer = nullptr;

	delete m_console;
	m_console = nullptr;

	delete m_luaMain;
	m_luaMain = nullptr;

	delete m_mapEditorState;
	m_mapEditorState = nullptr;

	//delete g_theGameClock; // the master clock will delete this
	//g_theGameClock = nullptr;
}

void Game::StartUp()
{

	m_loadingScreenTimer = new Timer(g_theGameClock);
	m_loadingScreenTimer->SetTimer(3.f);

	m_console->StartUp();

	NetworkStartUp();
	
	//m_theNetSession->Bind( GAME_PORT, 16U);
}

//-----------------------------------------------------------------------------------------------
void Game::NetworkStartUp()
{
#if defined(NETWORKING_ENABLED)
	
	m_theNetSession = new NetSession();
	RegisterNetCallbacks();

#endif
}

void Game::RegisterCommands()
{
	CommandRegister("quit","Type: quit","Exits Application", QuitGame);
	RegisterGameCommands();
}

void Game::RegisterNetCallbacks()
{
	RegisterCoreEngineNetMessages(*m_theNetSession);
	RegisterGameNetMessages(*m_theNetSession);
}

void Game::Update()
{
	if(m_theNetSession != nullptr ) { m_theNetSession->Update(); }

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
	case MAP_EDIT:
		m_mapEditorState->Update();
		break;
	case NUM_OF_GAME_STATES:
		break;
	default:
		break;
	}


	CheckKeyBoardInputs();
	if(m_theNetSession != nullptr ) { m_theNetSession->ProcessOutgoing(); }
	m_console->Update(); // using engine clock?

	LuaUpdate(*m_luaMain, g_theGameClock->deltaTime);
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
	case MAP_EDIT:
		m_mapEditorState->Render();
		break;
	case NUM_OF_GAME_STATES:
		break;
	default:
		break;
	}

	
	LuaRender(*m_luaMain);

	//////////////////////////////////////////////////////////////////////////
	// Show the console, we return instantly if its not open
	if(m_theNetSession != nullptr ) { NetSession::GetInstance()->Render(); }
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

//-----------------------------------------------------------------------------------------------
Map* Game::GetCurrentMap()
{
	if(m_currentState == MAP_EDIT)
		return m_mapEditorState->m_currentMap;
	if(m_currentState == PLAY)
		return (Map*) m_playingState->m_currentMap;

	return nullptr;
}

//-----------------------------------------------------------------------------------------------
void Game::GoToGameStateFrom(GameStates theStateToGoTo, GameStates theStateItWas)
{
	if(m_currentState != theStateItWas)
		m_currentState = theStateItWas;

	

	switch (m_currentState)
	{
	case NONE:
		break;
	case LOADING:
		break;
	case ATTRACT:
		break;
	case PLAY:
		break;
	case MAP_EDIT:
		m_mapEditorState->Exit(); // cleanup. May not need since we are exiting
		delete m_mapEditorState;
		m_mapEditorState = nullptr;
		break;
	case NUM_OF_GAME_STATES:
		break;
	default:
		break;
	}



	m_currentState = theStateToGoTo;
}

//-----------------------------------------------------------------------------------------------
String GetGameStateAsString(GameStates theState)
{
	String result;
	
	switch (theState)
	{
	case NONE:
		break;
	case LOADING:
		result = "loading";
		break;
	case ATTRACT:
		result = "attract";
		break;
	case PLAY:
		result = "play";
		break;
	case MAP_EDIT:
		result = "mapEdit";
		break;
	case NUM_OF_GAME_STATES:
		break;
	default:
		break;
	}
	return result;
}

//-----------------------------------------------------------------------------------------------
GameStates GetGameStateFromString(String theState)
{
	if(theState == "attract")	{ return ATTRACT; }
	if(theState == "loading")	{ return LOADING; }
	if(theState == "play")		{ return PLAY; }
	if(theState == "mapEdit")	{ return MAP_EDIT; }

	return NONE;
}
