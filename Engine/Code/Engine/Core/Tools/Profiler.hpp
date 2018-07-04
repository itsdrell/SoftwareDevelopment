#pragma once
#include "Engine/Core/General/EngineCommon.hpp"
#include <queue>
#include "Command.hpp"



//====================================================================================
// Forward Declare
//====================================================================================
class Profiler;

//====================================================================================
// Type Defs + Defines
//====================================================================================
#define MAX_AMOUNT_OF_MEASUREMENTS (128)

#define MARK_FRAME Profiler::GetInstance()->MarkFrame()

#define PROFILE_PUSH() ProfilerScope __timer__ ## __LINE__ ## (__FUNCTION__)


//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================
struct ProfileMeasurement
{
	ProfileMeasurement(std::string id);
	
	void Finish();
	void AddChild(ProfileMeasurement* newChild) { m_children.push_back(newChild); }

	//--------------------------------------------------------------------------
	std::string									m_id;
	uint64_t									m_startHPC;
	uint64_t									m_endHPC;

	std::vector<ProfileMeasurement*>			m_children;
	ProfileMeasurement*							m_parent = nullptr;
	
	//ProfileMeasurement*							m_prev = nullptr;
	//ProfileMeasurement*							m_next = nullptr;

};


struct ProfilerScope
{
public:

	ProfilerScope(std::string id);

	~ProfilerScope();

};

//====================================================================================
// Classes
//====================================================================================
class Profiler
{
public:
	Profiler();

	static Profiler* GetInstance();

	void MarkFrame();

	void Push(std::string id);
	void Pop();

public:
	ProfileMeasurement*									m_activeNode = nullptr;

	ProfileMeasurement*									m_frameHistory[MAX_AMOUNT_OF_MEASUREMENTS];
	int													m_currentIndex;

	bool												m_isPausing;
	bool												m_isPaused;

	static Profiler*									s_instance;
};

//====================================================================================
// Standalone C Functions
//====================================================================================
void PauseProfiler(Command& theCommand);
void ResumeProfiler(Command& theCommand);

//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [7/3/2018]
//====================================================================================