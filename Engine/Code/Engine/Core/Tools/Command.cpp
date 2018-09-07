#include "Command.hpp"
#include "Engine/Core/Tools/ErrorWarningAssert.hpp"
#include "Engine/Core/Utils/StringUtils.hpp"

// declare all static containers
std::map<std::string, CommandRegistration*> CommandRegistration::s_mapOfCallBacks;
std::vector<CommandRegistration*> CommandRegistration::s_allCallBacks;

//std::vector<Command*> Command::s_commandHistory;

void CommandStartup()
{
	
}

void CommandShutdown()
{
	
}

void CommandRegister(char const* name,std::string syntax, std::string desciption, command_cb cb)
{
	// Doing the check and addition to the map inside of the constructor
	/*CommandRegistration* newCommand = */
	
	new CommandRegistration(name,syntax,desciption,cb);

}

bool CommandRun(char const* command)
{
	// construct a command
	Command* newCommand = new Command(command);
	
	// see if we have it
	if(CommandRegistration::CheckForCallBack(newCommand->GetName()))
	{
		// We found it, so lets get the callback that is stored
		command_cb commandToRun = CommandRegistration::GetCallBack(newCommand->GetName());
		
		// This calls the function..? so we pass the command
		commandToRun(*newCommand);

		delete newCommand;

		return true;
	}

	// error + fail
	//ERROR_RECOVERABLE("Could not find: " + std::string(command));

	delete newCommand;

	return false;
}

Strings CommandAutoComplete(std::string root)
{
	Strings allCommandNames = CommandRegistration::GetAllTheCommandsNames();
	Strings allHelp = CommandRegistration::GetAllCommandHelpText();

	Strings result;

	for(uint i = 0; i < allCommandNames.size(); i++)
	{
		std::string nameToCompare = allCommandNames.at(i);

		// make sure we don't go over. if the root is greater, then it doesn't match
		if(root.length() > nameToCompare.length())
			continue;

		std::string partOfName;

		for(int s = 0; s < root.length(); s++)
		{
			partOfName.push_back(nameToCompare.at(s));
		}

		// add the result to the return
		if(partOfName == root)
		{
			// make it one line with its description
			std::string resultString = allCommandNames.at(i) + "        " + allHelp.at(i);
			result.push_back(resultString);
		}
	}

	return result;
}

Command::Command(char const* str)
{
	// Create the command, breaking it up by whitespace
	m_commandArguements = BreakSentenceIntoWords(str);
	m_command = str;

	// for history
	//s_hi.push_back(this);
}

Command::~Command()
{
}

std::string Command::GetName()
{
	// Its the first word in the vector
	return m_commandArguements.at(0);
}

std::string Command::GetNextString()
{
	String result = "";
	
	if(m_commandIndex < m_commandArguements.size())
	{
		result = m_commandArguements.at(m_commandIndex);
		m_commandIndex++;
	}
	
	return result;
}

std::string Command::GetRestOfCommand()
{
	String theText = "";
	
	while(m_commandIndex < m_commandArguements.size())
	{
		theText += (m_commandArguements.at(m_commandIndex));// + " ");

		m_commandIndex++;
	}

	return theText;
}


// void Command::ClearCommands()
// {
// 	// Since these are pointers I want to go through the vector and delete them
// 	for(int i = 0; i < (int) s_commandHistory.size(); i++)
// 	{
// 		Command*& currentCommand = s_commandHistory.at(i);
// 		delete currentCommand;
// 	}
// 
// 	// Clear the vector
// 	s_commandHistory.clear();
// }

//////////////////////////////////////////////////////////////////////////
// Registration
CommandRegistration::CommandRegistration(std::string name, std::string syntax, std::string desciption, command_cb theCallBack)
{
	// Make sure we don't already have one
	if(CheckForCallBack(name) == true)
	{
		// Let us know if we are trying to add something we already have
		ERROR_RECOVERABLE("You already have a command called: " + name);
		return;
	}

	// make one!
	m_callBack = theCallBack;
	m_name = name;
	m_syntax = syntax;
	m_description = desciption;

	s_allCallBacks.push_back(this);
	s_mapOfCallBacks[name] = this;

}

CommandRegistration::~CommandRegistration()
{
}


command_cb CommandRegistration::GetCallBack(std::string callBackName)
{
	std::map<std::string, CommandRegistration*>::iterator callBackIterator;
	callBackIterator = s_mapOfCallBacks.find(callBackName);

	// return it if found
	if(callBackIterator != s_mapOfCallBacks.end())
		return callBackIterator->second->m_callBack; 

	// Since its the console, we don't need to die
	ERROR_RECOVERABLE("Could not find the CallBack: " + callBackName);

	return nullptr;

}

bool CommandRegistration::CheckForCallBack(std::string callBackName)
{
	std::map<std::string, CommandRegistration*>::iterator callBackIterator;
	callBackIterator = s_mapOfCallBacks.find(callBackName);

	// We could not find one, so the iterator == the end
	if(callBackIterator == s_mapOfCallBacks.end())
		return false;
	
	return true;
}

Strings CommandRegistration::GetAllTheCommandsNames()
{
	Strings theCommandNames;

	std::map<std::string, CommandRegistration*>::iterator nameIterator;

	for(nameIterator = s_mapOfCallBacks.begin(); nameIterator != s_mapOfCallBacks.end(); nameIterator++)
	{
		std::string currentName = nameIterator->first;
		theCommandNames.push_back(currentName);
	}

	return theCommandNames;
}

Strings CommandRegistration::GetAllCommandHelpText()
{
	Strings theCommandNames;

	std::map<std::string, CommandRegistration*>::iterator nameIterator;

	for(nameIterator = s_mapOfCallBacks.begin(); nameIterator != s_mapOfCallBacks.end(); nameIterator++)
	{
		std::string currentName = nameIterator->second->m_description;
		theCommandNames.push_back(currentName);
	}

	return theCommandNames;
}

Strings CommandRegistration::GetAllCommandsAndTheirHelp()
{
	Strings allTheText;

	allTheText.push_back("Most commands take help as a parameter to show ALL parameters for that command!");
	
	for(int i = 0; i < (int)s_allCallBacks.size(); i++)
	{
		CommandRegistration* currentCommand = s_allCallBacks.at(i);
		
		// I do some formatting here so I can make commands easier
		//std::string currentText = std::string(currentCommand->m_name 
		//	+ " | " + currentCommand->m_syntax + " | " + currentCommand->m_description);

		// this is without the syntax since I give them all a help command
		std::string currentText = std::string(currentCommand->m_name 
			+ " | " + currentCommand->m_description);

		allTheText.push_back(currentText);
	}

	return allTheText;
}
