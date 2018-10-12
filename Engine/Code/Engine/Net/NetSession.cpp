#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "../Core/General/EngineCommon.hpp"
#include "NetAddress.hpp"
#include "NetMessage.hpp"
#include "../Core/Tools/DevConsole.hpp"
#include "../Core/Platform/Window.hpp"
#include "../Renderer/Renderer.hpp"
#include "../Renderer/RenderableComponents/Material.hpp"
#include "Engine/Core/General/Camera.hpp"
#include "../Renderer/Systems/MeshBuilder.hpp"
#include "../Core/Platform/Time.hpp"

#include <algorithm>

//===============================================================================================

NetSession* NetSession::s_mainNetSession = nullptr;

//===============================================================================================
TimeStampedPacket::TimeStampedPacket(int delay, NetPacket* thePacket, const NetAddress& theAddress )
{
	m_delay = delay;
	m_packetToBeProcessed = thePacket;
	m_whoSentThePacket = theAddress;
	double delayInDecimal = (double) (((float)(delay)) * .001f);
	m_timeToBeProcessed = GetCurrentTimeSeconds() + delayInDecimal;

	//DevConsole::AddConsoleDialogue(Stringf("Giving this message a delay of %f", delayInDecimal));
}

//===============================================================================================
NetSession::NetSession()
{
	if(s_mainNetSession == nullptr)
		s_mainNetSession = this;

	m_latencyRange = IntRange();
}

//-----------------------------------------------------------------------------------------------
NetSession::~NetSession()
{
	// delete all connections
	for(uint i = 0; i < NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS; i++)
	{
		if(m_connections[i] != nullptr)
		{
			delete m_connections[i];
			m_connections[i] = nullptr;
		}
	}

	// delete all callbacks
	for(uint j = 0; j < m_messageCallbacks.size(); j++)
	{
		delete m_messageCallbacks.at(j);
		m_messageCallbacks.at(j) = nullptr;
	}
	m_messageCallbacks.clear();
}

//-----------------------------------------------------------------------------------------------
bool NetSession::RegisterMessageDefinition(const String& id, NetMessage_cb cb)
{
	if(IsMessageAlreadyRegistered(id))
		return false;

	int theID = (int) m_messageCallbacks.size();
	NetMessageDefinition* newDefinition = new NetMessageDefinition(theID, id, cb);

	m_messageCallbacks.push_back(newDefinition);
	return true;
}

