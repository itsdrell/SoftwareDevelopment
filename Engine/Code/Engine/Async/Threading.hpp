#pragma once
#include "Engine/Core/General/EngineCommon.hpp"
#include <thread>
#include <vector>

// I'm looking for a file *like* this, not does not need to be
// exact.  Should have the basic thread functions in an easy to 
// find structure. 
//
// Even if you prefer class based Threads, provide the C functions,
// as the class based ones will use them anyway;
//
// HOW you implement them under the hood I don't care
// I use Win32 Functions, but std::thread is fine.  
// DO NOT USE BOOST!

/************************************************************************/
/* TYPES                                                                */
/************************************************************************/

#define MS_VC_EXCEPTION (0x406d1388)

// an example of a type-safe pointer type.  We never
// need to define thread_handle_type_t, as it is never actually used,
// just the pointer is; 
struct thread_handle_type_t;
typedef thread_handle_type_t* thread_handle_t; 


typedef std::thread::id ThreadID;



// If the above feels gross, this is another option, but no type safety
// typedef void* thread_handle_t;

// callback type - this is a very general purpose 
// C function.  Even more advanced types will usually have a function
// like this as their initial filter; 
typedef void (*thread_cb)( void *user_data ); 



//====================================================================================
class Thread
{
public:

	Thread(String name,thread_cb cb, void* user_data);
	
	static void RemoveThread( String name );
	static void Shutdown();

public:

	String									m_name;
	std::thread								m_thread;
	ThreadID								m_threadID;

	static std::vector<Thread*>				s_threads;
};


/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/

// Creating
void ThreadCreate(String name , thread_cb cb, void *user_data = nullptr); 

// Releasing - how we free an above resource
// Join will wait until the thread is complete before return control to the calling thread
void ThreadJoin( String name ); 
// Detach will return control immediately, and the thread will be freed automatically when complete;
void ThreadDetach( String name ); 

// This creates a thread and does not log it to our Thread map
void ThreadCreateAndDetach( thread_cb cb, void *user_data = nullptr ); 

// Control
void ThreadSleep( uint ms ); 
void ThreadYield(); 

void ThreadSetName( char const *name, void* ID );

// Just so you're aware - but do not implement yet
// These control how the OS treats these threads.
//
// CoreAffinity -> Which Processor/Core do we prefer to run this thread on;
// void ThreadSetCoreAffinity( uint core_bitfield ); 
// Priority -> How important is it that this thread run (lower means it may be 
// get starved out.  Higher means it could starve others).
// void ThreadSetPriority( uint priority ); 