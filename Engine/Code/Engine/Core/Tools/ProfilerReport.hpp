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
	ProfilerReportEntry(std::string id);

	~ProfilerReportEntry();

	void PopulateTree( ProfileMeasurement* node ) ;

	void AccumulateData( ProfileMeasurement* node );

	void PopulateFlat( ProfileMeasurement* node );

	Strings GenerateReportForFrame();


	ProfilerReportEntry* GetOrCreateChild( char const *str, bool addIndex = false);
	ProfilerReportEntry* FindEntry(const char* str);

public:
	std::string					m_id; 
	uint						m_call_count; 
	double						m_total_time; // inclusive time; 
	double						m_self_time;  // exclusive time
	double						m_percent_time;

	int							m_indentAmount;

	// if you care about new piece data - add time; 
	// mean
	// median
	// high water
	// low water

	ProfilerReportEntry *m_parent = nullptr; 
	std::map<std::string,ProfilerReportEntry*> m_children; 
};

//====================================================================================
class ProfilerReport 
{
public:
	
	ProfilerReport() {}
	~ProfilerReport();
	
	void GenerateReportTreeFromFrame( ProfileMeasurement* root );
	void GenerateReportFlatFromFrame( ProfileMeasurement* root );

	Strings GenerateReportText();

	void SortBySelfTime();

	void SortByTotalTime();

	double GetTotalFrameTime();

public:
	ProfilerReportEntry *m_root = nullptr; 
};

//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================
void PrintFrameToConsole(Command& theCommand);

//====================================================================================
// Written by Zachary Bracken : [7/4/2018]
//====================================================================================