#include "ScopedProfile.hpp"
#include "..\Platform\Time.hpp"
#include "DevConsole.hpp"
#include "..\Utils\StringUtils.hpp"

ScopedProfile::ScopedProfile(std::string name)
{
	m_name = name;
	m_startTime = GetPerformanceCounter();
}

ScopedProfile::~ScopedProfile()
{
	uint64_t endTime = GetPerformanceCounter();

	uint64_t elapsed = endTime - m_startTime;

	std::string text = Stringf("\nTime elapsed for %s: %f \n", m_name.c_str(), PerformanceCountToSeconds(elapsed));

	DebuggerPrintf(text.c_str());
	DevConsole::GetInstance()->AddConsoleDialogue(ConsoleDialogue(text, GetRandomColorInRainbow()));
}
