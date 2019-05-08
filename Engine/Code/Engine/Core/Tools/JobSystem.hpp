#pragma once
#include "Engine/Core/General/EngineCommon.hpp"
#include "Engine/Core/Tools/JobWorkerThread.hpp"
#include "../../Async/ThreadSafeContainers.hpp"

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
	Job(Byte jobChannels = DEFAULT_JOB_FLAG, int jobType = DEFAULT_JOB_TYPE);
	virtual ~Job() {}

	virtual void	Execute() = 0;
	virtual void	JobCompleteCallback() {}
	int				GetUniqueID() const { return m_jobID; }

public:
	int				m_jobID = -1; // Unique to each Job instance
	int				m_jobType = DEFAULT_JOB_TYPE; // User-assigned, to categorize or identify type of job
	Byte			m_jobChannels = DEFAULT_JOB_FLAG; // Flags must overlap with worker thread to run on it (was uint32)
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
	void				DestroyWorkerThread(const String& uniqueName);
	void				QueueJob(Job* job);

public:
	// Status queries
	JobStatus			GetJobStatus(int jobID) const;
	bool				IsJobComplete(int jobID) const;
	bool				AreAllJobsOfTypeComplete(int jobType);
	int					GetNumQueuedJobsOfType(int jobType);
	int					GetNumRunningJobsOfType(int jobType);
	int					GetNumCompletedJobsOfType(int jobType);
	int					GetNumJobsOfType(int jobType); // sum of Queued, Running, and Completed

public:
	void				FinishCompletedJobs();	// Calls callbacks for already-completed jobs
	void				FinishJob(int jobID);	// Blocks until job instance is callback-complete
	void				FinishAllJobsOfType(int jobType); // Blocks until all jobs of type complete

protected:
	Job*				GetAJob(Byte jobChannels = DEFAULT_JOB_FLAG);
	void				FinishJob(Job* finishedJob);

private:
	static JobSystem*	s_theJobSystem;

public:
	bool				m_isRunning = true;

protected:
	//...as well as whatever internal functions JobSystem or JobWorkerThreads need to use...
	//	...and internal data(e.g.Queued / Running / Completed lists, list of JobWorkerThreads, etc.)...
	std::vector<JobWorkerThread*>		m_workerThreads;
	
	ThreadSafeQueue<Job*>				m_queuedJobs;
	ThreadSafeVector<Job*>				m_runningJobs;
	ThreadSafeVector<Job*>				m_finishedJobs;
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