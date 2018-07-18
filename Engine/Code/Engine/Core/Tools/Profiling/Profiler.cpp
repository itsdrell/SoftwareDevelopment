#include "Profiler.hpp"
#include "Engine/Core\Platform\Time.hpp"
#include "Game\Main\EngineBuildPreferences.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "ProfilerReport.hpp"
#include "ProfilerUI.hpp"
#include <algorithm>
#include "Engine/Renderer/Systems/DebugRenderSystem.hpp"
#include <vector>

#pragma warning( disable : 4100)

//====================================================================================
Profiler* Profiler::s_instance = nullptr;

//====================================================================================
#if defined(PROFILING_ENABLED)

//====================================================================================
void PauseProfiler(Command& theCommand)
{
	UNUSED(theCommand);
	
	Profiler::GetInstance()->m_isPausing = true;
}

void ResumeProfiler(Command& theCommand)
{
	UNUSED(theCommand);
	
	Profiler::GetInstance()->m_isResuming = true;
}


//====================================================================================
ProfileMeasurement::ProfileMeasurement(std::string id)
{
	m_startHPC = GetPerformanceCounter();
	m_id = id;

}

ProfileMeasurement::ProfileMeasurement()
{
	m_id = "NON-INITIALIZED";
	m_startHPC = 0;
	m_endHPC = 0;
}

ProfileMeasurement::~ProfileMeasurement()
{
	for(uint i = 0; i < m_children.size(); i++)
	{
		ProfileMeasurement* current = m_children.at(i);

		m_children.at(i) = nullptr;
		delete current;
	}

}

void ProfileMeasurement::Finish()
{
	m_endHPC = GetPerformanceCounter();
}

double ProfileMeasurement::GetElapsedTime()
{
	return (double) m_endHPC - m_startHPC;
}

double ProfileMeasurement::GetTimeFromChildren()
{
	double total = 0.0;

	for(uint i = 0; i < m_children.size(); i++)
	{
		ProfileMeasurement* current = m_children.at(i);

		total +=  current->GetElapsedTime();

	}

	return total;
}

double ProfileMeasurement::GetRootTotalTime()
{
	if(m_parent != nullptr)
	{
		return m_parent->GetRootTotalTime();
	}
	else
	{
		return GetElapsedTime();
	}
}

//====================================================================================
ProfilerScope::ProfilerScope(std::string id)
{
	Profiler::GetInstance()->Push(id);
}

ProfilerScope::~ProfilerScope()
{
	Profiler::GetInstance()->Pop();
}

//====================================================================================
Profiler::Profiler()
{
	if(s_instance == nullptr)
		s_instance = this;

	m_currentIndex = 0;
}

std::vector<double> Profiler::GetFrameLengths()
{
	std::vector<double> times;
	
	for(uint i = 0; i < MAX_AMOUNT_OF_MEASUREMENTS; i++)
	{
		ProfileMeasurement* current = ProfileGetPreviousFrame(i);

		if(current != nullptr)
			times.push_back(current->GetElapsedTime());
	}

	return times;
}

double Profiler::GetLongestFrame()
{
	std::vector<double> frameLengths = GetFrameLengths();
	
	return *std::max_element(frameLengths.begin(), frameLengths.end());
}

void Profiler::MarkFrame()
{

	if(m_isResuming)
	{
		m_isPaused = false;
		m_isResuming = false;
		return;
	}

	// do this after we finished storing the last frame
	if(m_isPausing)
	{
		m_isPaused = true;
		m_isPausing = false;
		return;
	}

	if(m_activeNode != nullptr && m_isPaused == false)
	{
		//if(m_frameHistory.size() == MAX_AMOUNT_OF_MEASUREMENTS)
		//{
		//	ProfileMeasurement* current = m_frameHistory.front();
		//	delete current;
		//	current = nullptr;
		//	
		//	m_frameHistory.pop();
		//	//DestroyMeasurementTreeRecursive(m_prevFrame);
		//}

		m_currentIndex = (m_currentIndex + 1) % MAX_AMOUNT_OF_MEASUREMENTS;

		ProfileMeasurement* current = m_frameHistory[m_currentIndex];

		if(current != nullptr)
		{
			delete current;
			current = nullptr;
		}

		m_frameHistory[m_currentIndex] = m_activeNode;

		

		Pop();

		// not null - someone forgot to pop
		//GUARANTEE_OR_DIE(m_activeNode == nullptr, "You didn't pop");
	}

	Push("frame");
}

void Profiler::Push(std::string id)
{
	if(m_isPaused == false)
	{
		ProfileMeasurement* measurement = new ProfileMeasurement(id);

		if(m_activeNode == nullptr)
		{
			m_activeNode = measurement;
		}
		else
		{
			measurement->m_parent = m_activeNode;
			m_activeNode->AddChild( measurement );
			m_activeNode = measurement;
		}
	}
	
}

void Profiler::Pop()
{
	if(m_isPaused == false)
	{
		GUARANTEE_OR_DIE( m_activeNode != nullptr, "Someone called pop without a push");

		m_activeNode->Finish();
		m_activeNode = m_activeNode->m_parent;
	}
}


ProfileMeasurement* Profiler::ProfileGetPreviousFrame(int skip_count)
{
	int desiredIndex = (m_currentIndex + MAX_AMOUNT_OF_MEASUREMENTS - skip_count) % MAX_AMOUNT_OF_MEASUREMENTS;

	if(desiredIndex >= 0)
	{
		return (m_frameHistory[desiredIndex] != nullptr) ? m_frameHistory[desiredIndex] : nullptr;
	}

	return nullptr;

}

#else

//====================================================================================
//									NOTES:
//____________________________________________________________________________________
// Look, If you make a function you need to make an empty one here cause 
// link errors	
//====================================================================================

ProfileMeasurement::ProfileMeasurement(std::string id) {}
ProfileMeasurement::ProfileMeasurement() {}
void ProfileMeasurement::Finish() {}
double ProfileMeasurement::GetElapsedTime() { return 0.0; }
double ProfileMeasurement::GetRootTotalTime() { return 0.0; }
ProfilerScope::ProfilerScope(std::string id) {}
ProfilerScope::~ProfilerScope() {}
Profiler::Profiler() {}
void Profiler::MarkFrame() {}
void Profiler::Push(std::string id) {}
void Profiler::Pop() {}
std::vector<double> Profiler::GetFrameLengths() { return std::vector<double>(); }
double Profiler::GetLongestFrame() { return 0.f; }
ProfileMeasurement * Profiler::ProfileGetPreviousFrame(int skip_count) { return nullptr; }
double ProfileMeasurement::GetTimeFromChildren() { return 0.0f; }
void PauseProfiler(Command& theCommand) {}
void ResumeProfiler(Command& theCommand) {}

#endif

// this is the only function that isn't in the define
Profiler* Profiler::GetInstance()
{
	// lazy instantiating 
	if(s_instance == nullptr)
	{
		s_instance = new Profiler();

		// this is so the console cant try and call the commands when it doesn't work
#ifdef PROFILING_ENABLED
		CommandRegister("profilerPause", "", "Pauses Profiler", PauseProfiler);
		CommandRegister("profilerResume", "", "Resume Profiler", ResumeProfiler);
		CommandRegister("profilerReport", "", "Print Last Frame Report (tree or flat)", PrintFrameToConsole);
		CommandRegister("profiler", "", "Show Profiler", ShowOrHideProfiler);
#endif // PROFILING_ENABLED

	}

	return s_instance;
}