//-----------------------------------------------------------------------------------------------
bool NetSession::IsMessageAlreadyRegistered(const String& id)
{
	for(uint i = 0; i < m_messageCallbacks.size(); i++)
	{
		NetMessageDefinition& current = *m_messageCallbacks.at(i);

		if(current.m_callbackName == id)
			return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------
NetMessageDefinition* NetSession::GetMessageDefinition(int const & id)
{
	for(uint i = 0; i < m_messageCallbacks.size(); i++)
	{
		NetMessageDefinition* current = m_messageCallbacks.at(i);

		if(current->m_callbackID == id)
			return current;
	}

	ERROR_RECOVERABLE("Could not find Message Definition at index: " + std::to_string(id));
	return nullptr;
}

//-----------------------------------------------------------------------------------------------
NetMessageDefinition* NetSession::GetMessageDefinitionByIndex(uint8_t idx)
{
	if(IsIndexValid((uint) idx, m_messageCallbacks))
	{
		for(uint i = 0; i < m_messageCallbacks.size(); i++)
		{
			NetMessageDefinition* current = m_messageCallbacks.at(i);

			if(current->m_callbackID == ((int)(idx)))
				return current;
		}
	}
	
	ERROR_RECOVERABLE("Could not find a NetMessage at index: " + std::to_string(idx));
	return nullptr;

}

//-----------------------------------------------------------------------------------------------
NetMessageDefinition* NetSession::GetMessageDefinitionByName(const String& name)
{
	for(uint i = 0; i < m_messageCallbacks.size(); i++)
	{
		NetMessageDefinition* current = m_messageCallbacks.at(i);

		if(current->m_callbackName == name)
			return current;
	}

	ERROR_RECOVERABLE("Could not find Message Definition for: " + name);
	return nullptr;
}

//-----------------------------------------------------------------------------------------------
void NetSession::SortDefinitions()
{	
	for(uint i = 0; i < m_messageCallbacks.size() - 1; i++)
	{
		for(uint j = i + 1; j < m_messageCallbacks.size(); j++)
		{
			NetMessageDefinition*& a = m_messageCallbacks.at(i);
			NetMessageDefinition*& b = m_messageCallbacks.at(j);

			if(a->m_callbackName > b->m_callbackName)
			{
				std::swap( a, b );
			}
		}
	}

	// need to update the callback ID as well now that we have sorted
	for(uint k = 0; k < m_messageCallbacks.size(); k++)
	{
		NetMessageDefinition*& current = m_messageCallbacks.at(k);
		current->m_callbackID = (int) k;
	}
}

//-----------------------------------------------------------------------------------------------
void NetSession::Bind(const char* port, uint range_to_try /*= 0U */)
{
	// the PacketChannel has a UDPSocket and that is in charge of
	// handling the ranges and created the NetAddress, so just 
	// pass it down to the socket
	
	NetAddress addressToTry = NetAddress::GetLocalAddress(port);
	int idx = m_channel.Bind(addressToTry, range_to_try);

	if(idx == -1)
	{
		DevConsole::AddErrorMessage("NetSession had an error binding the NetAddress");
	}
	else
	{
		DevConsole::AddConsoleDialogue("Bound to port: " + addressToTry.GetPortAsString(), Rgba::WHITE);
	}


	// sort the definitions alphabetically
	SortDefinitions();

}

//-----------------------------------------------------------------------------------------------
NetConnection* NetSession::AddConnection(uint idx, NetAddress const & addr)
{
	NetConnection* newConnection = new NetConnection((uint8_t) idx, addr, this);
	
	// make sure we use a valid idx
	if(idx > NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS || idx < 0)
	{
		ERROR_RECOVERABLE("Invalid Index when trying to add a new index: " + std::to_string(idx));
	}

	// see if it is ok to add to index
	if(m_connections[idx] == nullptr)
	{
		m_connections[idx] = newConnection;

		// gonna set the heart beat timer when we get it
		newConnection->SetHeartbeatTimer(m_heartbeatRate);
		return newConnection;
	}
	else
	{
		ERROR_RECOVERABLE("There is already a connection at idx: " + std::to_string(idx));
	}
	
	return nullptr;
}

//-----------------------------------------------------------------------------------------------
void NetSession::CloseAllConnections()
{
	for(uint i = 0; i < NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS; i++)
	{
		if(m_connections[i] != nullptr)
		{
			delete m_connections[i];
			m_connections[i] = nullptr;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void NetSession::ProcessIncoming()
{
	// receive packets, see who they came from
	// and call process those packets until
	// no more packets are available; 

	NetAddress theAddress;
	BytePacker tempBuffer(LITTLE_ENDIAN);
	size_t result = m_channel.m_socket->ReceiveFrom(&theAddress, tempBuffer.GetBuffer(), PACKET_MTU);
	
	//incomingPacket.AdvanceWriteHead(result);

	// DON'T DO THIS THIS ASSIGNMENT, PROCESS EVERYTHING
	// Check to see if the address is a valid connect (in our list)
	//if(!IsAddressValid(theAddress))
	//{
	//	// someone who is not connected to us sent us stuff
	//	// delete packet and return
	//}

	
	if(result != 0)
	{
		
		if(!CheckRandomChance(m_lossAmount))
		{
			// lets make a packet with the temp buffer!
			NetPacket* incomingPacket = new NetPacket(false);
			//incomingPacket.ResetWrite();
			incomingPacket->WriteBytes(result, tempBuffer.GetBuffer());
			
			// See if packet is valid
			if(!incomingPacket->IsValid(*this))
			{
				// throw the packet away
				DevConsole::AddErrorMessage("Received a bad packet from:" + theAddress.ToString());
				return;
			}

			AddPacketToQueue(*incomingPacket, theAddress);
			//ProcessPacket(incomingPacket, theAddress);
		}
		else
		{
			DevConsole::AddErrorMessage("Threw away a packet!");
		}
	}
	
	ProcessTimeStampedPackets();
}

//-----------------------------------------------------------------------------------------------
void NetSession::ProcessPacket( NetPacket& packet, const NetAddress& sender)
{
	// Get Packet Header
	packet.ReadHeader(&packet.m_header);

	// See who sent it
	NetSender theSender;
	if(packet.m_header.m_senderConnectionIndex == INVALID_CONNECTION_INDEX)
	{
		// look up by connection (temp)
		theSender = NetSender(*GetConnectionFromAddress(sender));
	}
	else
	{
		theSender = NetSender(*GetConnection(packet.m_header.m_senderConnectionIndex));
	}

	// Get How many messages are in the packet
	uint8_t amount = packet.m_header.m_unreliableCount;
	
	// For x amount of messages, 
	for(uint i = 0; i < amount; i++)
	{
		NetMessage currentMessage;
		bool result = packet.ReadMessage(&currentMessage);

		// make sure we read the message properly
		if(result == true)
		{
			NetMessageDefinition* theDef = GetMessageDefinitionByIndex(currentMessage.m_header.m_messageCallbackDefinitionIndex);

			if(theDef != nullptr)
				theDef->m_callback(currentMessage, theSender);
		}
	}
}

//-----------------------------------------------------------------------------------------------
void NetSession::AddPacketToQueue(NetPacket & packet, const NetAddress & sender)
{
	TimeStampedPacket newPacket = TimeStampedPacket(m_latencyRange.GetRandomInRange(), &packet, sender);
	m_timeStampedPacketQueue.push_back(newPacket);

	// always sort once we add them
	std::sort(m_timeStampedPacketQueue.begin(), m_timeStampedPacketQueue.end(), CompareTimeStampedPacket);
}

//-----------------------------------------------------------------------------------------------
void NetSession::ProcessTimeStampedPackets()
{
	for(uint i = 0; i < m_timeStampedPacketQueue.size(); i++)
	{
		TimeStampedPacket current = m_timeStampedPacketQueue.at(i);

		double currentTime = GetCurrentTimeSeconds();
		if(current.m_timeToBeProcessed < currentTime)
		{
			ProcessPacket(*current.m_packetToBeProcessed, current.m_whoSentThePacket);
			delete current.m_packetToBeProcessed;

			RemoveFast(i, m_timeStampedPacketQueue);
		}
	}
}

//-----------------------------------------------------------------------------------------------
void NetSession::ProcessOutgoing()
{
	// foreach connection, process outgoing; 
	for(uint i = 0; i < NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS; i++)
	{
		if(m_connections[i] != nullptr)
			m_connections[i]->ProcessOutgoing();
	}
}

//-----------------------------------------------------------------------------------------------
void NetSession::SendPacket(const NetPacket& packet)
{
	uint index = packet.m_indexToWhoWeAreSendingTo;

	NetConnection* whoWeAreSendingTo = GetConnection(index);

	// make sure we have a connection
	if(whoWeAreSendingTo == nullptr)
	{
		ERROR_RECOVERABLE("Connection does not exist! Message trashed");
	}

	// send
	size_t result = m_channel.m_socket->SendTo(
		whoWeAreSendingTo->m_address, 
		packet.GetConstBuffer(), 
		packet.GetWrittenByteCount());

	if(result == 0)
	{
		// it failed, but who cares it's unreliable 
	}
}

//-----------------------------------------------------------------------------------------------
void NetSession::Render() const
{
	Renderer* r = Renderer::GetInstance();

	r->SetCamera(r->m_defaultUICamera);
	r->m_currentCamera->RenderDebugOrtho();

	// background
	r->DrawAABB2(AABB2(-49.f, 49.f, 20.f, 20.f), Rgba(0,0,0,200));
	r->DrawAABB2(AABB2(-49.f, 49.f, 20.f, 20.f), Rgba::WHITE, false);

	Vector2 pivot;
	pivot.x = -48.f;
	pivot.y = 46.f;
	float textSize = 1.2;

	MeshBuilder mb;

	// Draw the title + state
	std::string title = "Netsession Info";
	mb.Add2DText(pivot, title, 1.5f, r->m_threadedColor);

	// Draw the info
	String theInfo = Stringf("Rate: %i hz || sim lag: %i ms - %i ms || sim_loss: %d",
		20, 0, 0, 0, 0.f);
	mb.Add2DText(Vector2(pivot.x, pivot.y - 2.f), theInfo, textSize, r->m_threadedColor);

	// Draw the socket bound to the session
	mb.Add2DText(Vector2(pivot.x, pivot.y - 4.f), "Bound SockAddr:", textSize, r->m_threadedColor);
	String boundSocketAddress = m_channel.m_socket->GetAddress().ToString();
	mb.Add2DText( Vector2(pivot.x + 1.f, pivot.y - 6.f), boundSocketAddress, textSize, r->m_threadedColor);

	// show all connections
	mb.Add2DText(Vector2(pivot.x, pivot.y - 8.f), "Connections:", textSize, r->m_threadedColor);
	String header = Stringf("-- %6s %-20s %-3s %-3s %-3s %-3s %-3s %-3s %-3s", 
		"idx", "address", "rtt(ms)", "loss%", "lrcv(s)", "lsnt(s)", "sntack", "rcvack", "rcvbits");
	mb.Add2DText(Vector2(pivot.x + 1.f, pivot.y - 10.f), header, .8f, r->m_threadedColor);
	
	Vector2 currentPos = Vector2(pivot.x + 1.f, pivot.y - 12.f);
	for(uint i = 0; i < NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS; i++)
	{
		if(m_connections[i] != nullptr)
		{
			String isLocal = "--";

			if(m_connections[i]->m_address == m_channel.m_socket->m_address)
				isLocal = "L";
			
			String connectionText = Stringf("%s %6i %-20s %-7d %-7d %-7d %-7d %-7d %-7d %-7d", 
				isLocal.c_str(), i , m_connections[i]->m_address.ToString().c_str(),
				0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			
			
			mb.Add2DText(currentPos, connectionText, .8f, r->m_threadedColor);

			currentPos.y -= 2.f;
		}

	}

	// D R A W
	r->BindMaterial("uiText");
	r->DrawMesh(mb.CreateMesh<Vertex3D_PCU>(), true);

	// reset
	r->SetCamera();
}

//-----------------------------------------------------------------------------------------------
uint8_t NetSession::GetMyConnection() const
{
	return INVALID_CONNECTION_INDEX;
}

//-----------------------------------------------------------------------------------------------
uint8_t NetSession::GetConnectionForPacket(const PacketHeader& theHeader, const NetAddress& theAddressOfSender)
{
	UNUSED(theHeader);
	UNUSED(theAddressOfSender);
	
	return INVALID_CONNECTION_INDEX;
}

//-----------------------------------------------------------------------------------------------
NetConnection* NetSession::GetConnection(int idx) const
{
	// check bounds
	if(idx < 0 || idx > NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS)
		return nullptr;
	
	if(m_connections[idx] != nullptr)
		return m_connections[idx];

	return nullptr;
}

//-----------------------------------------------------------------------------------------------
NetConnection* NetSession::GetConnectionFromAddress(const NetAddress& sender) const
{
	for(uint i = 0; i < NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS; i++)
	{
		NetConnection* current = m_connections[i];

		if(current != nullptr)
		{
			if((current->m_address == sender))
				return current;
		}

	}

	return nullptr;
}

//-----------------------------------------------------------------------------------------------
void NetSession::SetSimulatedLatency(int minAddedLatencyMS, int maxAddedLatencyMS /*= 0*/)
{
	m_latencyRange.min = minAddedLatencyMS;
	m_latencyRange.max = Max(minAddedLatencyMS, maxAddedLatencyMS);
}

//-----------------------------------------------------------------------------------------------
void NetSession::SetHeartbeat(float hz)
{
	m_heartbeatRate = hz;

	// update all connections
	for(uint i = 0; i < NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS; i++)
	{
		NetConnection* current = m_connections[i];

		if(current != nullptr)
		{
			current->SetHeartbeatTimer(hz);
		}
	}
}

//-----------------------------------------------------------------------------------------------
void NetSession::SetSessionFlushRate(float hz)
{
	m_sessionFlushRate = hz;

	// update all connections
	for(uint i = 0; i < NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS; i++)
	{
		NetConnection* current = m_connections[i];

		if(current != nullptr)
		{
			current->SetFlushRate(hz);
		}
	}
}

//-----------------------------------------------------------------------------------------------
void NetSession::SetConnectionFlushRate(uint idx, float hz)
{
	if(idx >= 0 && idx < NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS)
	{
		if(m_connections[idx] != nullptr)
		{
			m_connections[idx]->SetFlushRate(hz);
		}
	}
}

//===============================================================================================
bool CompareTimeStampedPacket(const TimeStampedPacket& a, const TimeStampedPacket& b)
{
	if(a.m_timeToBeProcessed < b.m_timeToBeProcessed)
		return true;
	return false;
}
