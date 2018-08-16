#define WIN32_LEAN_AND_MEAN		
#include <windows.h>	
#include "Threading.hpp"


//====================================================================================
std::vector<Thread*>	 Thread::s_threads;
//====================================================================================

#pragma pack(push, 8)
struct THREADNAME_INFO
{
	DWORD type;            // must be 0x1000
	const char *name;      // name
	DWORD thread_id;      // -1 for calling thread
	DWORD flags;         // must be 0, reserved for future use
};
#pragma pack(pop)

//====================================================================================
Thread::Thread(String name, thread_cb cb, void* user_data)
{
	m_name = name;
	m_thread = std::thread(cb, user_data);
	m_threadID = m_thread.get_id();
	
	//THREADNAME_INFO info;
	//info.type = 0x1000;
	//info.name = m_name.c_str();
	//info.thread_id =  ::GetThreadId( static_cast<HANDLE>( m_thread.native_handle() ) );
	//info.flags = 0;
	//RaiseException( MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)(&info) );


	ThreadSetName(m_name.c_str(), m_thread.native_handle());

	s_threads.push_back(this);
}


void Thread::RemoveThread(String name)
{
	std::vector<Thread*>::iterator threadIterator;
	for( threadIterator = Thread::s_threads.begin(); threadIterator != Thread::s_threads.end(); threadIterator++)
	{
		Thread* current = *threadIterator;
		String currentName = current->m_name;

		if(currentName == name)
		{
			delete current;
			Thread::s_threads.erase(threadIterator);
		}
	}
}

//--------------------------------------------------------------------------
// This is called on app shutdown to clean up all threads
void Thread::Shutdown()
{
	std::vector<Thread*>	::iterator threadIterator;
	for( threadIterator = Thread::s_threads.begin(); threadIterator != Thread::s_threads.end(); threadIterator++)
	{
		Thread* current = *threadIterator;
		String currentName = current->m_name;

		current->m_thread.join();
		delete current;
		current = nullptr;
	}

	Thread::s_threads.clear();
}

//====================================================================================
void ThreadCreate(String name, thread_cb cb, void* user_data)
{
	new Thread(name, cb, user_data);
}

void ThreadJoin( String name )
{
	// find the thread
	std::vector<Thread*>	::iterator threadIterator;
	for( threadIterator = Thread::s_threads.begin(); threadIterator != Thread::s_threads.end(); threadIterator++)
	{
		Thread* current = *threadIterator;
		String currentName = current->m_name;

		if(currentName == name)
		{
			current->m_thread.join();
			delete current;
			Thread::s_threads.erase(threadIterator);
			return;
		}
	}
}

void ThreadDetach( String name )
{
	// find the thread
	std::vector<Thread*>	::iterator threadIterator;
	for( threadIterator = Thread::s_threads.begin(); threadIterator != Thread::s_threads.end(); threadIterator++)
	{
		Thread* current = *threadIterator;
		String currentName = current->m_name;

		if(currentName == name)
		{
			current->m_thread.detach();
			delete current;
			Thread::s_threads.erase(threadIterator);
		}
	}
}

void ThreadCreateAndDetach(thread_cb cb, void * user_data)
{
	std::thread newThread = std::thread(cb, user_data);
	newThread.detach();
}

void ThreadSleep(uint ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void ThreadYield()
{
	std::this_thread::yield();
}

void ThreadSetName( char const *name, void* ID )
{
	if (nullptr == name) {
		return;
	}

	THREADNAME_INFO info;
	info.type = 0x1000;
	info.name = name;
	info.thread_id =  ::GetThreadId( static_cast<HANDLE>( ID ) );
	info.flags = 0;

	__try 
	{
		RaiseException( MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)(&info) );
	} 
	__except (EXCEPTION_CONTINUE_EXECUTION) 
	{
	}
}
