#pragma once
#include "..\Command.hpp"
#include "Engine\Math\Geometry\AABB2.hpp"

//====================================================================================
// Forward Declare
//====================================================================================


//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================
enum PROFILER_HISTORY_VIEW_TYPE
{
	PROFILER_MAP_VIEW,
	PROFILER_TREE_TOTAL_VIEW,
	PROFILER_TREE_SELF_VIEW
};

//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class ProfilerUI
{
public:
	ProfilerUI();
	~ProfilerUI();

	static ProfilerUI* GetInstance();

	void Update();
	void HandleInput();

	void Render() const;
	void RenderBackgrounds() const;
	void RenderFPS() const;
	void RenderGraph() const;
	void RenderHelper() const;
	void RenderTextGraph() const;


	void Open();
	void Close();
	void Toggle();

	bool IsOpen() { return m_isOpen; }

public:
	bool		m_isOpen;
	bool		m_hasControlOfInput;
	bool		m_mouseHidden;


private:

	PROFILER_HISTORY_VIEW_TYPE		m_viewType;

	AABB2							m_textBox;
	AABB2							m_fpsBox;
	AABB2							m_graphBox;
	AABB2							m_helpCommandsBox;
};

//====================================================================================
// Standalone C Functions
//====================================================================================
void ShowOrHideProfiler(Command& theCommand);

//====================================================================================
// Externs
//====================================================================================
extern ProfilerUI* g_theProfilerUI;

//====================================================================================
// Written by Zachary Bracken : [7/4/2018]
//====================================================================================