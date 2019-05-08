#include "EventSystem.hpp"
#include "Engine/Core/General/EngineCommon.hpp"
#include "DevConsole.hpp"

//===============================================================================================
EventSystem* g_theEventSystem = nullptr;

//===============================================================================================
EventSystem * EventSystem::CreateOrGet()
{
	if (g_theEventSystem == nullptr)
	{
		g_theEventSystem = new EventSystem();
		return g_theEventSystem;
	}
	
	return g_theEventSystem;
}

//-----------------------------------------------------------------------------------------------
int EventSystem::FireEvent(const std::string& eventName, NamedProperties & args)
{
	auto foundIterator = m_subscriptions.find(eventName);
	if (foundIterator == m_subscriptions.end() || foundIterator->second.size() == 0)
	{
		DevConsole::AddErrorMessage("No one was subscribed to fired event: " + eventName);
		return 0;
	}

	EventSubscriptions& subscriptions = foundIterator->second;

	for (EventSubscription* sub : subscriptions)
	{
		bool wasConsumed = sub->Execute(args);
		if(wasConsumed)
			break;
	}

	return (int) subscriptions.size();
}

//-----------------------------------------------------------------------------------------------
void EventSystem::SubscribeEvent(const std::string& eventName, EventSubscription * subscription)
{
	auto foundIterator = m_subscriptions.find(eventName);
	if (foundIterator == m_subscriptions.end())
	{
		// need to make a new one!
		EventSubscriptions newSubscriptions;
		newSubscriptions.push_back(subscription);
		m_subscriptions[eventName] = newSubscriptions;
	}
	else
	{
		std::vector<EventSubscription*>& subscriptions = foundIterator->second;
		subscriptions.push_back(subscription);
	}
}

//-----------------------------------------------------------------------------------------------
void EventSystem::RemoveEventAndAllSubscriptions(const std::string& eventName)
{
	auto foundIterator = m_subscriptions.find(eventName);
	if (foundIterator != m_subscriptions.end())
	{
		EventSubscriptions& theSubscriptions = foundIterator->second;

		for (uint i = 0; i < theSubscriptions.size(); i++)
		{
			delete theSubscriptions.at(i);
		}

		theSubscriptions.clear();
		m_subscriptions.erase(foundIterator);
	}
}

//-----------------------------------------------------------------------------------------------
void EventSystem::UnsubscribeFunction(const std::string& eventName, EventFunctionCallbackPtrType callback)
{
	auto theIterator = m_subscriptions.find(eventName);
	if (theIterator == m_subscriptions.end())
		return;
	
	EventSubscriptions& subscriptions = theIterator->second;

	for (uint i = 0; i < subscriptions.size(); i++)
	{
		EventFunctionSubscription* current = dynamic_cast<EventFunctionSubscription*> (subscriptions.at(i));

		if (current != nullptr)
		{
			if (callback == current->m_functionPointer)
			{
				RemoveFast(i, subscriptions);
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
bool EventSystem::TestMember(NamedProperties& args)
{
	UNUSED(args);
	DevConsole::AddConsoleDialogue("members work");
	return false;
}

//===============================================================================================
int FireEvent(const std::string& name)
{
	NamedProperties dummy;
	int subscriptionsCalled = EventSystem::CreateOrGet()->FireEvent(name, dummy);
	return subscriptionsCalled;
}

//-----------------------------------------------------------------------------------------------
int FireEvent(const std::string& name, NamedProperties & args)
{
	return EventSystem::CreateOrGet()->FireEvent(name, args);
}

//-----------------------------------------------------------------------------------------------
void SubscribeEventCallbackFunction(const std::string& name, EventFunctionCallbackPtrType callback)
{
	EventSubscription* newSubscription = new EventFunctionSubscription(callback);
	EventSystem::CreateOrGet()->SubscribeEvent(name, newSubscription);
}

//-----------------------------------------------------------------------------------------------
void UnsubscribeEventCallbackFunction(const std::string& name, EventFunctionCallbackPtrType callback)
{
	EventSystem* theEventSystem = EventSystem::CreateOrGet();
	theEventSystem->UnsubscribeFunction(name, callback);
}
