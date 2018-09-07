#pragma once
#include <vector>
#include <string>
#include "Stopwatch.hpp"
#include "..\General\Rgba.hpp"

//====================================================================================
// Forward Declare
//====================================================================================
class TCPSocket;
class BytePacker;

//====================================================================================
// Type Defs + Defines
//====================================================================================
#define REMOTE_COMMAND_PORT ("29283")

#define MAX_AMOUNT_OF_CONNECTIONS (32)

//====================================================================================
// ENUMS
//====================================================================================
enum RemoteCommandStates
{
	REMOTE_INITIAL_STATE,
	REMOTE_HOST_STATE,
	REMOTE_CLIENT_STATE,
	REMOTE_NONE
};
std::string RemoteCommandStateToString(RemoteCommandStates state);

//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class RemoteCommandService
{
private:
	RemoteCommandService();

public:
	~RemoteCommandService();

	void Update();
	void UpdateColor();
	void InitialUpdate();
	void HostUpdate();
	void ClientUpdate();

	void Render() const;

	bool TryToJoinLocal();
	bool TryToHost();

	void ProcessNewConnections();
	void ProcessAllConnections();
	void CleanUpDisconnects();

	void SendAMessage( uint idx, bool isEcho, char const* str );
	void ProcessMessage( const TCPSocket* socket, BytePacker* payload);

	static RemoteCommandService* GetInstance();

public:
	
	// this could be a class called TCPSession
	std::vector<TCPSocket*>					m_connections;
	std::vector<BytePacker*>				m_connectionData;

	RemoteCommandStates						m_currentState = REMOTE_INITIAL_STATE;

	TCPSocket*								m_listeningSocket = nullptr;

	bool									m_isRunning = true;

	Rgba									m_currentColor;

	static RemoteCommandService*			s_theService;
};

//====================================================================================
// Standalone C Functions
//====================================================================================
void RemoteCommandServiceUpdate();
void SendAMessage(uint idx, bool isEcho, char const* str);

//====================================================================================
// Externs
//====================================================================================


//====================================================================================
// Written by Zachary Bracken : [9/2/2018]
//====================================================================================