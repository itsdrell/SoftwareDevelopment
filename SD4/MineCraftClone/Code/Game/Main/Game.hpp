#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/Geometry/Disc2.hpp"
#include "Engine/Math/Vectors/Vector3.hpp"
#include "Engine/Math/Vectors/Vector4.hpp"
#include "Engine/Core/General/Rgba.hpp"
#include "Engine/Renderer/Systems/Scene.hpp"
#include "Engine/Core/General/EngineCommon.hpp"
#include "Engine/Core/Tools/Stopwatch.hpp"
#include "Engine/ThirdParty/Lua/LuaScript.h"

//=============================================================
// Forward Declare
//=============================================================
class Texture;
class DevConsole;
class Attract;
class Playing;
class Loading;
class NetSession;
class World;

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
String GetGameStateAsString(GameStates theState);
GameStates GetGameStateFromString(String theState);

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
	~Game();

	static Game* GetInstance() { return s_theGame;}

	void		StartUp();
	void		NetworkStartUp();

	void		Update();

	void		ClockDebug();

	void		Render() const;

	void		CheckKeyBoardInputs();
	void		GoToGameStateFrom(GameStates theStateToGoTo, GameStates theStateItWas);
	World*		GetCurrentWorld();

public:

	DevConsole*		m_console = nullptr;
	Texture*		m_tileTexture = nullptr;

	GameStates		m_currentState;
	Attract*		m_attractState = nullptr;
	Playing*		m_playingState = nullptr;
	Loading*		m_loadingState = nullptr;

	Timer*			m_loadingScreenTimer = nullptr;

	NetSession*		m_theNetSession = nullptr;

	LuaScript*		m_luaMain;

	static Game*	s_theGame;

};
//=============================================================
// Standalone C Functions
//=============================================================


//=============================================================
// Externs
//=============================================================
extern Game* g_theGame;
