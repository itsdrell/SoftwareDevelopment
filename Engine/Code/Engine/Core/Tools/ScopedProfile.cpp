#include "ScopedProfile.hpp"
#include "..\Platform\Time.hpp"

ScopedProfile::ScopedProfile(std::string name)
{
	m_name = name;
	m_startTime = GetPerformanceCounter();
}

ScopedProfile::~ScopedProfile()
{
	uint64_t endTime = GetPerformanceCounter();

	uint64_t elapsed = endTime - m_startTime;

	DebuggerPrintf("\n Time elapsed for %s: %f \n", m_name.c_str(), PerformanceCountToSeconds(elapsed));
}
