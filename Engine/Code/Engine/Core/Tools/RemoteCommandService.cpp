#include "RemoteCommandService.hpp"
#include "Engine\Net\TCPSocket.hpp"
#include "Engine\Core\General\BytePacker.hpp"
#include "Engine\Core\General\EngineCommon.hpp"
#include "..\..\Renderer\Renderer.hpp"
#include "..\Platform\Window.hpp"
#include "..\..\Net\NetAddress.hpp"

//===============================================================================================
RemoteCommandService* RemoteCommandService::s_theService = nullptr;

//===============================================================================================
std::string RemoteCommandStateToString(RemoteCommandStates state)
{
	String result = "ERROR";
	
	switch (state)
	{
	case REMOTE_INITIAL_STATE:
		result = "INITIAL";
		break;
	case REMOTE_HOST_STATE:
		result = "HOST";
		break;
	case REMOTE_CLIENT_STATE:
		result = "CLIENT";
		break;
	default:
		break;
	}

	return result;
}

//===============================================================================================
RemoteCommandService::RemoteCommandService()
{
}

//-----------------------------------------------------------------------------------------------
RemoteCommandService::~RemoteCommandService()
{
	if(m_listeningSocket != nullptr)
	{
		delete m_listeningSocket;
		m_listeningSocket = nullptr;
	}
}

//-----------------------------------------------------------------------------------------------
void RemoteCommandService::Update()
{
	switch (m_currentState)
	{
	case REMOTE_INITIAL_STATE:
		InitialUpdate();
		break;
	case REMOTE_HOST_STATE:
		HostUpdate();
		break;
	case REMOTE_CLIENT_STATE:
		ClientUpdate();
		break;
	}
}

//-----------------------------------------------------------------------------------------------
void RemoteCommandService::InitialUpdate()
{
	if(TryToJoinLocal())
	{
		m_currentState = REMOTE_CLIENT_STATE;
		return;
	}

	if(TryToHost())
	{
		m_currentState = REMOTE_HOST_STATE;
		return;
	}
}

//-----------------------------------------------------------------------------------------------
void RemoteCommandService::HostUpdate()
{
	// Sees if anyone is trying to connect
	ProcessNewConnections();
	// Sees if anyone has sent them a message or an echo
	ProcessAllConnections();
	// takes care of any disconnects
	CleanUpDisconnects();
}

//-----------------------------------------------------------------------------------------------
void RemoteCommandService::ClientUpdate()
{
	ProcessAllConnections();
	CleanUpDisconnects();

	// See if there is no host, if not, go back to Init
}

//-----------------------------------------------------------------------------------------------
void RemoteCommandService::Render() const
{
	Renderer* r = Renderer::GetInstance();
	
	float windowWidth = Window::GetInstance()->GetWidth();
	float windowheight = Window::GetInstance()->GetHeight();

	Vector2 pivot;
	pivot.x = (windowWidth * .5f) - 600.f;
	pivot.y = (windowheight * .5f) - 20.f;

	// Draw the title + state
	std::string title = "Remote Command Service [" + RemoteCommandStateToString(m_currentState) + "]";
	r->DrawText2D(pivot, title, 16.f);

	// Draw the joinable IP (port + LocalNetAddress)
	std::string address = "Join Address: " + NetAddress::GetLocalAddressAsString(REMOTE_COMMAND_PORT);
	r->DrawText2D(pivot - Vector2(0.f, 20.f), address, 16.f);
	
	// Draw number of connections
	std::string numberOfConnections = std::to_string(m_connections.size());
	r->DrawText2D(pivot - Vector2(-80.f, 40.f), numberOfConnections + " Connections", 16.f);

	// draw all connections
	if(m_connections.size() > 0)
	{
		float startY = pivot.y - 40.f;
		for(uint i = 0; i < m_connections.size(); i++)
		{
			std::string address = "[" + std::to_string(i) + "] " + m_connections.at(i)->m_address.ToString();
			r->DrawText2D(Vector2(pivot.x + 350.f, startY), address, 12.f);

			startY -= 14.f;

		}
	}

}

//-----------------------------------------------------------------------------------------------
bool RemoteCommandService::TryToJoinLocal()
{
	// We are gonna try and join to the server
	NetAddress theAddress = NetAddress::GetLocalAddress(REMOTE_COMMAND_PORT);

	TCPSocket* socket = new TCPSocket();
	if (socket->Connect( theAddress )) 
	{
		// We are now a client!
	
		// Now we store off the socket and create a packet for it as well
		m_connections.push_back(m_listeningSocket);
		m_connectionData.push_back(new BytePacker());
	
		return true;
	}
	else
	{
		// we failed! Try to Host next :)
		delete socket;
		return false;
	}
}

//-----------------------------------------------------------------------------------------------
bool RemoteCommandService::TryToHost()
{
	TCPSocket* socket = new TCPSocket();
	if (socket->Listen( REMOTE_COMMAND_PORT, MAX_AMOUNT_OF_CONNECTIONS ))
	{
		// We are now a host!
		m_listeningSocket = socket;

		return true;
	}
	else
	{
		// we failed! Try to Join next :)
		delete socket;
		return false;
	}
}

//-----------------------------------------------------------------------------------------------
void RemoteCommandService::ProcessNewConnections()
{
}

//-----------------------------------------------------------------------------------------------
void RemoteCommandService::ProcessAllConnections()
{
}

//-----------------------------------------------------------------------------------------------
void RemoteCommandService::CleanUpDisconnects()
{
	for(uint i = 0; i < m_connections.size(); i++)
	{
		TCPSocket* current = m_connections.at(i);

		if(current->IsClosed())
		{
			delete current;
			current = nullptr;

			RemoveFast(i, m_connections);
		}
	}
}

//===============================================================================================
RemoteCommandService * RemoteCommandService::GetInstance()
{
	if(s_theService == nullptr)
	{
		s_theService = new RemoteCommandService();
	}
	else
	{
		return s_theService;
	}

}


