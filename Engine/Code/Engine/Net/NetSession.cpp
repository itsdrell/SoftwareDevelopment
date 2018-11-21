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
String NetSessionStateToString(eSessionState theState)
{
	String result = "";
	switch (theState)
	{
	case SESSION_DISCONNECTED:
		result = "Disconnected";
		break;
	case SESSION_BOUND:
		result = "Bound";
		break;
	case SESSION_CONNECTING:
		result = "Connecting";
		break;
	case SESSION_JOINING:
		result = "Joining";
		break;
	case SESSION_READY:
		result = "Ready";
		break;
	}

	return result;
}

//===============================================================================================
NetSession::NetSession()
{
	if(s_mainNetSession == nullptr)
		s_mainNetSession = this;

	m_latencyRange = IntRange();
	m_channel = new PacketChannel();
}

//-----------------------------------------------------------------------------------------------
NetSession::~NetSession()
{
	// delete all connections
	for(uint i = 0; i < NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS; i++)
	{
		if(m_boundConnections[i] != nullptr)
		{
			delete m_boundConnections[i];
			m_boundConnections[i] = nullptr;
		}
	}

	// delete all callbacks
	for(uint j = 0; j < m_messageCallbacks.size(); j++)
	{
		delete m_messageCallbacks.at(j);
		m_messageCallbacks.at(j) = nullptr;
	}
	m_messageCallbacks.clear();

	delete m_channel;
	m_channel = nullptr;
}

//-----------------------------------------------------------------------------------------------
void NetSession::Host(char const * my_id, const char * port, uint16_t port_range)
{
	//m_channel = PacketChannel();
	
	// can only host in this state so warning and early out
	if(m_state != SESSION_DISCONNECTED)
	{
		DevConsole::AddErrorMessage("Could not host because I am not disconnected");
		return;
	}

	// Bind so we can host 
	bool result = Bind(port, port_range);

	// check for errors
	if(result == false)
	{
		SetError(SESSION_ERROR_INTERNAL, "NetSession had an error binding the NetAddress");
		return;
	}

	// create the host
	NetConnectionInfo hostInfo;
	strcpy_s(hostInfo.m_id, my_id);
	hostInfo.m_sessionIndex = 0;
	hostInfo.m_address = m_channel->m_socket->GetAddress();

	// assign to the convenience pointers
	m_hostConnection = CreateConnection(hostInfo);
	m_myConnection = m_hostConnection;

	// mark both as ready
	m_myConnection->m_state = NET_CONNECTION_STATUS_READY;
	m_state = SESSION_READY;
}

//-----------------------------------------------------------------------------------------------
void NetSession::Join(char const * my_id, const NetAddress & hostAddress)
{
	//m_channel = PacketChannel();
	
	// bind socket
	bool result = Bind(GAME_PORT, NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS);

	// Create a connection for the host, bind it, and mark it as CONNECTED. 
	NetConnectionInfo hostInfo;
	hostInfo.m_address = hostAddress;
	hostInfo.m_sessionIndex = 0U;
	m_hostConnection = CreateConnection(hostInfo);
	m_hostConnection->m_state = NET_CONNECTION_STATUS_CONNECTED;

	//	Create a connection for yourself, and set it to `m_my_connection` (currently has an invalid index) 
	NetConnectionInfo myInfo;
	myInfo.m_address = m_channel->m_socket->m_address;
	strcpy_s(myInfo.m_id, my_id);
	
	m_myConnection = CreateConnection(myInfo);

	//	Set session state to `SESSION_CONNECTING`
	m_state = SESSION_CONNECTING;

	// Create the timers for joining
	m_sendJoinRequestTimer = new Timer();
	m_sendJoinRequestTimer->SetTimer(SESSION_SEND_JOIN_RATE);
	
	m_timeOutTimer = new Timer();
	m_timeOutTimer->SetTimer(SESSION_TIMEOUT_RATE);
}

