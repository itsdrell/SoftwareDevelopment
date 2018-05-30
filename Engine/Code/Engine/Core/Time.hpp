#pragma once
#include <stdint.h>

//-----------------------------------------------------------------------------------------------
// Time.hpp
//
#pragma once
#include <string>

//-----------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds();

const std::string CurrentDateTime();

// Gets raw performance counter
uint64_t GetPerformanceCounter(); 

// converts a performance count the seconds it represents
double PerformanceCountToSeconds( uint64_t seconds ); 