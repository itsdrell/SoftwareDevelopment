#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/NetMessage.hpp"
#include "Engine/Net/NetPacket.hpp"
#include "Engine/Net/NetSession.hpp"
#include "../Math/MathUtils.hpp"
#include "../Core/Tools/DevConsole.hpp"
#include "../Core/Tools/Clock.hpp"
#include "../Renderer/Systems/DebugRenderSystem.hpp"

//-----------------------------------------------------------------------------------------------
NetConnection::NetConnection(uint8_t idx, const NetAddress& theAddress, NetSession* owningSession)
{
	m_owningSession = owningSession;
	m_indexInSession = idx;
	m_address = theAddress;

	m_lastSendTimeMS = GetTimeInMilliseconds();
	m_lastRecievedTimeMS = GetTimeInMilliseconds();

	m_heartbeatTimer = new Timer();
	m_flushRateTimer = new Timer();
	CompareFlushRatesAndSet(); 

	CreateMessageChannels();

	SetHeartbeatTimer(HEARTBEAT_RATE);
}

//-----------------------------------------------------------------------------------------------
NetConnection::NetConnection(const NetConnectionInfo theInfo, NetSession* owningSession)
{
	m_info = theInfo;
	m_owningSession = owningSession;
	m_indexInSession = theInfo.m_sessionIndex;
	m_address = theInfo.m_address;

	m_lastSendTimeMS = GetTimeInMilliseconds();
	m_lastRecievedTimeMS = GetTimeInMilliseconds();

	m_heartbeatTimer = new Timer();
	m_flushRateTimer = new Timer();
	CompareFlushRatesAndSet(); 

	CreateMessageChannels();

	SetHeartbeatTimer(HEARTBEAT_RATE);

}

//-----------------------------------------------------------------------------------------------
void NetConnection::CreateMessageChannels()
{
	for(uint i = 0; i < NUMBER_OF_NETMESSAGE_CHANNELS; i++)
	{
		m_messageChannels[i] = new NetMessageChannel(*this);
	}
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

	delete m_heartbeatTimer;
	m_heartbeatTimer = nullptr;

	for(uint i = 0; i < NUMBER_OF_NETMESSAGE_CHANNELS; i++)
	{
		delete m_messageChannels[i];
		m_messageChannels[i] = nullptr;
	}
}

//-----------------------------------------------------------------------------------------------
bool NetConnection::IsMe() const
{
	if(m_owningSession->m_myConnection == this)
		return true;
	
	return false;
}

//-----------------------------------------------------------------------------------------------
bool NetConnection::IsHost() const
{
	if(m_owningSession->m_hostConnection == this)
		return true;
	
	return false;
}

//-----------------------------------------------------------------------------------------------
bool NetConnection::IsClient() const
{
	if(m_owningSession->m_hostConnection != this)
		return true;
	
	return false;
}

//-----------------------------------------------------------------------------------------------
void NetConnection::ProcessOutgoing()
{
	// see if we want to add a heartbeat message
	CheckHeartbeatTimer();

	// See if our flush timer is up
	if( m_flushRateTimer->CheckAndReset())
	{
		// if we have no messages, return
		if(!DoWeHaveAnyMessagesToSend())
			return;

		// send what we can fit in a packet
		Flush();

		// clean up
		ClearOutgoingMessages();
	}

}