//-----------------------------------------------------------------------------------------------
NetConnection* NetSession::CreateConnection(const NetConnectionInfo& info)
{
	NetConnection* newConnection = new NetConnection(info, this);

	// always put in here
	m_AllConnections.push_back(newConnection);

	// if we have the slot open, bind it
	if(info.m_sessionIndex != INVALID_CONNECTION_INDEX && IsConnectionIndexValid(info.m_sessionIndex))
	{
		newConnection->m_indexInSession = BindConnection(info.m_sessionIndex, newConnection);
	}

	return newConnection;
}

//-----------------------------------------------------------------------------------------------
void NetSession::Update()
{
	ProcessIncoming();

	switch (m_state)
	{
	case SESSION_DISCONNECTED:
		break;
	case SESSION_BOUND:
		break;
	case SESSION_CONNECTING:
		SessionConnectingUpdate();
		break;
	case SESSION_JOINING:
		SessionJoiningUpdate();
		break;
	case SESSION_READY:
		SessionReadyUpdate();
		break;
	default:
		break;
	}

	CheckForDisconnects();
}

//-----------------------------------------------------------------------------------------------
void NetSession::SessionConnectingUpdate()
{
	//	If you have been in this state for `JOIN_TIMEOUT` (roughly 10s), error and disconnect with a "Timed out" error.
	if(m_timeOutTimer->HasElapsed())
	{
		m_state = SESSION_DISCONNECTED;
		return;
	}
	
	//If you connection `is_connected`, move to the session state `SESSION_JOINING`
	if(m_myConnection->m_state == NET_CONNECTION_STATUS_CONNECTED)
	{
		m_state = SESSION_JOINING;
	}
	
	//	Otherwise, every 0.1 seconds, send a `JOIN_REQUEST` message to the host
	if(m_sendJoinRequestTimer->CheckAndReset())
	{
		NetMessage joinMessage("joinRequest");
		joinMessage.WriteString(m_myConnection->m_info.m_id);

		SendDirectMessageTo(joinMessage, m_hostConnection->m_address);
	}
	
}

//-----------------------------------------------------------------------------------------------
void NetSession::SessionJoiningUpdate()
{
	//State does nothing, but waits until your connection is marked as ready (happens in clalback for a `JOIN_FINISHED` message)
	if(m_myConnection->m_state == NET_CONNECTION_STATUS_READY)
	{
		m_state = SESSION_READY;
	}
}

