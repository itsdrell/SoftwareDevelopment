#include "RemoteCommandService.hpp"
#include "Engine\Net\TCPSocket.hpp"
#include "Engine\Core\General\BytePacker.hpp"
#include "Engine\Core\General\EngineCommon.hpp"
#include "..\..\Renderer\Renderer.hpp"
#include "..\Platform\Window.hpp"
#include "..\..\Net\NetAddress.hpp"
#include "..\..\Async\Threading.hpp"
#include "Clock.hpp"
#include "DevConsole.hpp"

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
	case REMOTE_COMMAND_TRY_TO_HOST:
		result = "TryToHost";
		break;
	case REMOTE_COMMAND_TRY_TO_JOIN:
		result = "TryToJoin";
	default:
		break;
	}

	return result;
}

//===============================================================================================
RemoteCommandService::RemoteCommandService()
{
	s_theService = this;
	ThreadCreateAndDetach((thread_cb) RemoteCommandServiceUpdate);
	m_currentColor = Rgba::BLUE;
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
	case REMOTE_COMMAND_TRY_TO_HOST:
		TryToHost(m_hostWithPortAddress.c_str());
		break;
	case REMOTE_COMMAND_TRY_TO_JOIN:
		TryToJoin(m_tryToJoinAddress.c_str());
		break;
	}

	UpdateColor();
}

//-----------------------------------------------------------------------------------------------
void RemoteCommandService::UpdateColor()
{
	// because we do this in update it gets off synce from the main thread and becomes 
	// a cool ass effect
	m_currentColor = GetRandomColor(); // gives a redish thing
}

//-----------------------------------------------------------------------------------------------
void RemoteCommandService::InitialUpdate()
{
	
	if(TryToJoin())
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
	if(m_connections.size() == 0)
		m_currentState = REMOTE_INITIAL_STATE;
}

//-----------------------------------------------------------------------------------------------
void RemoteCommandService::Render() const
{
	Renderer* r = Renderer::GetInstance();

	float windowWidth = Window::GetInstance()->GetWidth();
	float windowheight = Window::GetInstance()->GetHeight();

	Vector2 pivot;
	pivot.x = (windowWidth * .5f) - 630.f;
	pivot.y = (windowheight * .5f) - 20.f;

	// Draw the title + state
	std::string title = "Remote Command Service [" + RemoteCommandStateToString(m_currentState) + "]";
	r->DrawText2D(pivot, title, 16.f, m_currentColor);

	// Draw the joinable IP (port + LocalNetAddress)
	std::string address = "Join Address: " + NetAddress::GetLocalAddressAsString(m_portUsing.c_str());
	r->DrawText2D(pivot - Vector2(0.f, 20.f), address, 16.f, m_currentColor);
	
	// Draw number of connections
	std::string numberOfConnections = std::to_string(m_connections.size());
	r->DrawText2D(pivot - Vector2(-40.f, 40.f), numberOfConnections + " Connection(s)", 16.f, m_currentColor);

	// draw all connections
	if(m_connections.size() > 0)
	{	
		float startY = pivot.y - 40.f;
		for(uint i = 0; i < m_connections.size(); i++)
		{
			std::string addresssString =  "[" + std::to_string(i) + "] " + "NULL";
			
			if(m_connections.at(i) != nullptr)	
				addresssString = "[" + std::to_string(i) + "] " + m_connections.at(i)->m_address.ToString();
			
			r->DrawText2D(Vector2(pivot.x + 320.f, startY), addresssString, 12.f, m_currentColor);

			startY -= 14.f;
		}
	}

}

//-----------------------------------------------------------------------------------------------
bool RemoteCommandService::TryToJoin(NetAddress addressToJoin)
{
// 	if(m_currentState != REMOTE_INITIAL_STATE)
// 		return false;
	ClearAllConnections();

	// We are gonna try and join to the server
	NetAddress theAddress = addressToJoin;

	// we set this to blocklign and then non blocking in the connect call
	TCPSocket* socket = new TCPSocket(theAddress.ToString().c_str());
	
	if (socket->Connect( theAddress )) 
	{
		// We are now a client!
		if(m_currentState == REMOTE_COMMAND_TRY_TO_JOIN)
			m_currentState = REMOTE_CLIENT_STATE;

		// Now we store off the socket and create a packet for it as well
		m_connections.push_back(socket);
		m_connectionData.push_back(new BytePacker(BIG_ENDIAN));
	
		return true;
	}
	else
	{
		// we failed! Try to Host next :)
		delete socket;

		// for the console command
		if(m_currentState == REMOTE_COMMAND_TRY_TO_JOIN)
			m_currentState = REMOTE_INITIAL_STATE;

		return false;
	}
}

