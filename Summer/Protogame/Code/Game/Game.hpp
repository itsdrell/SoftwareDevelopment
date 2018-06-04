#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Core/General/Rgba.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Core/General/EngineCommon.hpp"
#include "Engine/Core/Tools/Stopwatch.hpp"

//=============================================================
// Forward Declare
//=============================================================
class Texture;
class DevConsole;
class Attract;
class Playing;

//=============================================================
// ENUMS
//=============================================================
enum GameStates
{
	NONE = -1,
	LOADING,
	ATTRACT,
	PLAY,
	NUM_OF_GAME_STATES
};

//=============================================================
// Structs
//=============================================================

//=============================================================
// Classes
//=============================================================
class Game
{
public:
	Game();
	
	void		StartUp();

	void		Update();
	void		UpdateLoadingScreen();
	void		ClockDebug();

	void		Render() const;
	void		RenderLoadingScreen() const;

	void		CheckKeyBoardInputs();


public:

	DevConsole*		m_console;
	Texture*		m_tileTexture;

	GameStates		m_currentState;
	Attract*		m_attractState;
	Playing*		m_playingState;

	Timer*			m_loadingScreenTimer;

};
//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
extern Game* g_theGame;
