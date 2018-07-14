#include "LogSystem.hpp"
#include "Engine\Core\General\EngineCommon.hpp"
#include "Engine\Async\Threading.hpp"
#include "Engine\Core\Platform\File.hpp"
#include "Engine\Core\Utils\StringUtils.hpp"
#include "Engine\Core\Platform\Time.hpp"
#include "Engine\Core\Tools\Command.hpp"
#include "Engine\Core\Tools\DevConsole.hpp"
#include "Engine\Core\Tools\ErrorWarningAssert.hpp"
#include <iostream>
#include <fstream>
#include <stdarg.h> 
#include <debugapi.h>


//====================================================================================
LogSystem* g_LogSystem = nullptr;

std::vector<LogHook>		LogSystem::s_callbacks;
//====================================================================================
// Console
//====================================================================================
void ShowTagCommand(Command& cb)
{
	if(cb.m_commandArguements.size() <= 1)
		return;

	String addTag = cb.m_commandArguements.at(1);

	LogSystem::GetInstance()->ShowTag(addTag);
}

//--------------------------------------------------------------------------
void HideTagCommand(Command& cb)
{
	if(cb.m_commandArguements.size() <= 1)
		return;

	String addTag = cb.m_commandArguements.at(1);

	LogSystem::GetInstance()->HideTag(addTag);
}

//--------------------------------------------------------------------------
void ToggleTagMode(Command& cb)
{
	LogSystem* ls = LogSystem::GetInstance();

	bool toggle = !ls->m_selectionIgnored;

	if(cb.m_commandArguements.size() > 1)
		toggle = ParseString(cb.m_commandArguements.at(1), toggle);

	ls->m_selectionIgnored = toggle;

	DevConsole::GetInstance()->AddConsoleDialogue("Mode switched to: " + std::to_string(toggle));
}

//--------------------------------------------------------------------------
void EnableFiltering(Command& cb)
{
	LogSystem* ls = LogSystem::GetInstance();

	bool toggle = !ls->m_usingFilering;

	if(cb.m_commandArguements.size() > 1)
		toggle = ParseString(cb.m_commandArguements.at(1), toggle);

	ls->m_usingFilering = toggle;

	DevConsole::GetInstance()->AddConsoleDialogue("Filtering is now " + std::to_string(toggle));
}

//--------------------------------------------------------------------------
void ClearTags(Command& cb)
{
	UNUSED(cb);
	
	LogSystem* ls = LogSystem::GetInstance();
	ls->m_tags.Clear();
}

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

	CommandRegister("showTag", "", "", ShowTagCommand);
	CommandRegister("hideTag", "", "", HideTagCommand);
	CommandRegister("toggleTag", "", "Switched between white list and black", ToggleTagMode);
	CommandRegister("toggleFiltering", "", "Switched between using filtering and not", EnableFiltering);
	CommandRegister("clearTags", "", "Clear all tags in log system", ClearTags);


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

	m_doneFlushing = true;
}

//--------------------------------------------------------------------------
void LogSystem::ForceFlush()
{
	m_doneFlushing = false;

	while(!m_doneFlushing) {}

	m_historyFile.flush();
	m_outputFile.flush();
}

//--------------------------------------------------------------------------
void LogSystem::ShowTag(const std::string & tag)
{
	if(m_selectionIgnored)
	{
		RemoveTag(tag);
	}
	else
	{
		AddTag(tag);
	}
}

//--------------------------------------------------------------------------
void LogSystem::HideTag(const std::string & tag)
{
	if(!m_selectionIgnored)
	{
		RemoveTag(tag);
	}
	else
	{
		AddTag(tag);
	}
}

//--------------------------------------------------------------------------
STATIC void LogSystem::LogThreadWorker(void * data)
{
	UNUSED(data);
	
	while (g_LogSystem->IsRunning()) 
	{
		g_LogSystem->Flush(); 
		ThreadSleep(10);  
	}

	// do the inner side of the loop again right before exiting
	g_LogSystem->Flush(); 

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
	// check for tags
	String tag = data.tag;
	LogSystem* ls = LogSystem::GetInstance();

	if(ls->m_usingFilering)
	{
		if(ls->m_tags.DoesContain(tag) == !ls->m_selectionIgnored)
		{
			for(uint i = 0; i < s_callbacks.size(); i++)
			{
				LogHook current = s_callbacks.at(i);

				current.callback(data, current.user_argument);
			}
		}
	}
	else
	{
		for(uint i = 0; i < s_callbacks.size(); i++)
		{
			LogHook current = s_callbacks.at(i);

			current.callback(data, current.user_argument);
		}
	}
	
}

//--------------------------------------------------------------------------
void LogSystem::AddTag(const std::string & tag)
{
	if(m_tags.DoesContain(tag) == false)
		m_tags.Add(tag);
}

//--------------------------------------------------------------------------
void LogSystem::RemoveTag(const std::string & tag)
{
	if(m_tags.DoesContain(tag) == true)
		m_tags.Remove(tag);
}


//====================================================================================
void LogSystemStartUp()
{
	LogSystem* ls = LogSystem::GetInstance();
	ls->StartUp();

	ls->AddHook((log_cb) LogToFile, nullptr);
	ls->AddHook((log_cb) LogToOutputWindow, nullptr);

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
	log->text = Stringv( format, args );

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

//--------------------------------------------------------------------------
void LogToOutputWindow(const Log& theLog)
{
	String text = "\n" + theLog.tag + " : " + theLog.text + "\n";
	OutputDebugStringA(text.c_str());
}

//--------------------------------------------------------------------------
void ForceLogSystemFlush()
{
	LogSystem::GetInstance()->ForceFlush();
}

