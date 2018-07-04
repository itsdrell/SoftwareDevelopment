#pragma once
#include "Profiler.hpp"

//====================================================================================
// Forward Declare
//====================================================================================


//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class ProfilerReportEntry 
{
public:
	void PopulateTree( ProfileMeasurement* node ) ;

	void AccumulateData( ProfileMeasurement* node );

	void PopulateFlat( ProfileMeasurement* node );


	ProfilerReportEntry* GetOrCreateChild( char const *str );

public:
	std::string					m_id; 
	uint						m_call_count; 
	double						m_total_time; // inclusive time; 
	double						m_self_time;  // exclusive time
	double						m_percent_time;

	// if you care about new piece data - add time; 
	// mean
	// median
	// high water
	// low water

	ProfilerReportEntry *m_parent; 
	std::map<std::string,ProfilerReportEntry*> m_children; 
};

//====================================================================================
class ProfilerReport 
{
public:
	void GenerateReportTreeFromFrame( ProfileMeasurement* root );

	void SortBySelfTime();

	void SortByTotalTime();

	double GetTotalFrameTime();

public:
	ProfilerReportEntry *m_root; 
};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [7/4/2018]
//====================================================================================