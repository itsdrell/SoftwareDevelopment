#include "ScopedProfile.hpp"
#include "..\Platform\Time.hpp"

ScopedProfile::ScopedProfile()
{
	m_startTime = GetPerformanceCounter();
}

ScopedProfile::~ScopedProfile()
{
	uint64_t endTime = GetPerformanceCounter();

	uint64_t elapsed = endTime - m_startTime;

	DebuggerPrintf("\n Time elapsed: %f \n", PerformanceCountToSeconds(elapsed));
}