//-----------------------------------------------------------------------------------------------
void NetConnection::Flush()
{
	// Create a packet, fill it with as many messages as you can, send it, repeat.
	NetPacket currentPacket;
	currentPacket.m_indexToWhoWeAreSendingTo = m_indexInSession;

	PacketHeader header;
	header.m_senderConnectionIndex = (uint8_t) m_owningSession->GetMySessionIndex();
	header.m_ack = GetNextAckToSend();
	m_lastSentAck = header.m_ack;
	header.m_lastRecievedAck = m_highestReceivedAck;
	header.m_previousRecievedAckBitfield = (uint16_t) m_previousReceivedAckBitfield;

	currentPacket.m_header = header;

	// Get the tracker slot for this ack being sent out
	PacketTracker* tracker = AddPacketTracker( header.m_ack );
	tracker; // unused for now - will track reliables soon;
	
	// reliables that are unconfirmed
	for(uint i = 0; i < m_sentAndUnconfirmedReliables.size(); i++)
	{
		NetMessage& currentMessage = *m_sentAndUnconfirmedReliables.at(i);
		
		if(ShouldSendReliableMessage(currentMessage))
		{
			if(currentPacket.WriteMessage(currentMessage))
			{
				tracker->AddReliable( currentPacket.m_header.m_messageCount, currentMessage.m_reliable_id);
				currentMessage.ResetAge();
			}
		}
	}

	// unsent reliables
	for(uint i = 0; i < m_unsentReliables.size(); i++)
	{
		NetMessage& currentMessage = *m_unsentReliables.at(i);

		uint sizeOfCurrentMessage = (uint)currentMessage.GetWrittenByteCount() + currentMessage.GetHeaderSize();

		if(currentPacket.HasRoom(sizeOfCurrentMessage))
		{
			currentMessage.m_reliable_id = GetAndIncrementNextReliableID();

			currentPacket.WriteMessage(currentMessage);
			tracker->AddReliable( currentPacket.m_header.m_messageCount, currentMessage.m_reliable_id);

			m_sentAndUnconfirmedReliables.push_back(&currentMessage);
			RemoveFast(i, m_unsentReliables);

			currentMessage.ResetAge();
		}
	}


	// Unreliables
	for(uint i = 0; i < m_outboundUnreliables.size(); i++)
	{		
		NetMessage& currentMessage = *m_outboundUnreliables.at(i);
		uint sizeOfCurrentMessage = (uint)currentMessage.GetWrittenByteCount() + currentMessage.GetHeaderSize();

		if(currentPacket.HasRoom(sizeOfCurrentMessage))
		{
			currentPacket.WriteMessage(currentMessage);
		}

		//if(!currentPacket.WriteMessage(currentMessage))
		//{
		//	// we ran out of room! we didn't write to the packet
		//	// but send it anyways 
		//	currentPacket.WriteHeader();
		//	m_owningSession->SendPacket(currentPacket);
		//
		//	// reset the head
		//	currentPacket.ResetWrite();
		//	currentPacket.ResetMessageCount();
		//	i--; // go back since we didn;t have room
		//}
	}

	currentPacket.WriteHeader();
	if(currentPacket.m_header.m_messageCount != 0)
	{
		m_owningSession->SendPacket(currentPacket);

		// increment ack and analytics
		IncrementSendAck();
		m_lastSendTimeMS = GetTimeInMilliseconds();
	}
	
}

