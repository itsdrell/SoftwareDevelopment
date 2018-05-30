#include "Stopwatch.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"

StopWatch::StopWatch(Clock* ref_clock)
{
	if(ref_clock == nullptr)
	{
		m_reference = g_theMasterClock;
	}
	else
	{
		m_reference = ref_clock;
	}
	
	//m_startHpc = GetPerformanceCounter();
	m_startTime = m_reference->total.seconds;
	
}

void StopWatch::SetClock(Clock* ref_clock)
{
	m_reference = ref_clock;
}

bool StopWatch::SetTimer(float seconds)
{
	// idk why this is a bool, maybe to see if its already running?
	
	m_startTime = m_reference->total.seconds; 
	m_length = seconds;
	return true;
}

float StopWatch::GetElapsedTime()
{
	return (m_startTime + m_length) - m_reference->total.seconds;
}

float StopWatch::GetNormalizedElapsedTime()
{
	float percentDone = GetElapsedTime() / (float) m_length;
	
	return percentDone;
}

bool StopWatch::HasElapsed()
{
	
	uint64_t endTime = m_startTime + m_length;

	if(m_reference->total.seconds > endTime)
	{
		return true;
	}
	
	return false;
}

void StopWatch::Reset()
{
	m_startTime = m_reference->total.seconds;
}

bool StopWatch::CheckAndReset()
{
	if(HasElapsed())
	{
		Reset();
		return true;		
	}	
	
	return false;
}

// I have no idea what these do or why I really need them
bool StopWatch::Decrement()
{
	return HasElapsed();
}

uint StopWatch::DecrementAll()
{
	return uint();
}
