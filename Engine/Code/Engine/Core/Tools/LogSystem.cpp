#include "LogSystem.hpp"
#include "Engine\Core\General\EngineCommon.hpp"
#include "Engine\Async\Threading.hpp"
#include "Engine\Core\Platform\File.hpp"
#include "Engine\Core\Utils\StringUtils.hpp"
#include <iostream>
#include <fstream>
#include <stdarg.h> 
#include "..\Platform\Time.hpp"


//====================================================================================
LogSystem* g_LogSystem = nullptr;

std::vector<LogHook>		LogSystem::s_callbacks;
//====================================================================================
LogSystem* LogSystem::GetInstance()
{
	if(g_LogSystem == nullptr)
	{
		g_LogSystem = new LogSystem();
	}

	return g_LogSystem;
}
//--------------------------------------------------------------------------
void LogSystem::StartUp()
{
	ThreadCreate(LOG_THREAD_NAME, LogThreadWorker, nullptr);

	m_outputFile.open(LOG_FILE_PATH, std::fstream::out | std::fstream::trunc);
	
	std::string historyPath = LOG_HISTORY_PATH + CurrentDateTime() + ".log";
	m_historyFile.open(historyPath, std::fstream::out | std::fstream::trunc);
}
//--------------------------------------------------------------------------
void LogSystem::ShutDown()
{
	m_is_running = false;
	ThreadJoin(LOG_THREAD_NAME);

	m_outputFile.close();
	m_historyFile.close();
}

//--------------------------------------------------------------------------
void LogSystem::Flush()
{
	Log* data = nullptr; 
	while (m_log_queue.dequeue(&data)) 
	{
		// do something with the data
		if(data != nullptr)
		{
			RunCallbacks(*data);

			delete data; 
		}
		
	}
}

//--------------------------------------------------------------------------
STATIC void LogSystem::LogThreadWorker(void * data)
{
	while (g_LogSystem->IsRunning()) 
	{
		g_LogSystem->Flush(); 
		ThreadSleep(10);  
	}

	// do the inner side of the loop again right before exiting
	g_LogSystem->Flush(); 

}

void LogFlush()
{

}

//--------------------------------------------------------------------------
STATIC bool LogSystem::CheckIfAlreadyRegistered(log_cb cb)
{
	for(uint i = 0; i < s_callbacks.size(); i++)
	{
		log_cb current = s_callbacks.at(i).callback;

		if(current == cb)
			return true;
	}

	return false;
}

//--------------------------------------------------------------------------
STATIC void LogSystem::AddHook( log_cb cb, void* userArgs )
{
	g_LogSystem->m_hookLock.Enter();
	
	if(CheckIfAlreadyRegistered(cb) == false)
		g_LogSystem->s_callbacks.push_back( LogHook(cb, userArgs));

	g_LogSystem->m_hookLock.Leave();
}

//--------------------------------------------------------------------------
STATIC void LogSystem::RemoveHook(log_cb cb)
{
	for(uint i = 0; i < s_callbacks.size(); i++)
	{
		log_cb current = s_callbacks.at(i).callback;

		if(current == cb)
		{
			s_callbacks.erase(s_callbacks.begin() + i);
			return;
		}
	}
}

//--------------------------------------------------------------------------
STATIC void LogSystem::RunCallbacks(const Log & data)
{
	for(uint i = 0; i < s_callbacks.size(); i++)
	{
		LogHook current = s_callbacks.at(i);

		current.callback(data, current.user_argument);
	}
}


//====================================================================================
void LogSystemStartUp()
{
	LogSystem* ls = LogSystem::GetInstance();
	ls->StartUp();

	ls->AddHook((log_cb) LogToFile, nullptr);

}

//--------------------------------------------------------------------------
void LogSystemShutDown()
{
	LogSystem* ls = LogSystem::GetInstance();
	ls->ShutDown();
}

//--------------------------------------------------------------------------
void LogTaggedPrintv(const char* tag, const char* format, va_list args)
{
	
	Log* log = new Log(); 
	log->tag = tag; 
	log->text = Stringf( format, args );

	g_LogSystem->m_log_queue.enqueue(log); 
}

//--------------------------------------------------------------------------
void LogTaggedPrintf(const char* tag, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogTaggedPrintv(tag , format, args);
	va_end(args);
}

//--------------------------------------------------------------------------
void LogPrintf(char const *format, ...)
{
	va_list args;
	va_start(args, format);
	LogTaggedPrintv("LOG", format, args);
	va_end(args);

}

//--------------------------------------------------------------------------
void LogWarning(const char * format, ...)
{
	va_list args;
	va_start(args, format);
	LogTaggedPrintv("WARNING", format, args);
	va_end(args);
}

//--------------------------------------------------------------------------
void LogError(const char * format, ...)
{
	va_list args;
	va_start(args, format);
	LogTaggedPrintv("ERROR", format, args);
	va_end(args);
}

//--------------------------------------------------------------------------
void LogToFile(const Log& data)
{
	LogSystem::GetInstance()->m_outputFile << (data.tag + " : " + data.text + "\n");
	LogSystem::GetInstance()->m_historyFile << (data.tag + " : " + data.text + "\n");
}
