//-----------------------------------------------------------------------------------------------
// Time.cpp
//	

//-----------------------------------------------------------------------------------------------
#pragma warning(disable:4996) // for localtime
#include "Engine/Core/Platform/Time.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <time.h>
#include "Engine/Core/Utils/StringUtils.hpp"



//------------------------------------------------------------------------
// local to time.cpp (could also use static variables)
class LocalTimeData 
{
public:
	LocalTimeData() 
	{
		::QueryPerformanceFrequency( (LARGE_INTEGER*) &m_hpc_per_second ); 

		// do the divide now, to not pay the cost later
		m_seconds_per_hpc = 1.0 / (double)m_hpc_per_second; 
	}

public:
	uint64_t m_hpc_per_second; 
	double m_seconds_per_hpc; // frequency
}; 

//------------------------------------------------------------------------
// Declaring on the global scope - will 
// cause constructor to be called before 
// our entry function. 
static LocalTimeData gLocalTimeData;



//------------------------------------------------------------------------
// Getting the performance counter
uint64_t GetPerformanceCounter() 
{
	uint64_t hpc;
	::QueryPerformanceCounter( (LARGE_INTEGER*)&hpc ); 
	return hpc; 
}

//------------------------------------------------------------------------
// Converting to seconds; 
// relies on gLocalTimeData existing;
double PerformanceCountToSeconds( uint64_t hpc ) 
{
	return (double) hpc * gLocalTimeData.m_seconds_per_hpc; 
}

//-----------------------------------------------------------------------------------------------
double InitializeTime( LARGE_INTEGER& out_initialTime )
{
	LARGE_INTEGER countsPerSecond;
	QueryPerformanceFrequency( &countsPerSecond );
	QueryPerformanceCounter( &out_initialTime );
	return( 1.0 / static_cast< double >( countsPerSecond.QuadPart ) );
}


//-----------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds()
{
	static LARGE_INTEGER initialTime;
	static double secondsPerCount = InitializeTime( initialTime );
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter( &currentCount );
	LONGLONG elapsedCountsSinceInitialTime = currentCount.QuadPart - initialTime.QuadPart;

	double currentSeconds = static_cast< double >( elapsedCountsSinceInitialTime ) * secondsPerCount;
	return currentSeconds;
}


//////////////////////////////////////////////////////////////////////////
// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
// Source: https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
const std::string CurrentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now); // this gives a warning
	
	// year_month_day_hour-min-sec
	strftime(buf, sizeof(buf), "%Y_%m_%d_%H-%M-%S", &tstruct);

	return buf;
}

