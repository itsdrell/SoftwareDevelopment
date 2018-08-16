#include "EngineCommon.hpp"
#include "Engine/Core/General/Blackboard.hpp"
#include "Engine/Renderer/Images/Sprites/SpriteSheet.hpp"
#include "Engine/Renderer/Pipeline/VertexLayout.hpp"
#include "Game/Main/GameCommon.hpp"
#include "../Platform/Window.hpp"
#include "Engine/Core/Tools/Profiling/Profiler.hpp"
#include "../Tools/LogSystem.hpp"
#include "Engine/Async/Threading.hpp"


//====================================================================================
Blackboard g_gameConfigBlackboard = Blackboard();
SpriteSheet g_tileSpriteSheet;
SpriteSheet g_dialogueSpriteSheet;


//====================================================================================
void EngineStartUp()
{
	AudioSystem* audio = new AudioSystem();
	InputSystem* input = new InputSystem();
	Renderer* renderer = new Renderer();

	LogSystemStartUp();
	renderer->RenderStartup(Window::GetInstance()->GetHandle()); // call the static variable


	audio = nullptr;
	input = nullptr;
	renderer = nullptr;
}

void EngineShutDown()
{
	delete g_theAudioSystem;
	g_theAudioSystem = nullptr;
	
	delete g_theInputSystem;
	g_theInputSystem = nullptr;

	delete Renderer::GetInstance();
	g_theRenderer = nullptr;

	if(Profiler::GetInstance() != nullptr)
		delete Profiler::GetInstance();

	LogSystemShutDown();

	Thread::Shutdown();
}
