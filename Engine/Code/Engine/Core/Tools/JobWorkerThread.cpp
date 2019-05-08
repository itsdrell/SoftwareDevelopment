#include "JobWorkerThread.hpp"
#include "../../Async/Threading.hpp"
#include "JobSystem.hpp"

//===============================================================================================
JobWorkerThread::JobWorkerThread(const char * uniqueName, Byte workerJobChannels)
{
	m_name = std::string(uniqueName);
	m_workerJobChannels = workerJobChannels;
	
	ThreadCreate(m_name, WorkerThreadLoop, this);
	ThreadDetach(m_name);
}

//-----------------------------------------------------------------------------------------------
Job* JobWorkerThread::GetJobFromJobSystem()
{
	JobSystem* js = JobSystem::CreateOrGet();
	return 	js->GetAJob(m_workerJobChannels);
}

//-----------------------------------------------------------------------------------------------
void JobWorkerThread::FinishJob(Job * theJob)
{
	JobSystem* js = JobSystem::CreateOrGet();
	js->FinishJob(theJob);
}

//===============================================================================================
void WorkerThreadLoop(void* userData)
{
	JobSystem* js = JobSystem::CreateOrGet();

	JobWorkerThread* thisThread = (JobWorkerThread*)userData;

	// the is done is if we want to destroy the thread, itll finish the job and then be done
	while ((js->m_isRunning) && (thisThread->m_isDone == false))
	{
		Job* newJob = thisThread->GetJobFromJobSystem();

		if (newJob != nullptr)
		{
			newJob->Execute();
			thisThread->FinishJob(newJob);
		}
		
		ThreadSleep(1);
	}

	// the thread will be destroyed since I created them as detached, this is to let the event system know
	thisThread->m_threadHasDestroyed = true;
}