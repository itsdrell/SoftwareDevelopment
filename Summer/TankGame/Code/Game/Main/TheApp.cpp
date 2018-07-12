#include "Game/Main/TheApp.hpp"
#include "Game/Main/Game.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/General/Blackboard.hpp"
#include "Game/Main/GameCommon.hpp"
#include "Engine/Core/General/EngineCommon.hpp"
#include "Engine/Core/Platform/Time.hpp"
#include "Engine/ThirdParty/tinyxml/tinyxml2.h"
#include "Engine/Core/Tools/ErrorWarningAssert.hpp"
#include "Engine/Core/Utils/XmlUtilities.hpp"
#include "Engine/Core/Tools/Clock.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Trajectory.hpp"
#include "Engine/Renderer/RenderableComponents/Shader.hpp"
#include "Engine/Core/Platform/File.hpp"
#include "Engine/Core/Tools/Profiling/Profiler.hpp"
#include "Engine/Core/Tools/Profiling/ProfilerReport.hpp"
#include "Engine/Renderer/Systems/DebugRenderSystem.hpp"
#include "Engine/Async/Threading.hpp"
#include <iostream>
#include <fstream>
#include "Engine/Core/Tools/LogSystem.hpp"

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
	
	EngineStartUp();
	
	// these are created in engine start up so we just store off for helper reasons
	g_theRenderer =		Renderer::GetInstance();
	g_theInput =		InputSystem::GetInstance();
	g_audio =			AudioSystem::GetInstance();

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

	EngineShutDown();

	g_theInput = nullptr;
	g_theRenderer = nullptr;
	g_audio = nullptr;
}

void App::StartUp()
{
	g_theGame->StartUp();

	CommandRegister("threadTest", "", "print a bunch to a file", RunThreadTest);
}

void App::RunFrame()
{
	MARK_FRAME;
	PROFILE_PUSH(); // I need this if I want to include run frame total
	
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
	PROFILE_PUSH();
	g_theGame->Render();
}

void App::Update()
{
	PROFILE_PUSH();

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

	if(WasKeyJustPressed(G_THE_LETTER_Z))
	{
		LogTaggedPrintv("Zac", "THIS IS A TEST NUMBER %i", 10);
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


void ThreadTest()
{
	std::ofstream outputFile;
	outputFile.open("Log/test.txt", std::fstream::trunc); 

	if(outputFile.is_open() == false)
		return DebuggerPrintf("\n Didn't work\n");

	for (uint i = 0; i < 12000000; ++i) {
		uint rand_uint = GetRandomIntRange(0,1); 
		std::string log = std::to_string(rand_uint);

		outputFile << log;
	}

	outputFile.close(); 
	DebuggerPrintf( "Finished ThreadTestWork" );
}

void RunThreadTest(Command& cb)
{
	UNUSED(cb);
	ThreadCreateAndDetach((thread_cb) ThreadTest, nullptr);
}