//-----------------------------------------------------------------------------------------------
bool NetConnection::DoWeHaveAnyMessagesToSend()
{
	if(m_unsentReliables.size() == 0 && m_outboundUnreliables.size() == 0 && m_sentAndUnconfirmedReliables.size() == 0)
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------------------------
bool NetConnection::HasStateChanged()
{
	if(m_previousState != m_state)
	{
		m_previousState = m_state;
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------
bool NetConnection::OnReceivePacket(const PacketHeader& header, NetPacket* packet)
{
	UNUSED(packet);
	
	m_lastRecievedTimeMS = GetTimeInMilliseconds();
	uint distanceOfCurrentAckAndHighest = 0U;

	if( header.m_ack != INVALID_PACKET_ACK )
	{
		
		// update my last received size
		if(header.m_ack > m_highestReceivedAck || m_highestReceivedAck == INVALID_PACKET_ACK)
		{
			distanceOfCurrentAckAndHighest = header.m_ack - m_highestReceivedAck;
			m_highestReceivedAck = header.m_ack;
		}

	}

	if( header.m_lastRecievedAck != INVALID_PACKET_ACK )
	{
		// confirm tracked packets
		ConfirmPacketReceived( header.m_lastRecievedAck );

		// confirm previous received acks as well
		ConfirmPreviousReceivedPackets(header.m_ack, distanceOfCurrentAckAndHighest);
	}

	return true;
}

//-----------------------------------------------------------------------------------------------
void NetConnection::ConfirmPacketReceived( uint16_t ack )
{
	PacketTracker* tracker = GetTracker(ack);

	if(nullptr == tracker)
	{
		return;
	}

	// TODO: Update our RTT.  Don't set it directly
	// but instead trend toward it (think on how you smoothly approach
	// a point gameplay...)
	// ...
	// currentRTT = (.9 of currentRTT) + (.1 of newRTT)
	uint newRTT = GetTimeInMilliseconds() - tracker->m_sentMS;
	m_roundTripTime = (.9f * m_roundTripTime) + (.1f * (float) newRTT);

	// More Coming Soon!
	ConfirmReliables(*tracker);

	// it has been confirmed, so invalidate to prevent a double confirmation
	// Invalid() the header
	tracker->m_ackNumber = INVALID_PACKET_ACK; 
}

//-----------------------------------------------------------------------------------------------
void NetConnection::ConfirmReliables(const PacketTracker& theTracker)
{
	for(uint i = 0; i < theTracker.m_amountSent; i++)
	{
		uint16_t currentReliableID = theTracker.m_sentReliables[i];
		RemoveReliableIDFromList(currentReliableID);
	}
}

//-----------------------------------------------------------------------------------------------
void NetConnection::RemoveReliableIDFromList(uint16_t reliableID)
{
	for(uint i = 0; i < m_sentAndUnconfirmedReliables.size(); i++)
	{
		NetMessage* current = m_sentAndUnconfirmedReliables.at(i);

		if(current->m_reliable_id == reliableID)
		{
			delete current;
			current = nullptr;

			RemoveFast(i, m_sentAndUnconfirmedReliables);
			return;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void NetConnection::ConfirmPreviousReceivedPackets( uint16_t currentAck , uint distance)
{
	// doing two different ones because we might get an ack that needs to be placed
	// in the middle
	if(currentAck == m_highestReceivedAck)
		CreateBitFlagForHightestAck(currentAck, distance);
	else
		CreateBitFlagForNonHighestAck( currentAck );
}

//-----------------------------------------------------------------------------------------------
void NetConnection::CreateBitFlagForHightestAck( uint16_t currentAck , uint distance )
{
	UNUSED(currentAck);
	
	// what forseth does
	m_previousReceivedAckBitfield <<= distance;
	m_previousReceivedAckBitfield |= (1 << (distance - 1));

	//uint16_t newField = 0b0000'0000;
	//for(int i = SIZE_OF_BIT_FIELD - 1; i >= 0; i--)
	//{
	//	newField <<= 1;
	//	PacketTracker current = *GetTracker(i);
	//
	//	if(current.m_ackNumber == INVALID_PACKET_ACK)
	//	{
	//		newField |= 0b0000'0001;
	//	}
	//
	//}
	//
	//m_previousReceivedAckBitfield = newField;

}

//-----------------------------------------------------------------------------------------------
void NetConnection::CreateBitFlagForNonHighestAck( uint16_t currentAck )
{
	uint distance = m_highestReceivedAck - currentAck;
	m_previousReceivedAckBitfield |= (1 << (distance - 1));
}

//-----------------------------------------------------------------------------------------------
uint16_t NetConnection::GetNextAckToSend()
{
	return m_nextAckToSend;
}

//-----------------------------------------------------------------------------------------------
void NetConnection::IncrementSendAck()
{
	m_nextAckToSend++;

	// This is to catch if we wrap and make sure to skip 0 since we are using that for our
	// invalid packet number
	if (m_nextAckToSend == INVALID_PACKET_ACK) {
		m_nextAckToSend++; 
	}
}

//-----------------------------------------------------------------------------------------------
uint16_t NetConnection::GetAndIncrementNextReliableID()
{
	uint16_t current = m_nextSentReliableID;
	m_nextSentReliableID++;
	return current;
}

//-----------------------------------------------------------------------------------------------
bool NetConnection::ShouldSendReliableMessage(const NetMessage& messageToCheck)
{
	int difference = ((int) GetTimeInMilliseconds()) - ((int) messageToCheck.m_lastSentTimeMS);
	
	if((difference >= TIME_TO_RESEND_RELIABLE_MS) && IsOldestUnconfirmedReliableWithinWindow())
		return true;

	return false;
}

//-----------------------------------------------------------------------------------------------
bool NetConnection::IsOldestUnconfirmedReliableWithinWindow()
{
	// probably should use cycle less
	return (m_nextSentReliableID - GetOldestUncomfirmedReliableID()) < RELIABLE_WINDOW;
}

//-----------------------------------------------------------------------------------------------
uint16_t NetConnection::GetOldestUncomfirmedReliableID()
{
	uint16_t currentHighest = 0;
	for(uint i = 0; i < m_sentAndUnconfirmedReliables.size(); i++)
	{
		uint16_t current = m_sentAndUnconfirmedReliables.at(i)->m_reliable_id;
		if(currentHighest < current) // probably should use cycleLess()
		{
			currentHighest = current;
		}
	}

	return currentHighest;
}

//-----------------------------------------------------------------------------------------------
void NetConnection::UpdateRecievedReliableList(uint16_t newID)
{
	// store off the highest
	if(newID > m_hightestRecievedReliableID)
		m_hightestRecievedReliableID = newID;

	// remove any that are smaller than highest - RELIABLE_WINDOW, but only if its greater than the window
	if(m_hightestRecievedReliableID > RELIABLE_WINDOW)
	{
		uint16_t threshhold = m_hightestRecievedReliableID - RELIABLE_WINDOW;
		for(uint i = 0; i < m_receivedReliableIDs.size(); i++)
		{
			uint16_t current = m_receivedReliableIDs.at(i);

			if(current < threshhold)
				RemoveFast( i, m_receivedReliableIDs );
		}
	}
}

//-----------------------------------------------------------------------------------------------
uint16_t NetConnection::GetAndIncrementNextSequenceID( NetMessageChannelIndexName channelToUse )
{
	return m_messageChannels[channelToUse]->GetAndIncrementSequenceID();
}

//-----------------------------------------------------------------------------------------------
PacketTracker* NetConnection::AddPacketTracker(uint16_t packetACK)
{
	PacketTracker *tracker = GetTracker(packetACK);

	// see if valid
	if (tracker->m_ackNumber != INVALID_PACKET_ACK) 
	{
		// this is a lost packet
		m_lossTally++;
	} 

	// check if we got to the end of the ring buffer, then calc loss
	TryToUpdateLoss(packetACK);

	//tracker->SetupForAck( packetACK ); 
	tracker->m_ackNumber = packetACK;
	tracker->m_sentMS = GetTimeInMilliseconds();
	return tracker; 
}

//-----------------------------------------------------------------------------------------------
PacketTracker* NetConnection::GetTracker(uint16_t ack)
{
	uint idxInRingBuffer = ack % TRACKED_PACKET_WINDOW_HISTORY; 

	return  &(m_trackers[idxInRingBuffer]);
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
	messageToSend.m_definition = theDef;

	// add to outgoing queue
	if(messageToSend.IsReliable())
	{
		m_unsentReliables.push_back(&messageToSend);
	}
	else if (messageToSend.IsReliableInOrder())
	{
		messageToSend.m_header.m_sequenceID = GetAndIncrementNextSequenceID(theDef->m_channelIndex);
		m_unsentReliables.push_back(&messageToSend);
	}
	else
	{
		m_outboundUnreliables.push_back(&messageToSend);
	}
}

//-----------------------------------------------------------------------------------------------
void NetConnection::CheckHeartbeatTimer()
{
	if(m_heartbeatTimer->CheckAndReset())
	{
		NetMessage* hb = new NetMessage( "heartbeat" ); 
		Send( *hb ); 
	}
}

//-----------------------------------------------------------------------------------------------
void NetConnection::SetHeartbeatTimer(float hz)
{
	// make sure we dont divide by 0
	if(hz == 0.f)
		hz = 1.f;
	
	// we do the conversion to seconds here
	m_heartbeatTimer->SetTimer((1.f / hz));
}

//-----------------------------------------------------------------------------------------------
void NetConnection::SetFlushRate(float hz)
{	
	// we save off what our connection flush rate is
	m_flushRateInHz = hz;

	// However, need to see which flush rate to use
	CompareFlushRatesAndSet();
}

//-----------------------------------------------------------------------------------------------
void NetConnection::CompareFlushRatesAndSet()
{
	// Here we are just gonna see if we should change our flush rate
	// because the session changed

	// if either are 0, just set the timer to 0 cause we gonna send F A S T
	if(m_owningSession->GetFlushRate() == 0.f || m_flushRateInHz == 0.f)
	{
		m_flushRateTimer->SetTimer(0.f); // send non stop
		return;
	}

	// Here we take the better rate between the session and my flush rate
	// The bigger the number, the faster because 1/1000 is small af compared to 1/.5 = 2
	float hzToUse = Max(m_owningSession->GetFlushRate(), m_flushRateInHz);

	m_flushRateTimer->SetTimer((1.f / hzToUse));
}

//-----------------------------------------------------------------------------------------------
void NetConnection::TryToUpdateLoss( uint16_t packetACK )
{
	uint idxInRingBuffer = packetACK % TRACKED_PACKET_WINDOW_HISTORY; 

	// we calculate loss whenever we reach the end of the ring buffer
	if(idxInRingBuffer == (TRACKED_PACKET_WINDOW_HISTORY - 1))
	{
		UpdateLoss();
	}
}

//-----------------------------------------------------------------------------------------------
void NetConnection::UpdateLoss()
{
	m_loss = ((float) m_lossTally / (float) TRACKED_PACKET_WINDOW_HISTORY);
	m_lossTally = 0U;
}

//-----------------------------------------------------------------------------------------------
float NetConnection::GetLoss()
{
	return m_loss;
}

//-----------------------------------------------------------------------------------------------
float NetConnection::GetRTT()
{
	return m_roundTripTime;
}

void PacketTracker::AddReliable( uint8_t idx, uint16_t ID )
{
	if(!(idx >= MAX_RELIBALES_PER_PACKET))
	{
		m_sentReliables[idx] = ID;
		m_amountSent++;
	}
}
