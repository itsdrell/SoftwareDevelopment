#include "Game/TheApp.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/General/Blackboard.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/General/EngineCommon.hpp"
#include "Engine/Core/Platform/Time.hpp"
#include "ThirdParty/tinyxml/tinyxml2.h"
#include "Engine/Core/Tools/ErrorWarningAssert.hpp"
#include "Engine/Core/Utils/XmlUtilities.hpp"
#include "Engine/Core/Tools/Clock.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Trajectory.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Core/Platform/File.hpp"


//  For testing blackboard
// #include "Engine/Math/IntRange.hpp"
// #include "Engine/Math/FloatRange.hpp"
// #include "Engine/Math/IntVector2.hpp"
// #include "Engine/Math/Vector2.hpp"
// #include "Engine/Core/Rgba.hpp"

App* g_theApp = nullptr;


App::App()
{
	LoadFromXML( "Data/GameConfig.xml", g_gameConfigBlackboard );

	g_theMasterClock = new Clock(nullptr); // renderer might use this so im gonna put it first
	g_theRenderer = new Renderer();
	g_theInput = new InputSystem();
	g_audio = new AudioSystem();
	g_theGame = new Game(); // this needs to be last

	// Clean up render log so we only get fresh data
	//LogStringToFile("Log/Shader.log", std::string(CurrentDateTime() + "\n--------------------------------------\n\n").c_str(), true);

	m_isQuitting = false;

	m_timeSinceStart = 0.f;
}

App::~App()
{
	delete g_theGame;
	g_theGame = nullptr;
	delete g_theInput;
	g_theInput = nullptr;
	delete g_theRenderer;
	g_theRenderer = nullptr;
	delete g_audio;
	g_audio = nullptr;
}

void App::StartUp()
{
	g_theGame->StartUp();
}

void App::RunFrame()
{
	ClockSystemBeginFrame();
	g_theInput->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theRenderer->UpdateTime(g_theGameClock->deltaTime, g_theMasterClock->deltaTime);
	g_audio->BeginFrame();
	g_theInput->Update();

	Update();
	Render();

	g_theInput->EndFrame();
	g_theRenderer->EndFrame();
	g_audio->EndFrame();
	

	// So that the computer isnt using all the cores
	Sleep(1);

}

void App::Render() const
{

	g_theGame->Render();
}

void App::Update()
{
	
	//float deltaSeconds = GetDeltaTime();

	g_theGame->Update();

	//////////////////////////////////////////////////////////////////////////
	// HotKeys
	if(WasKeyJustPressed(KEYBOARD_F4))
	{
		g_theRenderer->PrepForScreenShot();
	}

	if(WasKeyJustPressed(KEYBOARD_F9))
	{
		Shader::ReloadShaders();
	}

}

void App::LoadFromXML( const std::string& filePath, Blackboard& out_whereToStoreValues )
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( "Data/GameConfig.xml" );

	tinyxml2::XMLElement* rootElement = doc.RootElement();
	GUARANTEE_OR_DIE(rootElement != nullptr, "Could not read: " + filePath);

	//const char* rootElementName = rootElement->Name();
	//DebuggerPrintf( "Root element was <%s>\n", rootElementName );

	// Populate the blackboard that we wanted
	out_whereToStoreValues.PopulateFromXmlElementAttributes( *rootElement );

}
