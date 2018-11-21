#include "NetMessageChannel.hpp"
#include "Engine/Net/NetMessage.hpp"
#include "Engine/Core/Utils/MiscUtils.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "NetSession.hpp"

//-----------------------------------------------------------------------------------------------
NetMessageChannel::NetMessageChannel( NetConnection& owningConnection )
{
	m_owningConnection = &owningConnection;
}

//-----------------------------------------------------------------------------------------------
NetMessageChannel::~NetMessageChannel()
{
	DeleteElementsInVector(m_outOfOrderMessages);
}

//-----------------------------------------------------------------------------------------------
void NetMessageChannel::HandleMessage(NetMessage& messageToHandle)
{
	uint16_t sequenceID = messageToHandle.m_header.m_sequenceID;

	if(sequenceID == m_nextExpectedSequenceID)
	{
		ProcessMessage(messageToHandle);
		CheckListForMessagesToProcess();
	}
	else
	{
		// not what we need to process at the time, save for later
		NetMessage* theMessage = new NetMessage(messageToHandle);
		m_outOfOrderMessages.push_back(theMessage);
	}
}

//-----------------------------------------------------------------------------------------------
void NetMessageChannel::ProcessMessage(NetMessage& messageToHandle)
{
	// make a sender
	NetSender* theSender = new NetSender(m_owningConnection);
	
	// do the callback
	messageToHandle.m_definition->m_callback(messageToHandle, *theSender);

	// we processed the message so add it to our list!
	theSender->m_connection->m_receivedReliableIDs.push_back(messageToHandle.m_header.m_reliableID);

	// remove old ones and add highest received
	theSender->m_connection->UpdateRecievedReliableList(messageToHandle.m_header.m_reliableID);

	// update data
	m_nextExpectedSequenceID++;


	delete theSender;
}

//-----------------------------------------------------------------------------------------------
void NetMessageChannel::CheckListForMessagesToProcess()
{
	for(uint i = 0; i < m_outOfOrderMessages.size(); i++)
	{
		NetMessage* current = m_outOfOrderMessages.at(i);

		if(current->m_header.m_sequenceID == m_nextExpectedSequenceID)
		{
			ProcessMessage(*current);
			
			delete current;
			RemoveFast(i, m_outOfOrderMessages);
		}
	}
}

//-----------------------------------------------------------------------------------------------
uint16_t NetMessageChannel::GetAndIncrementSequenceID()
{
	uint16_t current = m_nextSentSequenceID;
	m_nextSentSequenceID++;
	return current;
}
