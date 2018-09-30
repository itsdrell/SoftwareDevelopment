#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "../Core/General/EngineCommon.hpp"
#include "NetAddress.hpp"
#include "NetMessage.hpp"

//-----------------------------------------------------------------------------------------------
NetSession::NetSession()
{
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
		return m_messageCallbacks.at(idx);
	}
	else
	{
		ERROR_RECOVERABLE("Could not find a NetMessage at index: " + std::to_string(idx));
		return nullptr;
	}
	
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
	// this is probs wrong. sorry future me
	
	for(uint i = 0; i < m_messageCallbacks.size(); i++)
	{
		for(uint j = 0; j < m_messageCallbacks.size() - 1; i++)
		{
			NetMessageDefinition* a = m_messageCallbacks.at(i);
			NetMessageDefinition* b = m_messageCallbacks.at(j);

			if(a->m_callbackName > b->m_callbackName)
			{
				NetMessageDefinition* temp = a;
				a = b;
				b = temp;
			}
		}
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
		ERROR_RECOVERABLE("NetSession had an error binding the NetAddress to Packet");
	}


	// sort the definitions alphabetically
	SortDefinitions();

}

//-----------------------------------------------------------------------------------------------
NetConnection* NetSession::AddConnection(uint idx, NetAddress const & addr)
{
	NetConnection* newConnection = new NetConnection(idx, addr, this);
	
	// make sure we use a valid idx
	if(idx > NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS || idx < 0)
	{
		ERROR_RECOVERABLE("Invalid Index when trying to add a new index: " + std::to_string(idx));
	}

	// see if it is ok to add to index
	if(m_connections[idx] == nullptr)
	{
		m_connections[idx] = newConnection;
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

	NetPacket incomingPacket;
	NetAddress theAddress;
	m_channel.m_socket->ReceiveFrom(&theAddress, incomingPacket.GetBuffer(), PACKET_MTU);

	// DON'T DO THIS THIS ASSIGNMENT, PROCESS EVERYTHING
	// Check to see if the address is a valid connect (in our list)
	//if(!IsAddressValid(theAddress))
	//{
	//	// someone who is not connected to us sent us stuff
	//	// delete packet and return
	//}

	

	// See if packet is valid
	if(!incomingPacket.IsValid())
	{
		// throw the packet away
		return;
	}

	ProcessPacket(incomingPacket);
	

	// delete buffer at the end
	//delete incomingPacket;
}

//-----------------------------------------------------------------------------------------------
void NetSession::ProcessPacket( NetPacket& packet )
{
	// Get Packet Header
	packet.ReadHeader(&packet.m_header);

	// See who sent it
	NetSender theSender = NetSender(*GetConnection(packet.m_header.m_senderConnectionIndex));

	// Get How many messages are in the packet
	uint8_t amount = packet.m_header.m_unreliableCount;
	
	// For x amount of messages, 
	for(uint i = 0; i < amount; i++)
	{
		NetMessage currentMessage;
		packet.ReadMessage(&currentMessage);

		NetMessageDefinition* theDef = GetMessageDefinitionByIndex(currentMessage.m_header.m_messageCallbackDefinitionIndex);
		theDef->m_callback(currentMessage, theSender);
	}
}

//-----------------------------------------------------------------------------------------------
void NetSession::ProcessOutgoing()
{
	// foreach connection, process outgoing; 
	for(uint i = 0; i < NET_SESSION_MAX_AMOUNT_OF_CONNECTIONS; i++)
	{
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
uint8_t NetSession::GetMyConnection() const
{
	return INVALID_CONNECTION_INDEX;
}

//-----------------------------------------------------------------------------------------------
uint8_t NetSession::GetConnectionForPacket(const PacketHeader& theHeader, const NetAddress& theAddressOfSender)
{
	return INVALID_CONNECTION_INDEX;
}

//-----------------------------------------------------------------------------------------------
NetConnection* NetSession::GetConnection(int idx)
{
	if(m_connections[idx] != nullptr)
		return m_connections[idx];

	return nullptr;
}