//-----------------------------------------------------------------------------------------------
bool RemoteCommandService::TryToHost(const char* port)
{
// 	if(m_currentState != REMOTE_INITIAL_STATE)
// 		return false;
	ClearAllConnections();
	TCPSocket* socket = new TCPSocket();
	if (socket->Listen( port, MAX_AMOUNT_OF_CONNECTIONS ))
	{
		// We are now a host!
		m_listeningSocket = socket;
		m_listeningSocket->SetBlockType(false);

		m_portUsing = port;

		// for console commands
		if(m_currentState == REMOTE_COMMAND_TRY_TO_HOST)
			m_currentState = REMOTE_HOST_STATE;

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
	TCPSocket* newConnection = m_listeningSocket->Accept();

	if(newConnection != nullptr && m_connections.size() < MAX_AMOUNT_OF_CONNECTIONS)
	{
		newConnection->SetBlockType(false);
		
		m_connections.push_back(newConnection);
		m_connectionData.push_back(new BytePacker(BIG_ENDIAN));
	}
}

//-----------------------------------------------------------------------------------------------
void RemoteCommandService::ProcessAllConnections()
{
	for(uint i = 0; i < m_connections.size(); i ++)
	{
		// this is for the echo to know who to send back to
		m_currentConnectedSockedIndex = i;
		
		BytePacker* buffer = m_connectionData.at(i);
		TCPSocket* currentSocket = m_connections.at(i);
		currentSocket->SetBlockType(false);
	
		if( buffer->GetReadableByteCount() < 2)
		{
			void* data = malloc(2);
			int read = (int) currentSocket->Receive( data, 2U - buffer->GetWrittenByteCount() );

			if(read > 0)
			{
				buffer->WriteRawBytes(read, data);
			}

			free(data);
		}
	
		bool isReadyToProcess = false;
		if(buffer->GetWrittenByteCount() >= 2U)
		{
			uint16_t len;
			buffer->Peek( &len, 2U ); // look at the first two bytes but not advance

			uint bytes_needed = (uint)(len + 2U - buffer->GetWrittenByteCount());
	
			if(bytes_needed > 0)
			{
				void* data = malloc(bytes_needed);
				int read = (int) currentSocket->Receive( data, bytes_needed);
				
				if(read > 0)
				{
					buffer->WriteRawBytes(read, data);
					bytes_needed -= read;
				}

				free(data);
			}
	
			isReadyToProcess = (bytes_needed == 0);
		}
	
	
		if(isReadyToProcess)
		{
			buffer->AdvanceReadHead( 2U );
			ProcessMessage( currentSocket, buffer );
			buffer->ResetWrite();
		}
	}
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

//-----------------------------------------------------------------------------------------------
void RemoteCommandService::ClearAllConnections()
{
	for(uint i = 0; i < m_connections.size(); i++)
	{
		delete m_connectionData.at(i);
		m_connectionData.at(i) = nullptr;
	
		m_connections.at(i)->Close();
		delete m_connections.at(i);
		m_connections.at(i) = nullptr;
	}
	
	m_connections.clear();
	m_connectionData.clear();
	
	if(m_listeningSocket != nullptr)
	{
		m_listeningSocket->Close();
		delete m_listeningSocket;
		m_listeningSocket = nullptr;
	}
}

//-----------------------------------------------------------------------------------------------
void RemoteCommandService::SendAMessage(uint idx, bool isEcho, char const* str)
{
	BytePacker message( BIG_ENDIAN ); 

	// make sure the index is valid
	if(!IsIndexValid(idx, m_connections))
	{
		DevConsole::AddErrorMessage("Invalid Index!");
	}

	if(m_connections.size() == 0)
	{
		DevConsole::AddErrorMessage("You have no connections");
	}

	TCPSocket* sock = m_connections.at( idx ); 
	if (sock == nullptr) 
	{
		return; 
	}

	message.WriteBytes( 1, &isEcho);
	//message.Write<bool>( is_echo );  
	message.WriteString( str ); 

	size_t len = message.GetWrittenByteCount(); 
	GUARANTEE_OR_DIE( len <= 0xffff, "Error" ); // format doesn't support lengths larger than a max ushort; 

	// This is sending the size
	uint16_t uslen = (uint16_t)len; 
	ToEndianness( sizeof(uslen), &uslen, BIG_ENDIAN ); 
	
	// do the send
	sock->Send(  &uslen, sizeof(uint16_t) );  
	sock->Send( (char*) message.GetBuffer(), len ); 

}

//-----------------------------------------------------------------------------------------------
void RemoteCommandService::ProcessMessage(const TCPSocket* socket, BytePacker* payload)
{
	UNUSED(socket);
	
	bool is_echo;
	payload->ReadBytes( &is_echo, 1 );

	char str[256];
	if(payload->ReadString( str, 256))
	{
		String theCommand = std::string(str);
		
		// succeeded in getting a command / string
		if( is_echo)
		{
			if(m_processingEchos)
			{
				String theindex = std::to_string(m_currentConnectedSockedIndex);
				String fullString = "[" + m_connections.at(m_currentConnectedSockedIndex)->m_address.ToString() 
					+ "] " + "(idx: " + theindex + ") " + theCommand;
				
				DevConsole::AddConsoleDialogue(fullString.c_str(), Rgba(200,200,200,255));
			}
		}
		else
		{
			RunRemoteCommandScript(theCommand.c_str());
		}

		payload->ResetWrite(); // so we don't just keep adding to the buffer
	}
}

//-----------------------------------------------------------------------------------------------
RemoteCommandService * RemoteCommandService::GetInstance()
{
	if(s_theService == nullptr)
	{
		s_theService = new RemoteCommandService();
	}

	return s_theService;


}

//===============================================================================================
void RemoteCommandServiceUpdate()
{
	while(RemoteCommandService::GetInstance()->m_isRunning)
	{
		RemoteCommandService::GetInstance()->Update();
	}
}

//-----------------------------------------------------------------------------------------------
void SendAMessage(uint idx, bool isEcho, char const* str)
{
	RemoteCommandService::GetInstance()->SendAMessage(idx, isEcho, str);
}

//-----------------------------------------------------------------------------------------------
void SendEcho(const char* message)
{
	RemoteCommandService* rcs = RemoteCommandService::GetInstance();
	rcs->SendAMessage(rcs->m_currentConnectedSockedIndex, true, message);
}