//-----------------------------------------------------------------------------------------------
void NetSession::SessionReadyUpdate()
{
	// changes the state in -v
	if(m_myConnection->HasStateChanged())
	{
		// let everyone know my status changed
		for(uint i = 0; i < NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS; i++)
		{
			NetConnection* current = m_boundConnections[i];

			if(current != nullptr)
			{
				NetMessage newMessage("updateConState");
				newMessage.WriteBytes(1U, &m_myConnection->m_state);

				current->Send(newMessage);
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void NetSession::CheckForDisconnects()
{
	for(uint i = 0; i < NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS; i++)
	{
		NetConnection* current = m_boundConnections[i];

		if(current != nullptr)
		{
			if(current->m_state == NET_CONNECTION_STATUS_DISCONNECTED)
			{
				// should have already sent a state change message by now (in update switch statement ready)
				if(current->IsHost())
				{
					if(current == m_myConnection)
					{
						// delete all connections 
						DestroyAllConnections();
					}
					else
					{
						// just remove the one connection
						DestroyConnection(current);
					}
				}
				else // if client
				{
					// Delete all connections
					DestroyAllConnections();
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void NetSession::DestroyConnection(NetConnection *cp)
{
	if(m_hostConnection == cp)
	{
		m_hostConnection = nullptr;
		// I don't think we delete?
	}

	if(m_myConnection == cp)
	{
		m_myConnection = nullptr;
		// I don't think we delete here?
	}

	// look up table
	for(uint i = 0; i < NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS; i++)
	{
		NetConnection* current = m_boundConnections[i];

		if(current == cp)
		{
			m_boundConnections[i] = nullptr;
		}
	}

	// all connections
	for(uint i = 0; i < m_AllConnections.size(); i++)
	{
		NetConnection* current = m_AllConnections.at(i);

		if(current == cp)
		{
			m_AllConnections.at(i) = nullptr;

			RemoveFast(i, m_AllConnections);
		}
	}

	// free
	delete cp;
	cp = nullptr;
}

//-----------------------------------------------------------------------------------------------
void NetSession::DestroyAllConnections()
{
	for(uint i = 0; i < NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS; i++)
	{
		NetConnection* current = m_boundConnections[i];

		if(current != nullptr)
		{
			DestroyConnection(current);
		}
	}

	// we have no connections so we are disconnected
	m_state = SESSION_DISCONNECTED;
}

//-----------------------------------------------------------------------------------------------
uint8_t NetSession::BindConnection(uint8_t idx, NetConnection *cp)
{
	// if it's invalid, just find one and return it
	if(idx == INVALID_CONNECTION_INDEX)
	{
		for(uint i = 0; i < NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS; i++)
		{
			if(m_boundConnections[i] == nullptr)
				return (uint8_t) i;
		}

		// error check?
		return idx;
	}
	else
	{
		m_boundConnections[idx] = cp;
		return idx;
	}
	
}

//-----------------------------------------------------------------------------------------------
bool NetSession::IsConnectionIndexValid(const uint8_t m_sessionIndex)
{
	if(m_boundConnections[m_sessionIndex] != nullptr)
		return false;

	if(m_sessionIndex < 0 || m_sessionIndex > NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS)
		return false;

	return true;
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
bool NetSession::Bind(const char* port, uint range_to_try /*= 0U */)
{
	// the PacketChannel has a UDPSocket and that is in charge of
	// handling the ranges and created the NetAddress, so just 
	// pass it down to the socket
	
	if(m_channel != nullptr)
		delete m_channel;

	m_channel = new PacketChannel();

	NetAddress addressToTry = NetAddress::GetLocalAddress(port);
	int idx = m_channel->Bind(addressToTry, range_to_try);

	if(idx == -1)
	{
		DevConsole::AddErrorMessage("NetSession had an error binding the NetAddress");
		return false;
	}
	else
	{
		DevConsole::AddConsoleDialogue("Bound to port: " + addressToTry.GetPortAsString(), Rgba::WHITE);
	}


	// sort the definitions alphabetically
	SortDefinitions();
	return true;
}

//-----------------------------------------------------------------------------------------------
void NetSession::SendDirectMessageTo(NetMessage &messageToSend, const NetAddress& address)
{
	// Update the index of this messageToSend
	NetMessageDefinition* msgDef = GetMessageDefinitionByName( messageToSend.m_definitionName );
	messageToSend.m_definition = msgDef;
	messageToSend.m_header.m_messageCallbackDefinitionIndex = msgDef->m_callbackID;

	// Send the Packet
	NetPacket packetToSend;
	packetToSend.m_header.m_messageCount = 1U;
	packetToSend.WriteHeader();
	packetToSend.WriteMessage( messageToSend );

	m_channel->m_socket->SendTo( address, packetToSend.GetBuffer(), packetToSend.GetWrittenByteCount() );
}

//-----------------------------------------------------------------------------------------------
// Deprecated
// NetConnection* NetSession::AddConnection(uint idx, NetAddress const & addr)
// {
// 	NetConnection* newConnection = new NetConnection((uint8_t) idx, addr, this);
// 	
// 	// make sure we use a valid idx
// 	if(idx > NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS || idx < 0)
// 	{
// 		ERROR_RECOVERABLE("Invalid Index when trying to add a new index: " + std::to_string(idx));
// 	}
// 
// 	// see if it is ok to add to index
// 	if(m_connections[idx] == nullptr)
// 	{
// 		m_connections[idx] = newConnection;
// 
// 		// gonna set the heart beat timer when we get it
// 		newConnection->SetHeartbeatTimer(m_heartbeatRate);
// 
// 		// see if we just added our own index, if so, store of the index for easy access!
// 		if(addr == m_channel->m_socket->m_address)
// 			m_connectionsIndexInSession = idx;
// 
// 		return newConnection;
// 	}
// 	else
// 	{
// 		ERROR_RECOVERABLE("There is already a connection at idx: " + std::to_string(idx));
// 	}
// 	
// 	return nullptr;
// }

//-----------------------------------------------------------------------------------------------
void NetSession::CloseAllConnections()
{
	for(uint i = 0; i < NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS; i++)
	{
		if(m_boundConnections[i] != nullptr)
		{
			delete m_boundConnections[i];
			m_boundConnections[i] = nullptr;
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
	size_t result = m_channel->m_socket->ReceiveFrom(&theAddress, tempBuffer.GetBuffer(), PACKET_MTU);
	
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
					currentMessage.m_definition = theDef;

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
					else if (theDef->m_option == NETMSSAGE_OPTION_RELIALBE_IN_ORDER)
					{
						// give it to the channel it belongs to and let it be in charge of it being processed
						NetMessageChannelIndexName theChannel = theDef->m_channelIndex;

						if(!DoesContain(currentMessage.m_header.m_reliableID, theSender->m_connection->m_receivedReliableIDs))
						{
							theSender->m_connection->m_messageChannels[theChannel]->HandleMessage( currentMessage );
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
		if(m_boundConnections[i] != nullptr)
			m_boundConnections[i]->ProcessOutgoing();
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
	size_t result = m_channel->m_socket->SendTo(
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
	title += "  (State: " + NetSessionStateToString(m_state) + ")";
	mb.Add2DRandomColoredText(pivot, title, 2.f);

	// Draw the info
	String theInfo = Stringf("Rate: %d hz || sim lag: %d ms - %d ms || sim_loss: %d",
		(int) m_sessionFlushRate,	// Flush rate
		m_latencyRange.min,			// Latency min
		m_latencyRange.max,			// Latency max
		(int) m_lossAmount);		// loss amount
	mb.Add2DRandomColoredText(Vector2(pivot.x, pivot.y - 2.f), theInfo, textSize);

	// Draw the socket bound to the session
	mb.Add2DRandomColoredText(Vector2(pivot.x, pivot.y - 4.f), "Bound SockAddr:", textSize);
	String boundSocketAddress = m_channel->m_socket->GetAddress().ToString();
	mb.Add2DRandomColoredText( Vector2(pivot.x + 1.f, pivot.y - 6.f), boundSocketAddress, textSize);

	// show all connections
	mb.Add2DRandomColoredText(Vector2(pivot.x, pivot.y - 8.f), "Connections:", textSize);
	String header = Stringf("-- %6s %-20s %-8s %-7s %-7s %-7s %-7s %-7s %-18s %-7s", 
		"idx", "address", "rtt(ms)", "loss%", "lrcv(s)", "lsnt(s)", "sntack", "rcvack", "rcvbits", "RLC");
	mb.Add2DRandomColoredText(Vector2(pivot.x + 1.f, pivot.y - 10.f), header, .8f);
	
	Vector2 currentPos = Vector2(pivot.x + 1.f, pivot.y - 12.f);
	for(uint i = 0; i < NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS; i++)
	{
		NetConnection* currentConnection = m_boundConnections[i];
		
		if(currentConnection != nullptr)
		{
			String isLocal = "";

			if(currentConnection->m_address == m_channel->m_socket->m_address)
				isLocal += "L";

			if(currentConnection == m_hostConnection)
				isLocal += "H";
			
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
	return m_boundConnections[m_connectionsIndexInSession];
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
	
	if(m_boundConnections[idx] != nullptr)
		return m_boundConnections[idx];

	return nullptr;
}

//-----------------------------------------------------------------------------------------------
NetConnection* NetSession::GetConnectionFromAddress(const NetAddress& sender) const
{
	for(uint i = 0; i < NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS; i++)
	{
		NetConnection* current = m_boundConnections[i];

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
		NetConnection* current = m_boundConnections[i];

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
		NetConnection* current = m_boundConnections[i];

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
		if(m_boundConnections[idx] != nullptr)
		{
			m_boundConnections[idx]->SetFlushRate(hz);
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
		m_reliableTimer->SetTimer(1.f);
	}


	if(m_reliableTimer->CheckAndReset())
	{
		NetConnection *cp = GetConnection( m_reliableIdx ); 

		uint currentTally = m_reliableTotalAmount - m_reliableCurrentAmount;

		NetMessage* msg = new NetMessage("sequence_test"); 
		msg->WriteBytes( sizeof(uint), &currentTally ); 
		msg->WriteBytes( sizeof(uint), &m_reliableTotalAmount ); 
		cp->Send( *msg );

		DevConsole::AddConsoleDialogue(Stringf("Sent message: %u/%u", currentTally, m_reliableTotalAmount), Rgba::WHITE);

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

//-----------------------------------------------------------------------------------------------
bool NetSession::ProcessJoinRequest(const NetConnectionInfo& connectionWantingToJoin)
{
	if(!CanWeAcceptTheConnection(connectionWantingToJoin))
	{
		// send deny
		SendJoinDeny(connectionWantingToJoin);
		return false;
	}
	else
	{
		// Accept and
		// Create and add connection (Create also binds)
		NetConnection* newConnection = CreateConnection(connectionWantingToJoin);

		// if cant create a connection send a fail
		if(newConnection->m_indexInSession == INVALID_CONNECTION_INDEX)
		{
			SendJoinDeny(connectionWantingToJoin);
		}

		// let em know its gucci
		SendJoinAccept(*newConnection);

		return true;
	}	
}

//-----------------------------------------------------------------------------------------------
bool NetSession::CanWeAcceptTheConnection(const NetConnectionInfo& connectionWantingToJoin)
{
	// not host
	if(m_myConnection->IsHost() == false)
		return false;

	// ignore if already exists and check if we have an empty slot
	bool hasRoom = false;
	for(uint i = 0; i < NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS; i++)
	{
		NetConnection* current = m_boundConnections[i];

		if(current->m_address == connectionWantingToJoin.m_address)
			return false;

		if(current == nullptr)
			hasRoom = true;
	}

	// if we didn;t have room for another, return false
	if(hasRoom == false)
		return false;

	return true;
}

//-----------------------------------------------------------------------------------------------
void NetSession::SendJoinDeny(const NetConnectionInfo& connectionWantingToJoin)
{
	NetMessage denyMessage = NetMessage("joinDeny");

	DevConsole::AddConsoleDialogue("Just sent a deny to: " + connectionWantingToJoin.m_address.ToString());

	SendDirectMessageTo(denyMessage, connectionWantingToJoin.m_address);
}

//-----------------------------------------------------------------------------------------------
void NetSession::SendJoinAccept( NetConnection& connectionWantingToJoin )
{
	// send a join accept packet
	NetMessage joinAcceptMessage;
	joinAcceptMessage.WriteBytes(1U, &connectionWantingToJoin.m_indexInSession);
	connectionWantingToJoin.Send(joinAcceptMessage);

	// send a join finished message
	NetMessage joinFinishMessage;
	connectionWantingToJoin.Send(joinFinishMessage);
}

//-----------------------------------------------------------------------------------------------
void NetSession::UpdateConnectionState(uint8_t stateToChangeTo, const NetConnection& theConnection)
{
	for(uint i = 0; i < NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS; i++)
	{
		NetConnection* current = m_boundConnections[i];

		if(current->m_address == theConnection.m_address)
		{
			current->m_state = (eNetConnectionState) stateToChangeTo;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void NetSession::SetError(eSessionError error, char const * str)
{
	m_errorCode = error;
	m_errorString = str;
}

//-----------------------------------------------------------------------------------------------
void NetSession::ClearError()
{
	m_errorCode = SESSION_OK;
	m_errorString = "No error :)";
}

//-----------------------------------------------------------------------------------------------
eSessionError NetSession::GetLastError(std::string * out_str)
{
	eSessionError currentError = m_errorCode;
	*out_str = m_errorString;
	
	ClearError();

	return currentError;
}

