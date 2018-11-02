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
#include "../Core/Tools/Clock.hpp"
#include "../Core/Utils/MiscUtils.hpp"

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
bool NetSession::RegisterMessageDefinition( uint8_t id, const String& name, NetMessage_cb cb, eNetMessageOptions option )
{
	if(IsMessageAlreadyRegistered(name))
		return false;

	//int theID = (int) m_messageCallbacks.size();
	NetMessageDefinition* newDefinition = new NetMessageDefinition(id, name, cb, option);

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
NetMessageDefinition* NetSession::GetMessageDefinitionByIndex(uint8_t idx) const
{
	if(IsIndexValid((uint) idx, (std::vector<NetMessageDefinition*>&) m_messageCallbacks))
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
NetMessageDefinition* NetSession::GetMessageDefinitionByName(const String& name) const
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

		// see if we just added our own index, if so, store of the index for easy access!
		if(addr == m_channel.m_socket->m_address)
			m_connectionsIndexInSession = idx;

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
				delete incomingPacket;
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

	// See who sent it OR MAKE A TEMP SENDER
	NetSender* theSender;
	if(packet.m_header.m_senderConnectionIndex == INVALID_CONNECTION_INDEX)
	{
		// look up by connection (temp)
		NetConnection* theConnection = GetConnectionFromAddress(sender);
		if(theConnection != nullptr)
			theSender = new NetSender(theConnection);
		else
			theSender = nullptr;
	}
	else
	{
		NetConnection* theConnection = GetConnection(packet.m_header.m_senderConnectionIndex);
		if(theConnection != nullptr)
			theSender = new NetSender(theConnection);
		else
			theSender = nullptr;
	}

	// Do the on receive packet stuff only for connections we know about
	if(theSender != nullptr && theSender->m_connection != nullptr)
		theSender->m_connection->OnReceivePacket(packet.m_header, &packet);


	// if we do not know a sender, we are gonna make a temp one
	NetConnection* theConnection = nullptr;
	if(theSender == nullptr || theSender->m_connection == nullptr)
	{
		theConnection = new NetConnection(69, sender, this);
		theSender = new NetSender(theConnection);		
	}

	// Get How many messages are in the packet
	uint8_t amount = packet.m_header.m_messageCount;
	
	// For x amount of messages, 
	for(uint i = 0; i < amount; i++)
	{
		NetMessage currentMessage;
		bool result = packet.ReadMessage(&currentMessage, *this);

		// make sure we read the message properly
		if(result == true)
		{
			
			if(currentMessage.RequiresConnection(*this) && theConnection != nullptr)
			{
				// skip message and log a warning
				DevConsole::AddErrorMessage("Received a packet that requires a connection from:" + sender.ToString());
			}
			else
			{
				NetMessageDefinition* theDef = GetMessageDefinitionByIndex(currentMessage.m_header.m_messageCallbackDefinitionIndex);
				if(theDef != nullptr)
				{
					// see if we are reliable
					if(theDef->m_option == NETMESSAGE_OPTION_RELIABLE)
					{
						if(!DoesContain(currentMessage.m_header.m_reliableID, theSender->m_connection->m_receivedReliableIDs))
						{
							theDef->m_callback(currentMessage, *theSender);

							// we processed the message so add it to our list!
							theSender->m_connection->m_receivedReliableIDs.push_back(currentMessage.m_header.m_reliableID);

							// remove old ones and add highest received
							theSender->m_connection->UpdateRecievedReliableList(currentMessage.m_header.m_reliableID);
						}
					}
					else // if not, just process
					{
						theDef->m_callback(currentMessage, *theSender);
					}
					
				}
				
			}

		}
	}

	if(theConnection != nullptr)
		delete theConnection;
	delete theSender;
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
	SendUnreliableTest();
	SendReliableTest();
	
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
	//r->m_currentCamera->RenderDebugOrtho();

	// background
	r->DrawAABB2(AABB2(-49.f, 49.f, 40.f, 20.f), Rgba(0,0,0,200));
	r->DrawAABB2(AABB2(-49.f, 49.f, 40.f, 20.f), Rgba::WHITE, false);

	Vector2 pivot;
	pivot.x = -48.f;
	pivot.y = 46.f;
	float textSize = 1.2f;

	MeshBuilder mb;

	// Draw the title + state
	std::string title = "Netsession Info";
	mb.Add2DRandomColoredText(pivot, title, 1.77f, 1.77f);

	// Draw the info
	String theInfo = Stringf("Rate: %d hz || sim lag: %d ms - %d ms || sim_loss: %d",
		(int) m_sessionFlushRate,	// Flush rate
		m_latencyRange.min,			// Latency min
		m_latencyRange.max,			// Latency max
		(int) m_lossAmount);		// loss amount
	mb.Add2DRandomColoredText(Vector2(pivot.x, pivot.y - 2.f), theInfo, textSize);

	// Draw the socket bound to the session
	mb.Add2DRandomColoredText(Vector2(pivot.x, pivot.y - 4.f), "Bound SockAddr:", textSize);
	String boundSocketAddress = m_channel.m_socket->GetAddress().ToString();
	mb.Add2DRandomColoredText( Vector2(pivot.x + 1.f, pivot.y - 6.f), boundSocketAddress, textSize);

	// show all connections
	mb.Add2DRandomColoredText(Vector2(pivot.x, pivot.y - 8.f), "Connections:", textSize);
	String header = Stringf("-- %6s %-20s %-8s %-7s %-7s %-7s %-7s %-7s %-18s %-7s", 
		"idx", "address", "rtt(ms)", "loss%", "lrcv(s)", "lsnt(s)", "sntack", "rcvack", "rcvbits", "RLC");
	mb.Add2DRandomColoredText(Vector2(pivot.x + 1.f, pivot.y - 10.f), header, .8f);
	
	Vector2 currentPos = Vector2(pivot.x + 1.f, pivot.y - 12.f);
	for(uint i = 0; i < NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS; i++)
	{
		NetConnection* currentConnection = m_connections[i];
		
		if(currentConnection != nullptr)
		{
			String isLocal = "--";

			if(currentConnection->m_address == m_channel.m_socket->m_address)
				isLocal = "L";
			
			float lastReceivedTime = (float)((GetTimeInMilliseconds() - currentConnection->GetLastReceivedTimeInMS())) / 1000.f;
			float lastSentTime = (float)((GetTimeInMilliseconds() - currentConnection->m_lastSendTimeMS)) / 1000.f;

			String connectionText = Stringf("%-2s %6i %-20s %-8.3f %-7.3f %-7.3f %-7.3f %-7d %-7d %-18s %-7i", 
				isLocal.c_str(), 
				i , 
				currentConnection->m_address.ToString().c_str(),
				currentConnection->GetRTT(), // RTT 
				currentConnection->GetLoss(), // LOSS
				lastReceivedTime, // LRCV
				lastSentTime, // LSNT
				currentConnection->m_lastSentAck, // Sntack
				currentConnection->m_highestReceivedAck, // Rcvack
				GetBytesAsString(currentConnection->m_previousReceivedAckBitfield).c_str(),// Rcvbits
				currentConnection->m_sentAndUnconfirmedReliables.size()); 
			
			mb.Add2DRandomColoredText(currentPos, connectionText, .8f);

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
NetConnection* NetSession::GetMyConnection() const
{
	return m_connections[m_connectionsIndexInSession];
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

//-----------------------------------------------------------------------------------------------
void NetSession::SendUnreliableTest()
{
	if(m_currentAmount == 0)
		return;

	if(m_unreliableTimer == nullptr)
	{
		m_unreliableTimer = new Timer();
		m_unreliableTimer->SetTimer(2.f);
	}


	if(m_unreliableTimer->CheckAndReset())
	{
		NetConnection *cp = GetConnection( m_idx ); 
		//if (cp == nullptr) 
		//{
		//	DevConsole::AddErrorMessage( Stringf("Unknown connection: %u", m_idx )); 
		//	return; 
		//}

		NetMessage* msg = new NetMessage("unreliable_test"); 
		msg->WriteBytes( sizeof(uint), &m_currentAmount ); 
		msg->WriteBytes( sizeof(uint), &m_totalAmount ); 
		cp->Send( *msg );
		
		m_currentAmount--;
	}
}

//-----------------------------------------------------------------------------------------------
void NetSession::SendReliableTest()
{
	if(m_reliableCurrentAmount == 0)
		return;

	if(m_reliableTimer == nullptr)
	{
		m_reliableTimer = new Timer();
		m_reliableTimer->SetTimer(2.f);
	}


	if(m_reliableTimer->CheckAndReset())
	{
		NetConnection *cp = GetConnection( m_reliableIdx ); 

		NetMessage* msg = new NetMessage("reliable_test"); 
		msg->WriteBytes( sizeof(uint), &m_reliableCurrentAmount ); 
		msg->WriteBytes( sizeof(uint), &m_totalAmount ); 
		cp->Send( *msg );

		m_reliableCurrentAmount--;
	}
}

//===============================================================================================
bool CompareTimeStampedPacket(const TimeStampedPacket& a, const TimeStampedPacket& b)
{
	if(a.m_timeToBeProcessed < b.m_timeToBeProcessed)
		return true;
	return false;
}

//-----------------------------------------------------------------------------------------------
NetSender::~NetSender()
{
	// shouldn't need to delete the connection because someone else (session)
	// is in charge of that
	m_connection = nullptr;
}
