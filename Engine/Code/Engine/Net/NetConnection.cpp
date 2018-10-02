#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/NetMessage.hpp"
#include "Engine/Net/NetPacket.hpp"
#include "Engine/Net/NetSession.hpp"

//-----------------------------------------------------------------------------------------------
NetConnection::NetConnection(uint8_t idx, const NetAddress& theAddress, NetSession* owningSession)
{
	m_owningSession = owningSession;
	m_indexInSession = idx;
	m_address = theAddress;
}

//-----------------------------------------------------------------------------------------------
NetConnection::~NetConnection()
{
	// delete all NetMessages
	for(uint i = 0; i < m_outboundUnreliables.size(); i++)
	{
		NetMessage* current = m_outboundUnreliables[i];

		delete current;
		current = nullptr;
	}
	m_outboundUnreliables.clear();

	m_owningSession = nullptr;
}

//-----------------------------------------------------------------------------------------------
void NetConnection::ProcessOutgoing()
{
	if(m_outboundUnreliables.size() == 0U)
		return;
	
	// Create a packet, fill it with as many messages as you can, send it, repeat.
	NetPacket currentPacket;
	currentPacket.m_indexToWhoWeAreSendingTo = m_indexInSession;
	for(uint i = 0; i < m_outboundUnreliables.size(); i++)
	{		
		NetMessage& currentMessage = *m_outboundUnreliables.at(i);

		if(!currentPacket.WriteMessage(currentMessage))
		{
			// we ran out of room! we didn't write to the packet
			// but send it anyways 
			currentPacket.WriteHeader();
			m_owningSession->SendPacket(currentPacket);
			
			// reset the head
			currentPacket.ResetWrite();
			currentPacket.ResetMessageCount();
			i--; // go back since we didn;t have room
		}
	}
	
	currentPacket.WriteHeader();
	m_owningSession->SendPacket(currentPacket);
	ClearOutgoingMessages();
}

//-----------------------------------------------------------------------------------------------
void NetConnection::ClearOutgoingMessages()
{
	for(uint i = 0; i < m_outboundUnreliables.size(); i++)
	{
		delete m_outboundUnreliables[i];
		m_outboundUnreliables[i] = nullptr;
	}

	m_outboundUnreliables.clear();
}

//-----------------------------------------------------------------------------------------------
void NetConnection::Send( NetMessage& messageToSend )
{
	// this is how game code sends stuff
	
	// Here is where we look at the message and get it's definition using
	// the Session we have stored off

	NetMessageDefinition* theDef = m_owningSession->GetMessageDefinitionByName(messageToSend.m_definitionName);
	messageToSend.m_header.m_messageCallbackDefinitionIndex = (uint8_t) theDef->m_callbackID;

	// add to outgoing queue
	m_outboundUnreliables.push_back(&messageToSend);
}
