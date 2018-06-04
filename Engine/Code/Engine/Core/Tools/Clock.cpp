#include "Clock.hpp"
#include "Engine/Core/Platform/Time.hpp"
#include "Command.hpp"
#include "DevConsole.hpp"



//////////////////////////////////////////////////////////////////////////
// This is called in the App
Clock* g_theMasterClock = nullptr;
Clock* g_theGameClock = nullptr;

//////////////////////////////////////////////////////////////////////////
// Creating console commands here
void DebugClock(Command& thecommand)
{
	std::string whatWeWantToDo = "";
	
	// we have a parameter
	if(thecommand.m_commandArguements.size() > 1)
		whatWeWantToDo = thecommand.m_commandArguements.at(1);
	
	if(whatWeWantToDo == "help") // show commands
	{
		DevConsole::AddConsoleDialogue(ConsoleDialogue("Just type clock with nothing after to enter debug mode w/ hotkeys", Rgba::YELLOW));
		DevConsole::AddConsoleDialogue(ConsoleDialogue("Type: clock pause to stop the clock", Rgba::YELLOW));
		DevConsole::AddConsoleDialogue(ConsoleDialogue("Type: clock unpause to resume the clock", Rgba::YELLOW));
		DevConsole::AddConsoleDialogue(ConsoleDialogue("Type: clock + number to scale the clock", Rgba::YELLOW));
	}

	if(whatWeWantToDo == "pause") // pause
	{
		DevConsole::AddConsoleDialogue(ConsoleDialogue("Clock Paused", Rgba::YELLOW));
		g_theMasterClock->SetPaused(true);
	}

	if(whatWeWantToDo == "unpause")
	{
		DevConsole::AddConsoleDialogue(ConsoleDialogue("Clock UnPaused", Rgba::YELLOW));
		g_theMasterClock->SetPaused(false);
	}

	if(whatWeWantToDo == "") // turn on debug
	{
		
		// Tell the user what hotkeys do what
		if(g_theMasterClock->m_debugMode == false)
		{
			DevConsole::AddConsoleDialogue(ConsoleDialogue("CLOCK DEBUG ENABLED", Rgba::YELLOW));
			DevConsole::AddConsoleDialogue(ConsoleDialogue("P to toggle paused mode (also turns scale to 0)", Rgba::YELLOW));
			DevConsole::AddConsoleDialogue(ConsoleDialogue("L Key Increases scale by .5", Rgba::YELLOW));
			DevConsole::AddConsoleDialogue(ConsoleDialogue("K Key Decreases scale by .5", Rgba::YELLOW));
			DevConsole::AddConsoleDialogue(ConsoleDialogue("O Key Goes to next frame", Rgba::YELLOW));
			DevConsole::AddConsoleDialogue(ConsoleDialogue("Backspace exits debug mode", Rgba::YELLOW));

		}
		else
		{
			DevConsole::AddConsoleDialogue(ConsoleDialogue("CLOCK DEBUG DISABLED", Rgba::YELLOW));
			g_theMasterClock->SetScale(1.f);
			g_theMasterClock->SetPaused(false);

		}

		// then enable the hotkeys
		g_theMasterClock->m_debugMode = !g_theMasterClock->m_debugMode;
	}

	// assume its a scale so convert to float and scale clock
	float scale = (float) atof(whatWeWantToDo.c_str());

	if(scale > 0)
	{
		DevConsole::AddConsoleDialogue(ConsoleDialogue("Clock Scale set to: " + whatWeWantToDo, Rgba::YELLOW));
		g_theMasterClock->SetScale(scale);
	}
}





//////////////////////////////////////////////////////////////////////////
//	Clock class
Clock::Clock(Clock* parent)
{
	m_parent = parent;

	Reset();
	m_timeScale = 1.0;
	m_paused = false;
	m_frameCount = 0;
	m_debugMode = false;

	if(parent != nullptr)
	{
		m_parent->AddChild(this);
	}

	if(g_theMasterClock == nullptr)
	{
		// if its null then this is the first constructor which is always master
		g_theMasterClock = this; 

		// registering commands
		CommandRegister("clock","Type: clock [parameter]","Turns on clock debugs. type help for parameter options", DebugClock);
	}
}

Clock::~Clock()
{
}

void Clock::Reset()
{
	m_lastFrameHpc = GetPerformanceCounter();
	
	// reset the structs - THIS IS LEGAL BUT GROSS (leaving it as an example...)
	memset(&frame, 0, sizeof(frame));
	memset(&total, 0, sizeof(total));
}

void Clock::BeginFrame()
{
	uint64_t currentHpc = GetPerformanceCounter();
	uint64_t elapsed = currentHpc - m_lastFrameHpc;

	Advance(elapsed);
}

void Clock::Advance(uint64_t elapsed)
{
	// frames always increase even when paused :)
	m_frameCount++;

	// check if we are paused or need to scale time
	if(m_paused)
	{
		elapsed = 0;
	}
	else
	{
		elapsed = (uint64_t)((double)elapsed * m_timeScale);
	}


	// so we can compares against the new frame hpc and get ds
	m_lastFrameHpc += elapsed;

	// Update the clocks member variables
	double elapsedSeconds = PerformanceCountToSeconds(elapsed);
	frame.hpcSeconds = elapsedSeconds;
	frame.seconds = (float) elapsedSeconds;
	deltaTime = frame.seconds; // storing it off for easier access
	frame.hpc = elapsed;
	frame.ms = (uint)frame.hpcSeconds * 1000;

	total.hpcSeconds += frame.hpcSeconds;
	total.seconds += frame.seconds;
	totalTime = total.seconds;
	total.hpc += frame.hpc;
	total.ms += frame.ms;

	// update all children
	for(Clock* iterator:m_children)
	{
		iterator->Advance(elapsed);
	}


}

void Clock::AddChild(Clock* child)
{
	m_children.push_back(child);
}

// double Clock::GetCurrentTime() const
// {
// 	return total.hpcSeconds;
// }

void Clock::SetPaused(bool paused)
{
	m_paused = paused;
}

void Clock::SetScale(float scale)
{
	m_timeScale = scale;
}


void Clock::IncreaseScale(float scaleToAdd /*= .5f*/)
{
	m_timeScale += scaleToAdd;

	std::string theScale = std::to_string(m_timeScale);
	DevConsole::AddConsoleDialogue(ConsoleDialogue("The Scale is now: " + theScale, Rgba::YELLOW));
}

void Clock::DescreaseScale(float scaleToSubtract /*= .5f*/)
{
	m_timeScale -= scaleToSubtract;

	if(m_timeScale < 0)
		m_timeScale = 0;

	std::string theScale = std::to_string(m_timeScale);
	DevConsole::AddConsoleDialogue(ConsoleDialogue("The Scale is now: " + theScale, Rgba::YELLOW));
}

void Clock::StepForwardAFrame(float deltaToMove /*= .03f*/)
{
	m_paused = true;
	m_timeScale = 0.f;

	for(Clock* iterator:m_children)
	{
		iterator->deltaTime = deltaToMove;
	}
}

//////////////////////////////////////////////////////////////////////////
// C Functions outside of class

Clock* GetMasterClock()
{
	return g_theMasterClock;
}

void ClockSystemBeginFrame()
{
	g_theMasterClock->BeginFrame();
}

double GetTheCurrentTime()
{
	return g_theMasterClock->total.hpcSeconds;
}

float GetDeltaTime()
{
	return g_theMasterClock->frame.seconds;
}


