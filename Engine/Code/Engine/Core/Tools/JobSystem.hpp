#pragma once
#include "Engine/Core/General/EngineCommon.hpp"
#include "Engine/Core/Tools/JobWorkerThread.hpp"

//====================================================================================
// Forward Declare
//====================================================================================


//====================================================================================
// Type Defs + Defines
//====================================================================================


//====================================================================================
// ENUMS
//====================================================================================
enum JobStatus
{
	JOB_STATUS_QUED,
	JOB_STATUS_WORKING,
	JOB_STATUS_DONE,
	JOB_DOES_NOT_EXIST
};

//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class Job // abstract base class; derive from Job to create your own custom Job types
{
public:
	Job(Byte jobChannels = 0xffffffff, int jobType = -1);
	virtual ~Job() {}

	virtual void	Execute() = 0;
	virtual void	JobCompleteCallback() {}
	int				GetUniqueID() const { return m_jobID; }

public:
	int				m_jobID = -1; // Unique to each Job instance
	int				m_jobType = -1; // User-assigned, to categorize or identify type of job
	Byte			m_jobChannels = 0xffffffff; // Flags must overlap with worker thread to run on it (was uint32)
};

//-----------------------------------------------------------------------------------------------
class JobSystem
{
	friend class JobWorkerThread; // Allow access to protected methods (e.g. ClaimAJob)

public:
	JobSystem();
	~JobSystem();

public:
	static JobSystem*	CreateOrGet();
	static void			Destroy();

public:
	void				CreateWorkerThread(const char* uniqueName, Byte workerJobChannels = 0xFFFFFFFF);
	void				DestroyWorkerThread(const char* uniqueName);
	void				QueueJob(Job* job);

public:
	// Status queries
	JobStatus			GetJobStatus(int jobID) const;
	bool				IsJobComplete(int jobID) const;
	bool				AreAllJobsOfTypeComplete(int jobType) const;
	int					GetNumQueuedJobsOfType(int jobType) const;
	int					GetNumRunningJobsOfType(int jobType) const;
	int					GetNumCompletedJobsOfType(int jobType) const;
	int					GetNumJobsOfType(int jobType) const; // sum of Queued, Running, and Completed

public:
	void				FinishCompletedJobs();	// Calls callbacks for already-completed jobs
	void				FinishJob(int jobID);	// Blocks until job instance is callback-complete
	void				FinishAllJobsOfType(int jobType); // Blocks until all jobs of type complete

protected:
	//...as well as whatever internal functions JobSystem or JobWorkerThreads need to use...
	//	...and internal data(e.g.Queued / Running / Completed lists, list of JobWorkerThreads, etc.)...
};


//====================================================================================
// Standalone C Functions
//====================================================================================


//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [5/7/2019]
//====================================================================================