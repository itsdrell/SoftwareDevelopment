#pragma once
#include "Engine/Core/General/NamedProperties.hpp"

#include <map>
#include <string>
#include <vector>

//====================================================================================
// Forward Declare
//====================================================================================
class EventSubscription;

//====================================================================================
// Type Defs + Defines
//====================================================================================
//note: the bool is (later) a signal of whether the event should be consumed by me
typedef bool(*EventFunctionCallbackPtrType) (NamedProperties& args);

typedef std::vector<EventSubscription*> EventSubscriptions;

//====================================================================================
// ENUMS
//====================================================================================


//====================================================================================
// Structs
//====================================================================================


//====================================================================================
// Classes
//====================================================================================
class EventSystem
{
public:
	static EventSystem* CreateOrGet();

public:
	int FireEvent(const std::string& eventName, NamedProperties& args);

public:
	void SubscribeEvent(const std::string& eventName, EventSubscription* subscription);
	void RemoveEventAndAllSubscriptions(const std::string& eventName);
	void UnsubscribeFunction(const std::string& eventName, EventFunctionCallbackPtrType callback);
	
	template <typename T, typename MemberFunction>
	void UnSubscribeEventMethod(const std::string& name, T& object, MemberFunction callback);

public:
	bool TestMember(NamedProperties& args);

private:
	std::map< std::string, EventSubscriptions > m_subscriptions;
};

//-----------------------------------------------------------------------------------------------
// base class
class EventSubscription
{
public:
	virtual ~EventSubscription() {}

	virtual bool Execute(NamedProperties& args) = 0;
};

//-----------------------------------------------------------------------------------------------
class EventFunctionSubscription : public EventSubscription
{
public:

	EventFunctionSubscription(EventFunctionCallbackPtrType theCallback)
		: m_functionPointer(theCallback) {}
	
	virtual bool Execute(NamedProperties& args) override
	{
		return m_functionPointer(args);
	}

public:
	//this is not a pointer, because the typedef is already a pointer.
	EventFunctionCallbackPtrType m_functionPointer = nullptr;

};

//-----------------------------------------------------------------------------------------------
template <class T>
class EventObjectMethodSub : public EventSubscription
{
public:
	typedef bool (T::*EventObjectMethodCallbackPtrType)(NamedProperties& eventArgs);

	template <typename T>
	EventObjectMethodSub(T& object, EventObjectMethodCallbackPtrType method)
		: m_object(object)
		, m_method(method)
	{};

	virtual bool Execute(NamedProperties& args) override
	{
		// we need our specific instance to call our function...
		return (m_object.*m_method)(args);
	}

public:
	T& m_object;
	
	//Game::Method(...) or Thing::Method(...)
	EventObjectMethodCallbackPtrType			m_method = nullptr;
};

//====================================================================================
// Standalone C Functions
//====================================================================================
int FireEvent(const std::string& name);
int FireEvent(const std::string& name, NamedProperties& args);

void SubscribeEventCallbackFunction(const std::string& name, EventFunctionCallbackPtrType callback);
void UnsubscribeEventCallbackFunction(const std::string& name, EventFunctionCallbackPtrType callback);

template <typename T, typename MemberFunction>
void SubscribeEventCallbackMethod(const std::string& name, T& object, MemberFunction callback);

template <typename T, typename MemberFunction>
void UnSubscribeEventCallbackMethod(const std::string& name, T& object, MemberFunction callback);

//====================================================================================
// Templates
//====================================================================================
template <typename T, typename MemberFunction>
void EventSystem::UnSubscribeEventMethod(const std::string& name, T& object, MemberFunction callback)
{
	auto theIterator = m_subscriptions.find(name);
	if (theIterator == m_subscriptions.end())
		return;

	EventSubscriptions& subscriptions = theIterator->second;

	for (uint i = 0; i < subscriptions.size(); i++)
	{
		EventObjectMethodSub<T>* current = dynamic_cast<EventObjectMethodSub<T>*> (subscriptions.at(i));

		if (current != nullptr)
		{
			if ((callback == current->m_method) && (&current->m_object == &object))
			{
				RemoveFast(i, subscriptions);
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
template <typename T, typename MemberFunction>
void SubscribeEventCallbackMethod(const std::string& name, T& object, MemberFunction callback)
{	
	typedef bool (T::*EventObjectMethodCallbackPtrType)(NamedProperties& eventArgs);
	EventObjectMethodCallbackPtrType ptr = (EventObjectMethodCallbackPtrType)(callback);

	EventObjectMethodSub<T>* newSubscription = new EventObjectMethodSub<T>(object, ptr);

	EventSystem* theEventSystem = EventSystem::CreateOrGet();
	theEventSystem->SubscribeEvent(name, newSubscription);
}

//-----------------------------------------------------------------------------------------------
template <typename T, typename MemberFunction>
void UnSubscribeEventCallbackMethod(const std::string& name, T& object, MemberFunction callback)
{
	EventSystem* theEventSystem = EventSystem::CreateOrGet();
	theEventSystem->UnSubscribeEventMethod(name, object, callback);
}

//====================================================================================
// Externs
//====================================================================================
extern EventSystem* g_theEventSystem;

//====================================================================================
// Written by Zachary Bracken : [5/6/2019]
//====================================================================================