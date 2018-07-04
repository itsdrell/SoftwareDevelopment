#include "Profiler.hpp"
#include "..\Platform\Time.hpp"
#include "Game\Main\EngineBuildPreferences.hpp"


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
	
	Profiler::GetInstance()->m_isPaused = false;
}


//====================================================================================
ProfileMeasurement::ProfileMeasurement(std::string id)
{
	m_startHPC = GetPerformanceCounter();
	m_id = id;

}

void ProfileMeasurement::Finish()
{
	m_endHPC = GetPerformanceCounter();
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

void Profiler::MarkFrame()
{
	if(m_isPausing)
	{
		m_isPaused = true;
		m_isPausing = false;
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

		//m_prevFrame = m_activeNode;
		//m_frameHistory.push(m_activeNode);
		Pop();

		// not null - someone forgot to pop
		GUARANTEE_OR_DIE(m_activeNode == nullptr, "You didn't pop");
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

#else

//====================================================================================
//									NOTES:
//____________________________________________________________________________________
// Look, If you make a function you need to make an empty one here cause 
// link errors	
//====================================================================================

ProfileMeasurement::ProfileMeasurement(std::string id) {}
void ProfileMeasurement::Finish() {}
ProfilerScope::ProfilerScope(std::string id) {}
ProfilerScope::~ProfilerScope() {}
Profiler::Profiler() {}
void Profiler::MarkFrame() {}
void Profiler::Push(std::string id) {}
void Profiler::Pop() {}

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

		CommandRegister("profilerPause", "", "Pauses Profiler", PauseProfiler);
		CommandRegister("profilerResume", "", "Resume Profiler", ResumeProfiler);
	}

	return s_instance;
}