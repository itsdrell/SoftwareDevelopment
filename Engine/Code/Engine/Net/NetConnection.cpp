#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/NetMessage.hpp"
#include "Engine/Net/NetPacket.hpp"
#include "Engine/Net/NetSession.hpp"
#include "../Math/MathUtils.hpp"
#include "../Core/Tools/DevConsole.hpp"
#include "../Core/Tools/Clock.hpp"

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
		if(m_outboundUnreliables.size() == 0U)
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
	header.m_previousRecievedAckBitfield = m_previousReceivedAckBitfield;

	currentPacket.m_header = header;

	// Get the tracker slot for this ack being sent out
	PacketTracker* tracker = AddPacketTracker( header.m_ack );
	tracker; // unused for now - will track reliables soon;
	
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

	// increment ack and analytics
	IncrementSendAck();
	m_lastSendTimeMS = GetTimeInMilliseconds();
}

//-----------------------------------------------------------------------------------------------
bool NetConnection::OnReceivePacket(const PacketHeader& header, NetPacket* packet)
{
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

	// it has been confirmed, so invalidate to prevent a double confirmation
	// Invalid() the header
	tracker->m_ackNumber = INVALID_PACKET_ACK; 
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

	// add to outgoing queue
	m_outboundUnreliables.push_back(&messageToSend);
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
