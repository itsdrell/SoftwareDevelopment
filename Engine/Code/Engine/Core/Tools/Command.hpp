#pragma once
#include <vector>
#include <string>
#include <map>
#include "Engine/Core/General/EngineCommon.hpp"



//////////////////////////////////////////////////////////////////////////
// COMMAND
// A command is a single submitted commmand
// NOT the definition (which I hide internally)
// Comments will be using a Command constructed as follows; 
// Command cmd = Command( "echo_with_color (255,255,0) \"Hello \\"World\\"\" ); 
class Command
{
public:
	Command( char const *str ); 
	~Command(); 

	std::string GetName(); // would return "echo_with_color"

	// Gets the next string in the argument list.
	// Breaks on whitespace.  Quoted strings should be treated as a single return 
	std::string GetNextString();   // would return after each call...
								   // first:  "(255,255,0)""
								   // second: "Hello \"world\""
								   // third+: ""

	// [OPTIONAL] I like to add helpers for getting arguments
	// and I just add them as I need them.
	// Each takes the output variable, and returns whether it succeeded
	// bool GetNextInt( int *out_val ); 
	// bool GetNextColor( RGBA *out_val );
	// bool GetNextVector2( Vector2 *out_val ); 
	// bool GetNextVector3( Vector3 *out_val );
	// ... 


	//static void ClearCommands();

public:
	std::string	m_command; 
	Strings		m_commandArguements;

	uint		m_commandIndex = 1; // the name is the 0, so we start at 1

	// gonna let the dev console hold it..?
	//static std::vector<Command*>	s_commandHistory;
};

//////////////////////////////////////////////////////////////////////////
// Command callbacks take a Command.
typedef void (*command_cb)( Command &cmd ); 

//////////////////////////////////////////////////////////////////////////
// This is just used to house all the commands and other useful info
// I want to make this class have more power so it'll be easier to use
class CommandRegistration
{
public:
	CommandRegistration(std::string name, std::string syntax, std::string desciption, command_cb theCallBack);
	~CommandRegistration();

	static command_cb	GetCallBack(std::string callBackName);
	static bool			CheckForCallBack(std::string callBackName);
	static Strings		GetAllTheCommandsNames();
	static Strings		GetAllCommandHelpText();
	static Strings		GetAllCommandsAndTheirHelp();


public:
	std::string		m_name;
	std::string		m_syntax;
	std::string		m_description;
	command_cb		m_callBack;

private:
	// I'm not sure which I like yet, so im testing both
	static std::map<std::string, CommandRegistration*>	s_mapOfCallBacks;
	static std::vector<CommandRegistration*>			s_allCallBacks;
};

//////////////////////////////////////////////////////////////////////////
// Globals

// Allows for setting up the system and clenaing up.
// Optional, but may help with some tasks. 
void CommandStartup();
void CommandShutdown(); 

// Registers a command with the system
// Example, say we had a global function named...
//    void Help( Command &cmd ) { /* ... */ }  
// 
// We then, during some startup, call
//    CommandRegister( "help", Help ); 
void CommandRegister( char const *name, std::string syntax, std::string desciption, command_cb cb ); 

// Will construct a Command object locally, and if 
// a callback is associated with its name, will call it and 
// return true, otherwise returns false.
// Name is case-insensitive
bool CommandRun( char const *command ); 


// Returns a list of commands that start with the root word
// should ignore case. 
Strings CommandAutoComplete( std::string root ); 