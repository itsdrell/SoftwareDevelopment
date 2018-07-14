#pragma once
#include "Engine/Async/ThreadSafeContainers.hpp"
#include "Engine/Core/Tools/Command.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

//====================================================================================
// Forward Declare
//====================================================================================


//====================================================================================
// Type Defs + Defines
//====================================================================================
#define LOG_THREAD_NAME ("logThread")
#define LOG_FILE_PATH ("Log/Log.log")
#define LOG_HISTORY_PATH ("Log/History/") // this is for our timestamped history

//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================
struct Log
{
	std::string tag; 
	std::string text; 
};

typedef void (*log_cb)( const Log &log, void* args); 

struct LogHook
{
	LogHook( log_cb cb, void* userArgs)
		: callback(cb)
		, user_argument(userArgs) {}
	
	log_cb		callback;
	void*		user_argument = nullptr;
};


//====================================================================================
// Classes
//====================================================================================
class LogSystem
{

public:
	LogSystem() {}

	static LogSystem* GetInstance();

	void StartUp();
	void ShutDown();
	bool IsRunning() { return m_is_running; }
	bool Stop() { m_is_running = false; }

	void Flush();
	void ForceFlush();

	void ShowTag(const std::string& tag);
	void HideTag(const std::string& tag);

	static void LogThreadWorker( void* data );
	static bool CheckIfAlreadyRegistered( log_cb cb );
	static void AddHook( log_cb cb, void* userArgs );
	static void RemoveHook( log_cb cb );
	static void RunCallbacks( const Log& data );

private:
	void AddTag(const std::string& tag);
	void RemoveTag(const std::string& tag);

public:

	bool								m_is_running = true;
	bool								m_doneFlushing = false;

	ThreadSafeVector<std::string>		m_tags;
	bool								m_usingFilering = true;
	bool								m_selectionIgnored = true;

	ThreadSafeQueue<Log*>				m_log_queue; 
	SpinLock							m_hookLock;

	std::ofstream							m_outputFile;
	std::ofstream							m_historyFile;

	static std::vector<LogHook>			s_callbacks;
}; 



//====================================================================================
// Standalone C Functions
//====================================================================================
void LogSystemStartUp();
void LogSystemShutDown();

void ForceLogSystemFlush();

// Works like ("tag", "The number is: %i", 10)
void LogTaggedPrintv( const char* tag, const char* format, va_list args);
void LogTaggedPrintf( const char* tag, const char* format, ...);
void LogPrintf( char const *format, ... );
void LogWarning(const char* format, ...);
void LogError(const char* format, ...);

void LogToFile(const Log& data);
void LogToOutputWindow(const Log& theLog);

void ShowTagCommand(Command& cb);
void HideTagCommand(Command& cb);
void ToggleTagMode(Command& cb);
void EnableFiltering(Command& cb);
void ClearTags(Command& cb);

//====================================================================================
// Externs
//====================================================================================
extern LogSystem* g_LogSystem;

//====================================================================================
// Written by Zachary Bracken : [7/11/2018]
//====================================================================================