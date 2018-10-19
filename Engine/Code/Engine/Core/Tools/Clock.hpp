#pragma once
#include "Engine/Core/General/EngineCommon.hpp"

//------------------------------------------------------------------------
//------------------------------------------------------------------------

struct time_unit_t 
{
	time_unit_t()
	{
		memset(this,0,sizeof(time_unit_t)); //gag
	}
	
	uint64_t	hpc; 
	double		hpcSeconds;  // high precision seconds 
	uint		ms;
	
	float		seconds;      // convenience float seconds 
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
class Clock 
{
public:
	Clock( Clock *parent = nullptr );
	~Clock(); // make sure to cleanup the heirachy

	// resets the clock - everything should be zeros out
	// and the reference hpc variables set 
	// to the current hpc.
	void Reset(); 

	// used on a root clock
	// calculates elapsed time since last call
	// and calls advance.
	void BeginFrame(); 

	// advance the clock by the given hpc
	void Advance( uint64_t elapsed ); 

	// adds a clock as a child to this clock
	void AddChild( Clock *child ); 

	// returns time since this clock 
	// using the current hpc, and our start hpc
	// to get the most up-to-date time.
	double GetCurrentTime() const; 

	uint GetTotalFrameCount() { return m_frameCount; }

	// manipulation
	void SetPaused( bool paused ); 
	void TogglePaused() {m_paused = !m_paused;}
	void SetScale( float scale ); 
	void IncreaseScale(float scaleToAdd = .5f);
	void DescreaseScale(float scaleToSubtract = .5f);
	void StepForwardAFrame(float deltaToMove = .03f);

	// add whatever other convenience methods you may want; 
	// ...

public:
	// I don't use "m_" here as 
	// the intended usage is to be able to go 
	// clock->total.seconds -or- clock->frame.seconds; 
	// these are only updated at begin_frame.
	time_unit_t			frame; 
	time_unit_t			total; 
	
	// for game clock so i can just say g_theGameClock.deltaTime like unity
	float				deltaTime; 
	float				totalTime;
	
	bool				m_debugMode;

private:
	// local data we need to track for the clock to work; 
	uint64_t			m_startHpc;      // hpc when the clock was last reset (made)
	uint64_t			m_lastFrameHpc; // hpc during last begin_frame call

	double				m_timeScale; 
	uint				m_frameCount;        // if you're interested in number of frames your game has processes
	bool				m_paused;  

	// For the hierarchy 
	Clock *m_parent;
	std::vector<Clock*> m_children; 
}; 


//------------------------------------------------------------------------
// C Funtions
//------------------------------------------------------------------------
// could calso be static Clock* Clock::GetMaster(); 
Clock* GetMasterClock(); 

// convenience - calls begin frame on the master clock
void ClockSystemBeginFrame();

// I now move this here - as this now refers to the master clock
// who is keeping track of the starting reference point. 
double GetTheCurrentTime(); 

// I usually also add convenience functions so I'm not constantly fetching the master clock, like
float GetDeltaTime();
//float GetFrameStartTime();

float GetFPS();

uint GetTimeInMilliseconds();

// Declaring all main clocks here
extern Clock* g_theMasterClock;
extern Clock* g_theGameClock;