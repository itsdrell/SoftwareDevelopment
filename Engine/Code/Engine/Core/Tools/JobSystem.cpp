#include "JobSystem.hpp"

//===============================================================================================
JobSystem* JobSystem::s_theJobSystem = nullptr;

//===============================================================================================
Job::Job(Byte jobChannel, int jobType )
{
	static int currentID = 0;

	m_jobID = currentID;
	m_jobType = jobType;
	m_jobChannels = jobChannel;

	currentID++;
}

//===============================================================================================
JobSystem::JobSystem()
{
}

//-----------------------------------------------------------------------------------------------
JobSystem::~JobSystem()
{
	// make sure all threads are done with the jobs before deleting the queue
	Destroy();


}

//-----------------------------------------------------------------------------------------------
JobSystem* JobSystem::CreateOrGet()
{
	if (s_theJobSystem == nullptr)
	{
		s_theJobSystem = new JobSystem();	
	}
	
	return s_theJobSystem;
}

//-----------------------------------------------------------------------------------------------
void JobSystem::Destroy()
{
	std::vector<JobWorkerThread*>& threads = s_theJobSystem->m_workerThreads;
	
	for (uint i = 0; i < threads.size(); i++)
	{
		threads.at(i)->m_isDone = true;
	}

	// stall and wait for them to all return they are done
	while (threads.size() != 0)
	{
		for (uint i = 0; i < threads.size(); i++)
		{
			if (threads.at(i)->m_threadHasDestroyed)
			{
				RemoveFast(i, threads);
			}
		}
	}

	threads.clear();
}

//-----------------------------------------------------------------------------------------------
void JobSystem::CreateWorkerThread(const char * uniqueName, Byte workerJobChannels)
{
	JobWorkerThread* newThread = new JobWorkerThread(uniqueName, workerJobChannels);
	m_workerThreads.push_back(newThread);
}

//-----------------------------------------------------------------------------------------------
void JobSystem::DestroyWorkerThread(const String& uniqueName)
{
	for (uint i = 0; i < m_workerThreads.size(); i++)
	{
		JobWorkerThread* currentThread = m_workerThreads.at(i);

		if (currentThread->m_name == uniqueName)
		{
			currentThread->m_isDone = true;
			return;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void JobSystem::QueueJob(Job * job)
{
	m_queuedJobs.enqueue(job);
}

//-----------------------------------------------------------------------------------------------
JobStatus JobSystem::GetJobStatus(int jobID) const
{
	return JobStatus();
}

//-----------------------------------------------------------------------------------------------
bool JobSystem::IsJobComplete(int jobID) const
{
	return false;
}

//-----------------------------------------------------------------------------------------------
bool JobSystem::AreAllJobsOfTypeComplete(int jobType)
{
	return false;
}

//-----------------------------------------------------------------------------------------------
int JobSystem::GetNumQueuedJobsOfType(int jobType)
{	
	m_queuedJobs.Lock();

	int counter = 0;
	for (uint i = 0; i < m_queuedJobs.Size(); i++)
	{
		Job* currentJob = m_queuedJobs.At(i);

		if (currentJob->m_jobType == jobType)
			counter++;
	}
	
	m_queuedJobs.Unlock();
	
	return counter;
}

//-----------------------------------------------------------------------------------------------
int JobSystem::GetNumRunningJobsOfType(int jobType) 
{
	m_runningJobs.Lock();

	int counter = 0;
	for (uint i = 0; i < m_runningJobs.Size(); i++)
	{
		Job* currentJob = m_runningJobs.At(i);

		if (currentJob->m_jobType == jobType)
			counter++;
	}

	m_runningJobs.Unlock();

	return counter;
}

//-----------------------------------------------------------------------------------------------
int JobSystem::GetNumCompletedJobsOfType(int jobType)
{
	m_finishedJobs.Lock();

	int counter = 0;
	for (uint i = 0; i < m_finishedJobs.Size(); i++)
	{
		Job* currentJob = m_finishedJobs.At(i);

		if (currentJob->m_jobType == jobType)
			counter++;
	}

	m_finishedJobs.Unlock();

	return counter;
}

//-----------------------------------------------------------------------------------------------
int JobSystem::GetNumJobsOfType(int jobType)
{
	int count = 0;
	count += GetNumQueuedJobsOfType(jobType);
	count += GetNumRunningJobsOfType(jobType);
	count += GetNumCompletedJobsOfType(jobType);
	return count;
}

//-----------------------------------------------------------------------------------------------
void JobSystem::FinishCompletedJobs()
{
}

//-----------------------------------------------------------------------------------------------
void JobSystem::FinishJob(int jobID)
{
}

//-----------------------------------------------------------------------------------------------
void JobSystem::FinishAllJobsOfType(int jobType)
{
}

//-----------------------------------------------------------------------------------------------
Job* JobSystem::GetAJob(Byte jobChannels)
{
	
	
	return nullptr;
}

//-----------------------------------------------------------------------------------------------
void JobSystem::FinishJob(Job* finishedJob)
{
	// remove from doing list and put in finished list
}


